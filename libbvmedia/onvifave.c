/*************************************************************************
    > File Name: onvifave.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年11月04日 星期二 11时54分34秒
 ************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

/**
 * @file
 * onvif video encode main file
 */
#include <libavformat/avformat.h>

#include <libbvutil/bvstring.h>
#include <libbvutil/log.h>
#include <libbvutil/opt.h>

#include <wsseapi.h>
#include "bvmedia.h"

typedef struct OnvifContext {
    BVClass *bv_class;
    char svrurl[1024];
    char *token;
    char *user;
    char *passwd;
    enum BVCodecID vcodec_id;
    BVRational video_rate;
    int width, height;
    char onvif_url[128];
    int timeout;
    struct soap *soap;
    AVFormatContext *onvif;
} OnvifContext;

#define OFFSET(x) offsetof(OnvifContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    {"token", "", OFFSET(token), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"user", "", OFFSET(user), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"passwd", "", OFFSET(passwd), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"vcodec_id", "", OFFSET(vcodec_id), BV_OPT_TYPE_INT, {.i64 = BV_CODEC_ID_H264}, 0, INT_MAX, DEC},
    {"video_rate", "", OFFSET(video_rate), BV_OPT_TYPE_VIDEO_RATE, {.str = "25"}, 0, 0, DEC},
    {"size", "set video size", OFFSET(width), BV_OPT_TYPE_IMAGE_SIZE, {.str = "hd720"}, 0, 0, DEC},
    {"timeout", "", OFFSET(timeout), BV_OPT_TYPE_INT, {.i64 = -5000}, INT_MIN, INT_MAX, DEC},

    {NULL}
};

static int onvif_probe(BVMediaContext *s, BVProbeData *p)
{
    if (bv_strstart(p->filename, "onvifave:", NULL))
        return BV_PROBE_SCORE_MAX;
    return 0;
}

static enum BVCodecID avid_to_bvid(enum AVCodecID codec_id)
{
    int i = 0;
    struct {
        enum AVCodecID avid;
        enum BVCodecID bvid;
    }IDS[] = {
        {AV_CODEC_ID_NONE, BV_CODEC_ID_NONE},
        {AV_CODEC_ID_H264, BV_CODEC_ID_H264},
        {AV_CODEC_ID_MPEG4, BV_CODEC_ID_MPEG},
        {AV_CODEC_ID_JPEG2000, BV_CODEC_ID_JPEG},
        {AV_CODEC_ID_PCM_ALAW, BV_CODEC_ID_G711A},
        {AV_CODEC_ID_PCM_MULAW, BV_CODEC_ID_G711U},
        {AV_CODEC_ID_ADPCM_G726, BV_CODEC_ID_G726},
        {AV_CODEC_ID_AAC, BV_CODEC_ID_AAC},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(IDS); i++) {
        if (IDS[i].avid == codec_id)
            return IDS[i].bvid;
    }
    return BV_CODEC_ID_NONE;
}

static void set_video_info(BVStream *bvst, AVStream *avst)
{
    bvst->codec->codec_type = BV_MEDIA_TYPE_VIDEO;
    bvst->codec->width = avst->codec->width;
    bvst->codec->height = avst->codec->height;
//    bvst->codec->time_base = (BVRational){avst->codec->time_base.den, avst->codec->time_base.num};
    bvst->time_base = (BVRational){avst->time_base.den, avst->time_base.num};
    bvst->codec->time_base = (BVRational){avst->avg_frame_rate.num, avst->avg_frame_rate.den};
    bvst->codec->bit_rate = avst->codec->bit_rate;
    bvst->codec->codec_id = avid_to_bvid(avst->codec->codec_id);
    bvst->codec->gop_size = avst->codec->gop_size;
    if (avst->codec->extradata_size > 0) {
        bvst->codec->extradata = bv_memdup(avst->codec->extradata, avst->codec->extradata_size);
        if (bvst->codec->extradata) {
            bvst->codec->extradata_size = avst->codec->extradata_size;
        }
    }
}

static int set_avcodec_info(BVMediaContext *s, OnvifContext *onvifctx)
{
    AVFormatContext *onvif = onvifctx->onvif;
    int i;
    BVStream *bvst = NULL;
    AVStream *avst = NULL;
    for (i = 0; i < onvif->nb_streams; i++) {
        avst = onvif->streams[i];
        bvst = bv_stream_new(s, NULL);
        if (avst->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            set_video_info(bvst, avst);
        }
    }
    return 0;
}
#define ONVIF_TMO (-5000)
static struct soap *bv_soap_new(OnvifContext *onvifctx)
{
    struct soap *soap = NULL;
    int timeout = onvifctx->timeout;
    soap = soap_new();
    if (soap == NULL) {
        return NULL;
    }
    soap_set_namespaces(soap, namespaces);
    if (!timeout) {
        timeout = ONVIF_TMO;
    }

    if (onvifctx->user && onvifctx->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "onvifave_user", onvifctx->user, onvifctx->passwd);
    }

    soap->recv_timeout = timeout;
    soap->send_timeout = timeout;
    soap->connect_timeout = timeout;
    return soap;
}

static void bv_soap_free(struct soap *soap)
{
    if (soap == NULL) {
        return;
    }
    soap_destroy(soap);
    soap_end(soap);
    soap_free(soap);
}


static int bv_onvif_stream_uri(OnvifContext *onvifctx)
{
    int retval = SOAP_OK;
    int ret = -1;
    char *p = NULL;
    char *q = NULL;
    struct soap *soap = onvifctx->soap;
    struct _trt__GetStreamUriResponse trt__GetStreamUriResponse;
    struct _trt__GetStreamUri trt__GetStreamUri;
    struct tt__StreamSetup StreamSetup;    /* required element of type tt:StreamSetup */
    struct tt__Transport Transport;    /* required element of type tt:Transport */

    trt__GetStreamUri.ProfileToken = onvifctx->token;
    trt__GetStreamUri.StreamSetup = &StreamSetup;
    StreamSetup.Transport = &Transport;
    StreamSetup.Stream = tt__StreamType__RTP_Unicast;
    Transport.Protocol = tt__TransportProtocol__RTSP;
    Transport.Tunnel = NULL;
    StreamSetup.__size = 1;
    StreamSetup.__any = NULL;
    StreamSetup.__anyAttribute = NULL;

    retval = soap_call___trt__GetStreamUri(soap, onvifctx->svrurl , NULL, &trt__GetStreamUri, &trt__GetStreamUriResponse);
    if (retval != SOAP_OK) {
        printf("get media url error\n");
        printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
    }  else {
        printf("get uri %s\n", trt__GetStreamUriResponse.MediaUri->Uri);
        q = trt__GetStreamUriResponse.MediaUri->Uri;
        if (onvifctx->user && onvifctx->passwd) {
            char tmp[128] = { 0 };
            bv_sprintf(tmp, sizeof(tmp), "%s:%s@", onvifctx->user, onvifctx->passwd);
            p = bv_sinsert(q, "://", tmp); 
            bv_strlcpy(onvifctx->onvif_url, p, sizeof(onvifctx->onvif_url));
            bv_free(p);
         } else {
            bv_strlcpy(onvifctx->onvif_url, q, sizeof(onvifctx->onvif_url));
        }
        ret = 0;
    }
    return ret;
}


static int bv_onvif_snapshot_uri(OnvifContext *onvifctx)
{
    int retval = SOAP_OK;
    int ret = -1;
    struct soap *soap = onvifctx->soap;
    struct _trt__GetSnapshotUri trt__GetSnapshotUri;
    struct _trt__GetSnapshotUriResponse trt__GetSnapshotUriResponse;

    trt__GetSnapshotUri.ProfileToken = onvifctx->token;

    retval = soap_call___trt__GetSnapshotUri(soap, onvifctx->svrurl, NULL, &trt__GetSnapshotUri, &trt__GetSnapshotUriResponse);
    if(retval != SOAP_OK) {
        printf("get SnapshotUri error");
        printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
    } else {
        strncpy(onvifctx->onvif_url, trt__GetSnapshotUriResponse.MediaUri->Uri, sizeof(onvifctx->onvif_url));
        ret = 0;
    }

    return ret;
}

//url like onvifave://192.168.6.149:8899/onvif/device_serveces
static bv_cold int onvif_read_header(BVMediaContext * s)
{
    int ret;
    char *p;
    OnvifContext *onvifctx = s->priv_data;
    if (!onvifctx->token) {
        bv_log(s, BV_LOG_ERROR, "must set onvifave token url\n");
        return BVERROR(EINVAL);
    }
    onvifctx->soap = bv_soap_new(onvifctx);
    if (!onvifctx->soap)
        return BVERROR(ENOMEM);
    p = bv_sreplace(s->filename, "onvifave", "http");
    if (!p) {
        return BVERROR(ENOMEM);
    }
    bv_strlcpy(onvifctx->svrurl, p, sizeof(onvifctx->svrurl));
    bv_free(p);
    bv_log(s, BV_LOG_INFO, "svr %s\n", onvifctx->svrurl);
    if (onvifctx->vcodec_id == BV_CODEC_ID_H264 || onvifctx->vcodec_id == BV_CODEC_ID_MPEG) {
        ret = bv_onvif_stream_uri(onvifctx);
    } else if (onvifctx->vcodec_id == BV_CODEC_ID_JPEG) {
        ret = bv_onvif_snapshot_uri(onvifctx);
    } else {
        bv_log(onvifctx, BV_LOG_ERROR, "video codec id error\n");
        return -1;
    }

    if(ret < 0) {
        bv_log(onvifctx, BV_LOG_ERROR, "onvif get stream uri error\n");
        return -1;
    }
    /**
     *  RTSP 流的处理协议的解析等等 现在使用FFmpeg中的，已有的功能。
     *  抓拍流是HTTP的协议也是用FFmpeg中的
     *  自己开发估计耗时太久。
     *  FIXME
     *      依赖 libavformat libavcodec libavutil
     *  以后可能RTSP协议处理会添加在libbvprotocol中
     */
    if(avformat_open_input(&onvifctx->onvif, onvifctx->onvif_url, NULL, NULL) < 0) {
        bv_log(onvifctx, BV_LOG_ERROR, "open onvif stream error\n");
        return -1;
    }
    if (avformat_find_stream_info(onvifctx->onvif, NULL) < 0) {
        bv_log(onvifctx, BV_LOG_ERROR, "get onvif stream info error\n");
        return -1;
    }

    av_dump_format(onvifctx->onvif, 0, onvifctx->onvif_url, 0);
    set_avcodec_info(s, onvifctx);
    return 0;
}

static int copy_data(BVPacket *pkt, AVPacket *rpkt)
{
    if (bv_packet_new(pkt, rpkt->size) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "NoMem at %s %d\n", __FILE__, __LINE__);
        return BVERROR(ENOMEM);
    }
    memcpy(pkt->data, rpkt->data, rpkt->size);
    pkt->size = rpkt->size;
    pkt->pts = rpkt->pts;
    pkt->dts = rpkt->dts;
    if (rpkt->flags & AV_PKT_FLAG_KEY)
        pkt->flags |= BV_PKT_FLAG_KEY;
    pkt->stream_index = rpkt->stream_index;
    return 0;
}
static int onvif_read_packet(BVMediaContext * s, BVPacket * pkt)
{
    AVPacket rpkt;
    OnvifContext *onvifctx = s->priv_data;
    if (onvifctx->onvif == NULL) {
        return BVERROR(EINVAL);
    }
    av_init_packet(&rpkt);
    if(av_read_frame(onvifctx->onvif, &rpkt) < 0) {
        return BVERROR(EIO);
    }
#if 0
    in_stream = onvifctx->onvif->streams[rpkt.stream_index];
    index = bv_find_best_stream(s, in_stream->codec->codec_type, -1, -1, NULL, 0);
    if (index < 0) {
        bv_log(s, BV_LOG_WARNING, "Not find This Type stream \n");
        return BVERROR(EINVAL);
    }
    index = rpkt.stream_index;
    out_stream = s->streams[index];
    //bv_copy_packet(pkt, &rpkt);

    pkt->pts = bv_rescale_q_rnd(rpkt.pts, in_stream->time_base, out_stream->time_base, BV_ROUND_NEAR_INF|BV_ROUND_PASS_MINMAX);
    pkt->dts = bv_rescale_q_rnd(rpkt.dts, in_stream->time_base, out_stream->time_base, BV_ROUND_NEAR_INF|BV_ROUND_PASS_MINMAX);
    pkt->duration = bv_rescale_q(rpkt.duration, in_stream->time_base, out_stream->time_base);
    pkt->stream_index = index;
#endif
    copy_data(pkt, &rpkt);
    av_free_packet(&rpkt);
    return pkt->size;
}

static bv_cold int onvif_read_close(BVMediaContext * s)
{
    OnvifContext *onvifctx = s->priv_data;
    avformat_close_input(&onvifctx->onvif);
    bv_soap_free(onvifctx->soap);
    return 0;
}

static bv_cold int onvif_control(BVMediaContext *s, int type, BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return 0;
}

static const BVClass onvif_class = {
    .class_name = "onvifave indev",
    .item_name = bv_default_item_name,
    .option = options,
    .version = LIBBVUTIL_VERSION_INT,
    .category = BV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
};

BVInputMedia bv_onvifave_demuxer = {
    .name = "onvifave",
    .priv_data_size = sizeof(OnvifContext),
    .read_probe     = onvif_probe,
    .read_header = onvif_read_header,
    .read_packet = onvif_read_packet,
    .read_close = onvif_read_close,
    .control_message = onvif_control,
    .flags = BV_MEDIA_FLAG_NOFILE,
    .priv_class = &onvif_class,
};