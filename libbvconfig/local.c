/*************************************************************************
    > File Name: local.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月13日 星期二 10时56分35秒
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
 * Copyright (C) albert@BesoVideo, 2015
 */

#line 25 "local.c"

#include <libbvutil/bvstring.h>

#include "bvconfig.h"

typedef struct _LocalContext {
    const BVClass       *bv_class;
    char                value[512]; //warning:must enough to save string
    int64_t             lvalue;
}LocalContext;



#define DETERMINE_FIELD_IS_OR_NOT_EXIST(object, field, ret)  \
        do { \
            if (!object) { \
                bv_log(h, BV_LOG_ERROR, "get member[%s] error", field); \
                ret = BVERROR(EINVAL); \
                goto error; \
            } else { \
                bv_log(h, BV_LOG_DEBUG, "get member[%s] done\n", field); \
            } \
        } while (0);


#define DETERMINE_INDEX_IS_OR_NOT_VALID(element, index, ret) \
        do { \
            if (!element) { \
                bv_log(h, BV_LOG_ERROR, "get element index[%d] error\n", index); \
                ret = BVERROR(EINVAL); \
                goto error; \
            } else { \
                bv_log(h, BV_LOG_DEBUG, "get element index[%d] done\n", index); \
            } \
        } while (0);


#define GET_VALUE(PARENT, KEY, VALUE_STR, VALUE_INT) \
    do { \
        BVConfigObject *memb = bv_config_get_member(h->pdb, PARENT, KEY); \
        if (!memb) {    \
            bv_log(h, BV_LOG_ERROR, "get key %s error\n", KEY); \
            break; \
        } \
        if (memb->type == BV_CONFIG_OBJTYPE_STRING) { \
            if (bv_config_object_get_value(h->pdb, memb, localctx->value) == 0) { \
                bv_strlcpy(VALUE_STR, localctx->value, sizeof(VALUE_STR)); \
                bv_log(h, BV_LOG_DEBUG, "get key %s value %s\n", KEY, localctx->value); \
            } \
            else { \
                bv_log(h, BV_LOG_ERROR, "get key %s value error\n", KEY); \
            } \
        } else { \
            if (bv_config_object_get_value(h->pdb, memb, &localctx->lvalue) == 0) {  \
                VALUE_INT = localctx->lvalue;  \
                bv_log(h, BV_LOG_DEBUG, "get key %s value %lld\n", KEY, localctx->lvalue); \
            } else { \
                bv_log(h, BV_LOG_ERROR, "get key %s value error\n", KEY); \
            } \
        }  \
        bv_config_object_decref(h->pdb, memb); \
    } while (0);


#define SET_VALUE(PARENT, KEY, VALUE_STR, VALUE_INT) \
    do { \
        BVConfigObject *memb = bv_config_get_member(h->pdb, PARENT, KEY); \
        if (!memb) { \
            bv_log(h, BV_LOG_ERROR, "get key %s error\n", KEY); \
            break; \
        } \
        if (memb->type == BV_CONFIG_OBJTYPE_STRING) { \
            bv_strlcpy(localctx->value, VALUE_STR, sizeof(VALUE_STR)); \
            if (bv_config_object_set_value(h->pdb, memb, localctx->value) == 0) { \
                bv_log(h, BV_LOG_DEBUG, "set key %s value %s\n", KEY, localctx->value); \
            } else { \
                bv_log(h, BV_LOG_ERROR, "set key %s value error\n", KEY); \
            } \
        } else { \
            localctx->lvalue = VALUE_INT;  \
            if (bv_config_object_set_value(h->pdb, memb, &localctx->lvalue) == 0) { \
                bv_log(h, BV_LOG_DEBUG, "set key %s value %lld\n", KEY, localctx->lvalue); \
            } else { \
                bv_log(h, BV_LOG_ERROR, "set key %s value error\n", KEY); \
            } \
        } \
        bv_config_object_decref(h->pdb, memb); \
    } while (0);

static int tmp = 0;

static int local_probe(BVConfigContext *h, BVProbeData *p)
{
    if (bv_strstart(p->filename, "local:", NULL))
        return BV_PROBE_SCORE_MAX;
    return 0;
}

//url like: test.json.
static int local_open(BVConfigContext *h)
{
    int ret = 0;
    bv_log(h, BV_LOG_DEBUG, "open config, return [%d]\n", ret);
    return ret;
}

static int local_close(BVConfigContext *h)
{
    int ret = 0;
    bv_log(h, BV_LOG_DEBUG, "close config, return [%d]\n", ret);
    return ret;
}

static int local_get_device_info(BVConfigContext *h, BVDeviceInfo *devinfo)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "device_info");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "device_info", ret);

    GET_VALUE(obj, "device_model", devinfo->device_model, tmp);
    GET_VALUE(obj, "device_type", devinfo->device_type, tmp);
    GET_VALUE(obj, "software_version", devinfo->software_version, tmp);
    GET_VALUE(obj, "hardware_version", devinfo->hardware_version, tmp);
    GET_VALUE(obj, "hardware_model", devinfo->hardware_model, tmp);
    GET_VALUE(obj, "manufacturer_id", devinfo->manufacturer_id, tmp);
    GET_VALUE(obj, "manufacturer", devinfo->manufacturer_name, tmp);
    GET_VALUE(obj, "device_id", devinfo->device_id, tmp);
    GET_VALUE(obj, "device_name", devinfo->device_name, tmp);
    GET_VALUE(obj, "wifi_count", NULL, devinfo->wifi_count);
    GET_VALUE(obj, "wireless_count", NULL, devinfo->wireless_count);
    GET_VALUE(obj, "channel_count", NULL, devinfo->channel_count);
    GET_VALUE(obj, "video_source_devices", NULL, devinfo->video_source_devices);
    GET_VALUE(obj, "video_output_devices", NULL, devinfo->video_output_devices);
    GET_VALUE(obj, "audio_source_devices", NULL, devinfo->audio_source_devices);
    GET_VALUE(obj, "audio_output_devices", NULL, devinfo->audio_output_devices);
    GET_VALUE(obj, "video_source", NULL, devinfo->video_sources);
    GET_VALUE(obj, "video_outputs", NULL, devinfo->video_outputs);
    GET_VALUE(obj, "audio_sources", NULL, devinfo->audio_sources);
    GET_VALUE(obj, "audio_outputs", NULL, devinfo->audio_outputs);
    GET_VALUE(obj, "serial_ports", NULL, devinfo->serial_ports);
    GET_VALUE(obj, "alert_in_count", NULL, devinfo->alert_in_count);
    GET_VALUE(obj, "alert_out_count", NULL, devinfo->alert_out_count);
    GET_VALUE(obj, "storage_count", NULL, devinfo->storage_count);
    GET_VALUE(obj, "relay_outputs", NULL, devinfo->relay_outputs);
    GET_VALUE(obj, "ptz_count", NULL, devinfo->ptz_count);
    GET_VALUE(obj, "gps_count", NULL, devinfo->gps_count);
    GET_VALUE(obj, "support_sms", NULL, devinfo->support_sms);
    GET_VALUE(obj, "support_call", NULL, devinfo->support_call);
    GET_VALUE(obj, "preset_count", NULL, devinfo->preset_count);
    GET_VALUE(obj, "cruise_count", NULL, devinfo->cruise_count);
    GET_VALUE(obj, "temperature_sensor", NULL, devinfo->temperature_count);
    GET_VALUE(obj, "voltage_count", NULL, devinfo->voltage_count);
    GET_VALUE(obj, "speed_count", NULL, devinfo->speed_count);

error:
    if (obj) 
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_video_encoder(BVConfigContext *h, int channel, int index, BVVideoEncoder *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_encoders");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "video_encoders", ret);

    elem = bv_config_get_element(h->pdb, obj, channel);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, channel, ret);

    elem2 = bv_config_get_element(h->pdb, elem, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem2, index, ret);

    GET_VALUE(elem2, "token", config->token, tmp);
    GET_VALUE(elem2, "width", NULL, config->codec_context.width);
    GET_VALUE(elem2, "height", NULL, config->codec_context.height);
    GET_VALUE(elem2, "quality", NULL, config->codec_context.quality);
    GET_VALUE(elem2, "gop", NULL, config->codec_context.gop_size);
    GET_VALUE(elem2, "bitrate", NULL, config->codec_context.bit_rate);
    GET_VALUE(elem2, "framerate", NULL, config->codec_context.time_base.den);
    config->codec_context.time_base.num = 1;

    GET_VALUE(elem2, "encoding", localctx->value, tmp);
    if (bv_strcasecmp(localctx->value, "H264") == 0) {
        config->codec_context.codec_id = BV_CODEC_ID_H264;
    } else if (bv_strcasecmp(localctx->value, "MPEG") == 0) {
        config->codec_context.codec_id = BV_CODEC_ID_MPEG;
    } else if (bv_strcasecmp(localctx->value, "JPEG") == 0) {
        config->codec_context.codec_id = BV_CODEC_ID_JPEG;
    }

    GET_VALUE(elem2, "rate_control", localctx->value, tmp);
    if (bv_strcasecmp(localctx->value, "VBR") == 0) {
        config->codec_context.mode_id = BV_RC_MODE_ID_VBR;
    } else if (bv_strcasecmp(localctx->value, "CBR") == 0) {
        config->codec_context.mode_id = BV_RC_MODE_ID_CBR;
    } else if (bv_strcasecmp(localctx->value, "ABR") == 0) {
        config->codec_context.mode_id = BV_RC_MODE_ID_ABR;
    } else if (bv_strcasecmp(localctx->value, "FIXQP") == 0) {
        config->codec_context.mode_id = BV_RC_MODE_ID_FIXQP;
    } else if (bv_strcasecmp(localctx->value, "BUTT") == 0) {
        config->codec_context.mode_id = BV_RC_MODE_ID_BUTT;
    }

error:
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_set_video_encoder(BVConfigContext *h, int channel, int index, BVVideoEncoder *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_encoders");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "video_encoders", ret);

    elem = bv_config_get_element(h->pdb, obj, channel);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, channel, ret);

    elem2 = bv_config_get_element(h->pdb, elem, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem2, index, ret);

    switch (config->codec_context.codec_id) {
        case BV_CODEC_ID_H264:
        {
            SET_VALUE(elem2, "encoding", "H264", tmp);
            break;
        }
        case BV_CODEC_ID_MPEG:
        {
            SET_VALUE(elem2, "encoding", "MPEG", tmp);
            break;
        }
        case BV_CODEC_ID_JPEG:
        {
            SET_VALUE(elem2, "encoding", "JPEG", tmp);
            break;
        }
    }

    SET_VALUE(elem2, "token", config->token, tmp);
    SET_VALUE(elem2, "width", NULL, config->codec_context.width);
    SET_VALUE(elem2, "height", NULL, config->codec_context.height);
    SET_VALUE(elem2, "quality", NULL, config->codec_context.quality);
    SET_VALUE(elem2, "gop", NULL, config->codec_context.gop_size);
    SET_VALUE(elem2, "bitrate", NULL, config->codec_context.bit_rate);

    switch (config->codec_context.mode_id) {
        case BV_RC_MODE_ID_VBR:
        {
            SET_VALUE(elem2, "rate_control", "VBR", tmp);
            break;
        }
        case BV_RC_MODE_ID_CBR:
        {
            SET_VALUE(elem2, "rate_control", "CBR", tmp);
            break;
        }
        case BV_RC_MODE_ID_ABR:
        {
            SET_VALUE(elem2, "rate_control", "ABR", tmp);
            break;
        }
        case BV_RC_MODE_ID_FIXQP:
        {
            SET_VALUE(elem2, "rate_control", "FIXQP", tmp);
            break;
        }
        case BV_RC_MODE_ID_BUTT:
        {
            SET_VALUE(elem2, "rate_control", "BUTT", tmp);
            break;
        }   
    }
    
    if (config->codec_context.time_base.num == 0) {
        config->codec_context.time_base.den = 1;
        config->codec_context.time_base.num = 25;
    }
    SET_VALUE(elem2, "framerate", NULL, config->codec_context.time_base.den / config->codec_context.time_base.num);

error:
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_video_decoder(BVConfigContext *h, int channel, int index, BVVideoDecoder *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_decoders");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "video_decoders", ret);

    elem = bv_config_get_element(h->pdb, obj, channel);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, channel, ret);

    GET_VALUE(elem, "token", config->token, tmp);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_audio_decoder(BVConfigContext *h, int channel, int index, BVAudioDecoder *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;
    
    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_decoders");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "audio_decoders", ret);

    elem = bv_config_get_element(h->pdb, obj, channel);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, channel, ret);

    GET_VALUE(elem, "token", config->token, tmp);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int get_value_video_encoder_options(BVConfigContext *h, BVConfigObject *elem, BVVideoOption *options)
{
    int i = 0;
    int rett = 0;
    char *ptr = NULL;
    char *saveptr = NULL;
    char string[512] = { 0 };
    LocalContext *localctx = h->priv_data;

    GET_VALUE(elem, "framerate", localctx->value, tmp);
    rett = sscanf(localctx->value, "[%lld..%lld]", &options->framerate_range.min, &options->framerate_range.max);
    if (rett != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[framerate] value is invalid\n");
    }

    GET_VALUE(elem, "gop", localctx->value, tmp);
    rett = sscanf(localctx->value, "[%lld..%lld]", &options->gop_range.min, &options->gop_range.max);
    if (rett != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[gop] value is invalid\n");
    }

    GET_VALUE(elem, "bitrate", localctx->value, tmp);
    rett = sscanf(localctx->value, "[%lld..%lld]", &options->bitrate_range.min, &options->bitrate_range.max);
    if (rett != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[bitrate] value is invalid\n");
    }
    
    GET_VALUE(elem, "nb_resolutions", NULL, options->nb_resolutions);
    if (options->nb_resolutions <= 0) {
        bv_log(h, BV_LOG_ERROR, "member[nb_resolutions] value is invalid\n");
        return BVERROR(EINVAL);
    }
    options->resolutions = bv_mallocz(sizeof(BVVideoResolution) * options->nb_resolutions);
    if (!options->resolutions) {
        bv_log(h, BV_LOG_ERROR, "malloc failed\n");
        return BVERROR(EINVAL);
    }
    GET_VALUE(elem, "resolutions", localctx->value, tmp);
    rett = sscanf(localctx->value, "(%[^)])", string);
    if (rett != 1) {
        bv_log(h, BV_LOG_ERROR, "member[resolutions] format wrong: lack ()\n");
        return BVERROR(EINVAL);
    }
    ptr = bv_strtok(string, " ", &saveptr);
    if (!ptr) {
        i = 1;
        rett = sscanf(ptr, "%dx%d", &(options->resolutions)[0].width, &(options->resolutions)[0].height);
        if (rett != 2) {
            bv_log(h, BV_LOG_ERROR, "get member[resolutions] value is invalid\n");
        }
    } else {
        for (i = 0; i < options->nb_resolutions; i++) {
            rett = sscanf(ptr, "%dx%d", &(options->resolutions)[i].width, &(options->resolutions)[i].height);
            if (rett != 2) {
                bv_log(h, BV_LOG_ERROR, "get member[resolutions] value is invalid\n");
            }
            ptr = bv_strtok(NULL, " ", &saveptr);
            if (!ptr)
                break;
        }
    }
    return 0;
}

static int get_encoding_video_encoder_options(BVConfigContext *h, BVConfigObject *elem, BVVideoEncoderOption *config)
{
    LocalContext *localctx = h->priv_data; 

    if (bv_strcasecmp(localctx->value, "H264") == 0) {
        config->h264 = bv_mallocz(sizeof(BVVideoOption));
        if (!config->h264) {
            bv_log(h, BV_LOG_ERROR, "h264 malloc failed\n");
            return BVERROR(EINVAL);
        }
        config->h264->codec_id = BV_CODEC_ID_H264;
        get_value_video_encoder_options(h, elem, config->h264);
    } else if (bv_strcasecmp(localctx->value, "MPEG") == 0) {
        config->mpeg = bv_mallocz(sizeof(BVVideoOption));
        if (!config->mpeg) {
            bv_log(h, BV_LOG_ERROR, "mpeg malloc failed\n");
            return BVERROR(EINVAL);
        }
        config->mpeg->codec_id = BV_CODEC_ID_MPEG;
        get_value_video_encoder_options(h, elem, config->mpeg);
    } else if (bv_strcasecmp(localctx->value, "JPEG") == 0) {
        config->jpeg = bv_mallocz(sizeof(BVVideoOption));
        if (!config->jpeg) {
            bv_log(h, BV_LOG_ERROR, "jpeg malloc failed\n");
            return BVERROR(EINVAL);
        }
        config->jpeg->codec_id = BV_CODEC_ID_JPEG;
        get_value_video_encoder_options(h, elem, config->jpeg);
    }
    return 0;
}

static int local_get_video_encoder_options(BVConfigContext *h, int channel, int index, BVVideoEncoderOption *config)
{
    int i = 0;
    int max = 0;
    int ret = 0;
    int rett = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    BVConfigObject *options = NULL;
    BVConfigObject *elem3 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_encoder_options");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "video_encoders_options", ret);

    elem = bv_config_get_element(h->pdb, obj, channel);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, channel, ret);

    elem2 = bv_config_get_element(h->pdb, elem, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem2, index, ret);

    GET_VALUE(elem2, "token", config->token, tmp);
    GET_VALUE(elem2, "quality", localctx->value, tmp);
    rett = sscanf(localctx->value, "[%lld..%lld]", &config->quality.min, &config->quality.max);
    if (rett != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[quality] value is invalid\n");
    }

    options = bv_config_get_member(h->pdb, elem2, "options");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "options", ret);

    max = bv_config_get_elements(h->pdb, options);
    for (i = 0; i < max; i++) {
        elem3 = bv_config_get_element(h->pdb, options, i);
        DETERMINE_INDEX_IS_OR_NOT_VALID(elem3, i, ret);
        GET_VALUE(elem3, "encoding", localctx->value, tmp);
        get_encoding_video_encoder_options(h, elem3, config);
        bv_config_object_decref(h->pdb, elem3);
    }
    
error:
    if (options)
        bv_config_object_decref(h->pdb, options);
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int get_value_audio_encoder_options(BVConfigContext *h, BVConfigObject *elem, BVAudioEncoderOption *config)
{
    int i = 0;
    int max = 0;
    int ret = 0;
    char *ptr = NULL;
    char *saveptr = NULL;
    char string[512] = { 0 };
    char string2[512] = { 0 };
    LocalContext *localctx = h->priv_data;

    GET_VALUE(elem, "encoding", localctx->value, tmp);
    if (bv_strcasecmp(localctx->value, "G711A") == 0) {
        config->options->codec_id = BV_CODEC_ID_G711A;
    } else if (bv_strcasecmp(localctx->value, "G711U") == 0) {
        config->options->codec_id = BV_CODEC_ID_G711U;
    } else if (bv_strcasecmp(localctx->value, "G726") == 0) {
        config->options->codec_id = BV_CODEC_ID_G726;
    } else if (bv_strcasecmp(localctx->value, "AAC") == 0) {
        config->options->codec_id = BV_CODEC_ID_AAC;
    }

    GET_VALUE(elem, "bitrate", localctx->value, tmp);
    ret = sscanf(localctx->value, "(%[^)])", string);
    if (ret != 1) {
        bv_log(h, BV_LOG_ERROR, "member[bitrate] format wrong: lack ()\n");
        return BVERROR(EINVAL);
    }
    bv_strlcpy(string2, string, sizeof(string2));
    ptr = bv_strtok(string, " ", &saveptr);
    if (!ptr) {
        max = 1;
    } else {
        do {
            ++max;
            ptr = bv_strtok(NULL, " ", &saveptr);
            if (!ptr)
                break;
        } while(1);
    }
    config->options->bitrate_list.nb_int = max;
    config->options->bitrate_list.items = bv_mallocz(sizeof(int64_t) * max);
    ptr = bv_strtok(string2, " ", &saveptr);
    if (!ptr) {
        bv_log(h, BV_LOG_ERROR, "bitrate copy string is wrong\n");
    } else {
        for (i = 0; i < config->options->bitrate_list.nb_int; i++) {
            ret = sscanf(ptr, "%lld", &config->options->bitrate_list.items[i]);
            if (ret != 1) {
                bv_log(h, BV_LOG_ERROR, "get member[bitrate] value is invalid\n");
            }
            ptr = bv_strtok(NULL, " ", &saveptr);
            if (!ptr)
                break;
        }
    }

    max = 0;
    GET_VALUE(elem, "sample_rate", localctx->value, tmp);
    ret = sscanf(localctx->value, "(%[^)])", string);
    if (ret != 1) {
        bv_log(h, BV_LOG_ERROR, "member[bitrate] format wrong: lack ()\n");
        return BVERROR(EINVAL);
    }
    bv_strlcpy(string2, string, sizeof(string2));
    ptr = bv_strtok(string, " ", &saveptr);
    if (!ptr) {
        max = 1;
    } else {
        do {
            ++max;
            ptr = bv_strtok(NULL, " ", &saveptr);
            if (!ptr)
                break;
        } while(1);
    }
    config->options->sample_rate_list.nb_int = max;
    config->options->sample_rate_list.items = bv_mallocz(sizeof(int64_t) * max);
    ptr = bv_strtok(string2, " ", &saveptr);
    if (!ptr) {
        bv_log(h, BV_LOG_ERROR, "sample_rate copy string is wrong\n");
    } else {
        for (i = 0; i < config->options->sample_rate_list.nb_int; i++) {
            ret = sscanf(ptr, "%lld", &config->options->sample_rate_list.items[i]);
            if (ret != 1) {
                bv_log(h, BV_LOG_ERROR, "get member[sample_rate] value is invalid\n");
            }
            ptr = bv_strtok(NULL, " ", &saveptr);
            if (!ptr)
                break;
        }
    }
    return 0;
}


static int local_get_audio_encoder_options(BVConfigContext *h, int channel, int index, BVAudioEncoderOption *config)
{
    int i = 0;
    int max = 0;
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    BVConfigObject *options = NULL;
    BVConfigObject *elem3 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_encoder_options");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "audio_encoder_options", ret);

    elem = bv_config_get_element(h->pdb, obj, channel);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, channel, ret);

    elem2 = bv_config_get_element(h->pdb, elem, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem2, index, ret);

    GET_VALUE(elem2, "token", config->token, tmp);

    options = bv_config_get_member(h->pdb, elem2, "options");
    if (!options) {
        bv_log(h, BV_LOG_ERROR, "get member[options] error\n");
        goto error;
    }
    max = bv_config_get_elements(h->pdb, options);
    config->nb_options = max;
    config->options = bv_mallocz(sizeof(BVAudioOption) * config->nb_options);
    for (i = 0; i < max; i++) {
        elem3 = bv_config_get_element(h->pdb, options, i);
        DETERMINE_INDEX_IS_OR_NOT_VALID(elem3, i, ret);
        get_value_audio_encoder_options(h, elem3, config);
        bv_config_object_decref(h->pdb, elem3);
    }

error:
    if (options) 
        bv_config_object_decref(h->pdb, options);
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_audio_encoder(BVConfigContext *h, int channel, int index, BVAudioEncoder *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_encoders");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "audio_encoders", ret);

    elem = bv_config_get_element(h->pdb, obj, channel);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, channel, ret);

    elem2 = bv_config_get_element(h->pdb, elem, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem2, index, ret);

    GET_VALUE(elem2, "token", config->token, tmp);
    GET_VALUE(elem2, "bitrate", NULL, config->codec_context.bit_rate);
    GET_VALUE(elem2, "sample_rate", NULL, config->codec_context.sample_rate);

    GET_VALUE(elem2, "encoding", localctx->value, tmp);
    if (bv_strcasecmp(localctx->value, "G711A") == 0) {
        config->codec_context.codec_id = BV_CODEC_ID_G711A;
    } else if (bv_strcasecmp(localctx->value,"G711U") == 0) {
        config->codec_context.codec_id = BV_CODEC_ID_G711U;
    } else if (bv_strcasecmp(localctx->value, "G726") == 0) {
        config->codec_context.codec_id = BV_CODEC_ID_G726;
    } else if (bv_strcasecmp(localctx->value, "AAC") == 0)  {
        config->codec_context.codec_id = BV_CODEC_ID_AAC;
    }

error:
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_set_audio_encoder(BVConfigContext *h, int channel, int index, BVAudioEncoder *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_encoders");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "audio_encoders", ret);

    elem = bv_config_get_element(h->pdb, obj, channel);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, channel, ret);

    elem2 = bv_config_get_element(h->pdb, elem, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem2, index, ret);

    switch (config->codec_context.codec_id) {
        case BV_CODEC_ID_G711A:
        {
            SET_VALUE(elem2, "encoding", "G711A", tmp);
            break;
        }
        case BV_CODEC_ID_G711U:
        {
            SET_VALUE(elem2, "encoding", "G711U", tmp);
            break;
        }
        case BV_CODEC_ID_G726:
        {
            SET_VALUE(elem2, "encoding", "G726", tmp);
            break;
        }
        case BV_CODEC_ID_AAC:
        {
            SET_VALUE(elem2, "encoding", "AAC", tmp);
            break;
        }
    }

    SET_VALUE(elem2, "token", config->token, tmp);
    SET_VALUE(elem2, "bitrate", NULL, config->codec_context.bit_rate);
    SET_VALUE(elem2, "sample_rate", NULL, config->codec_context.sample_rate);

error:
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_ptz_device(BVConfigContext *h, int channel, int index, BVPTZDevice *config)
{
    int ret = 0;
    int rett = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *elem2 = NULL;
    BVConfigObject *rs232 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "PTZDevice");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "PTZDevice", ret);

    elem = bv_config_get_element(h->pdb, obj, channel);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, channel, ret);

    elem2 = bv_config_get_element(h->pdb, elem, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem2, index, ret);

    GET_VALUE(elem2, "url", config->token, tmp);
    GET_VALUE(elem2, "pan", localctx->value, tmp);
    rett = sscanf(localctx->value, "[%f..%f]", &config->pan_range.min, &config->pan_range.max);
    if (rett != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[pan] value is invalid\n");
    }

    GET_VALUE(elem2, "tilt", localctx->value, tmp);
    rett = sscanf(localctx->value, "[%f..%f]", &config->tilt_range.min, &config->tilt_range.max);
    if (rett != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[tilt] value is invalid\n");
    }

    GET_VALUE(elem2, "zoom", localctx->value, tmp);
    rett = sscanf(localctx->value, "[%f..%f]", &config->zoom_range.min, &config->zoom_range.max);
    if (rett != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[zoom] value is invalid\n");
    }

    GET_VALUE(elem2, "protocol", localctx->value, tmp);
    if (bv_strcasecmp(localctx->value, "PELCO_P") == 0) {
        config->protocol = BV_PTZ_PROTO_PELCO_P;
    } else if (bv_strcasecmp(localctx->value, "PELCO_D") == 0) {
        config->protocol = BV_PTZ_PROTO_PELCO_D;
    } else if (bv_strcasecmp(localctx->value, "SAMSUNG") == 0) {
        config->protocol = BV_PTZ_PROTO_SAMSUNG;
    } else if (bv_strcasecmp(localctx->value, "VISCA") == 0) {
        config->protocol = BV_PTZ_PROTO_VISCA;
    } else if (bv_strcasecmp(localctx->value, "YAAN") == 0) {
        config->protocol = BV_PTZ_PROTO_YAAN;
    }

    rs232 = bv_config_get_member(h->pdb, elem2, "rs232");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(rs232, "rs232", ret);

    GET_VALUE(rs232, "data_bits", NULL, config->rs485.data_bits);
    GET_VALUE(rs232, "baud_rate", NULL, config->rs485.baud_rate);
    GET_VALUE(rs232, "stop_bit", NULL, config->rs485.stop_bit);
    GET_VALUE(rs232, "parity", NULL, config->rs485.parity);
    GET_VALUE(rs232, "flow_control", NULL, config->rs485.flow_control);

error:
    if (rs232)
        bv_config_object_decref(h->pdb, rs232);
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_video_source_device(BVConfigContext *h, int index, BVVideoSourceDevice *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_source_devices");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "video_source_devices", ret);

    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);

    GET_VALUE(elem, "token", config->token, tmp);
    GET_VALUE(elem, "chip", config->chip, tmp);
    GET_VALUE(elem, "dev", config->dev, tmp);
    GET_VALUE(elem, "interface", config->interface, tmp);
    GET_VALUE(elem, "work_mode", config->work_mode, tmp);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_audio_source_device(BVConfigContext *h, int index, BVAudioSourceDevice *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_source_devices");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "audio_source_devices", ret);

    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);

    GET_VALUE(elem, "token", config->token, tmp);
    GET_VALUE(elem, "work_mode", config->work_mode, tmp);
    GET_VALUE(elem, "chip", config->chip, tmp);
    GET_VALUE(elem, "dev", config->dev, tmp);
    GET_VALUE(elem, "channel_mode", NULL, config->channel_mode);
    GET_VALUE(elem, "channels", NULL, config->channel_counts);
    GET_VALUE(elem, "sample_format", NULL, config->sample_format);
    GET_VALUE(elem, "sample_rate", NULL, config->sample_rate);
    GET_VALUE(elem, "sample_points", NULL, config->sample_points);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_video_output_device(BVConfigContext *h, int index, BVVideoOutputDevice *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *display = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_output_devices");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "video_output_devices", ret);

    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);

    GET_VALUE(elem, "token", config->token, tmp);
    GET_VALUE(elem, "chip", config->chip, tmp);
    GET_VALUE(elem, "dev", config->dev, tmp);
    GET_VALUE(elem, "interface", config->interface, tmp);
    GET_VALUE(elem, "work_mode", config->work_mode, tmp);

    display = bv_config_get_member(h->pdb, elem, "display");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(display, "display", ret);

    GET_VALUE(display, "left", NULL, config->display.x);
    GET_VALUE(display, "top", NULL, config->display.y);
    GET_VALUE(display, "width", NULL, config->display.width);
    GET_VALUE(display, "height", NULL, config->display.height);

error:
    if (display)
        bv_config_object_decref(h->pdb, display);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_audio_output_device(BVConfigContext *h, int index, BVAudioOutputDevice *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_output_devices");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "audio_output_devices", ret);

    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);

    GET_VALUE(elem, "token", config->token, tmp);
    GET_VALUE(elem, "chip", config->chip, tmp);
    GET_VALUE(elem, "dev", config->dev, tmp);
    GET_VALUE(elem, "work_mode", config->work_mode, tmp);
    GET_VALUE(elem, "channels_mode", NULL, config->channel_mode);
    GET_VALUE(elem, "channels", NULL, config->channel_counts);
    GET_VALUE(elem, "sample_format", NULL, config->sample_format);
    GET_VALUE(elem, "sample_rate", NULL, config->sample_rate);
    GET_VALUE(elem, "sample_points", NULL, config->sample_points);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_video_source(BVConfigContext *h, int index, BVVideoSource *config)
{
    int ret = 0;
    int rett = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *capture = NULL;
    BVConfigObject *imaging = NULL;
    BVConfigObject *elem2 = NULL;
    BVConfigObject *vision = NULL;
    BVConfigObject *elem3 = NULL;
    BVConfigObject *elem4 = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_sources");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "video_sources", ret);

    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);

    GET_VALUE(elem, "token", config->token, tmp);
    GET_VALUE(elem, "video_source_device", NULL, config->video_source_device);
    GET_VALUE(elem, "framerate", NULL, config->framerate);
    GET_VALUE(elem, "day_to_night", localctx->value, tmp);
    config->day_capture.date_time.second = 0;
    rett = sscanf(localctx->value, "%d:%d", &config->day_capture.date_time.hour, &config->day_capture.date_time.minute);
    if (rett != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[day_to_night] value is invalid\n");
    }

    GET_VALUE(elem, "night_to_day", localctx->value, tmp);
    config->night_capture.date_time.second = 0;
    rett = sscanf(localctx->value, "%d:%d", &config->night_capture.date_time.hour, &config->night_capture.date_time.minute);
    if (rett != 2) {
        bv_log(h, BV_LOG_ERROR, "get member[night_to_day] value is invalid\n");
    }

    capture = bv_config_get_member(h->pdb, elem, "capture_rect");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(capture, "capture_rect", ret);

    GET_VALUE(capture, "left", NULL, config->bounds.x);
    GET_VALUE(capture, "top", NULL, config->bounds.y);
    GET_VALUE(capture, "width", NULL, config->bounds.width);
    GET_VALUE(capture, "height", NULL, config->bounds.height);

    imaging = bv_config_get_member(h->pdb, h->pdb->root, "imaging_setting");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(imaging, "imaging_setting", ret);

    elem2 = bv_config_get_element(h->pdb, imaging, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem2, index, ret);

    vision = bv_config_get_member(h->pdb, elem2, "vision_control");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(vision, "vision_control", ret);

    elem3 = bv_config_get_element(h->pdb, vision, 0);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem3, 0, ret);

    GET_VALUE(elem3, "luminance", NULL, config->day_capture.imaging.luminance);
    GET_VALUE(elem3, "contrast", NULL, config->day_capture.imaging.contrast);
    GET_VALUE(elem3, "hue", NULL, config->day_capture.imaging.hue);
    GET_VALUE(elem3, "saturation", NULL, config->day_capture.imaging.saturation);
    GET_VALUE(elem3, "sharpness", NULL, config->day_capture.imaging.sharpness);

    elem4 = bv_config_get_element(h->pdb, vision, 1);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem4, 1, ret);    

    GET_VALUE(elem4, "luminance", NULL, config->night_capture.imaging.luminance);
    GET_VALUE(elem4, "contrast", NULL, config->day_capture.imaging.contrast);
    GET_VALUE(elem4, "hue", NULL, config->day_capture.imaging.hue);
    GET_VALUE(elem4, "saturation", NULL, config->day_capture.imaging.saturation);
    GET_VALUE(elem4, "sharpness", NULL, config->day_capture.imaging.sharpness);

error:
    if (elem4)
        bv_config_object_decref(h->pdb, elem4);
    if (elem3)
        bv_config_object_decref(h->pdb, elem3);
    if (vision)
        bv_config_object_decref(h->pdb, vision);
    if (elem2)
        bv_config_object_decref(h->pdb, elem2);
    if (imaging)
        bv_config_object_decref(h->pdb, imaging);
    if (capture)
        bv_config_object_decref(h->pdb, capture);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_audio_source(BVConfigContext *h, int index, BVAudioSource *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_sources");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "audio_sources", ret);

    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);

    GET_VALUE(elem, "token", config->token, tmp);
    GET_VALUE(elem, "audio_source_device", NULL, config->audio_source_device);
    GET_VALUE(elem, "channels", NULL, config->channels);
    GET_VALUE(elem, "volume", NULL, config->volume);
    GET_VALUE(elem, "sample_rate", NULL, config->sample_rate);
    GET_VALUE(elem, "input_type", localctx->value, tmp);
    if (bv_strcasecmp(localctx->value, "mic") == 0) {
        config->input_type = BV_AUDIO_INPUT_TYPE_MIC;
    } else if (bv_strcasecmp(localctx->value, "line_in") == 0) {
        config->input_type = BV_AUDIO_INPUT_TYPE_LINE_IN;
    }

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_video_output(BVConfigContext *h, int index, BVVideoOutput *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    BVConfigObject *display = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "video_outputs");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "video_outputs", ret);

    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);

    GET_VALUE(elem, "token", config->token, tmp);
    GET_VALUE(elem, "video_output_device", NULL, config->video_output_device);

    display = bv_config_get_member(h->pdb, elem, "display");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(display, "display", ret);

    GET_VALUE(display, "left", NULL, config->display.x);
    GET_VALUE(display, "top", NULL, config->display.y);
    GET_VALUE(display, "width", NULL, config->display.width);
    GET_VALUE(display, "height", NULL, config->display.height);


error:
    if (display)
        bv_config_object_decref(h->pdb, display);
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_audio_output(BVConfigContext *h, int index, BVAudioOutput *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "audio_outputs");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "audio_outputs", ret);

    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);

    GET_VALUE(elem, "token", config->token, tmp);
    GET_VALUE(elem, "volume", NULL, config->volume);
    GET_VALUE(elem, "channels", NULL, config->channels);
    GET_VALUE(elem, "audio_output_device", NULL, config->audio_output_device);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_media_encoder(BVConfigContext *h, int index, BVMediaEncoder *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "media_encoders");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "media_encoders", ret);
    
    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);

    GET_VALUE(elem, "video_source", NULL, config->video_source);
    GET_VALUE(elem, "audio_source", NULL, config->audio_source);
    GET_VALUE(elem, "video_channel", NULL, config->video_channel);
    GET_VALUE(elem, "audio_channel", NULL, config->audio_channel);
    GET_VALUE(elem, "storage_index", NULL, config->storage_index);
    GET_VALUE(elem, "transfer_index", NULL, config->transfer_index);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_media_decoder(BVConfigContext *h, int index, BVMediaDecoder *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "media_decoders");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "media_decoders", ret);

    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);

    GET_VALUE(elem, "video_output", NULL, config->video_output);
    GET_VALUE(elem, "audio_output", NULL, config->audio_output);
    GET_VALUE(elem, "video_channel", NULL, config->video_channel);
    GET_VALUE(elem, "audio_channel", NULL, config->audio_channel);

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_talkback(BVConfigContext *h, int index, BVTalkBack *config)
{
    int ret = 0;
    int rett = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "talkbacks");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "talkbacks", ret);

    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);

    GET_VALUE(elem, "media_encoder", NULL, config->media_encoder_index);
    GET_VALUE(elem, "media_decoder", NULL, config->media_decoder_index);

    if ((rett = local_get_media_encoder(h, config->media_encoder_index, &config->media_encoder)) < 0) {
        bv_log(h, BV_LOG_ERROR, "get member[media_encoders] in talk back error, return [%d]\n", rett);
    }
    if ((rett = local_get_media_decoder(h, config->media_decoder_index, &config->media_decoder)) < 0) {
        bv_log(h, BV_LOG_ERROR, "get member[media_decoders] in talk back error, return [%d]\n", rett);
    }

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_get_media_device(BVConfigContext *h, int index, BVMediaDevice *config)
{
    int ret = 0;
    int rett = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "media_devices");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "media_devices", ret);

    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);

    GET_VALUE(elem, "name", config->name, tmp);
    GET_VALUE(elem, "video_encode_type", NULL, config->video_encode_type);
    GET_VALUE(elem, "audio_encode_type", NULL, config->audio_encode_type);
    GET_VALUE(elem, "video_decode_type", NULL, config->video_decode_type);
    GET_VALUE(elem, "audio_decode_type", NULL, config->audio_decode_type);

    if ((rett = local_get_media_encoder(h, index, &config->media_encoder)) < 0) {
        bv_log(h, BV_LOG_ERROR, "get member[media_encoders] in media device error, return [%d]\n", rett);
    }
    if ((rett = local_get_media_decoder(h, index, &config->media_decoder)) < 0) {
        bv_log(h, BV_LOG_ERROR, "get member[media_decoders] in media device error, return [%d]\n", rett);
    }

    if (config->video_encode_type == BV_MEDIA_STREAM_TYPE_IPC_VIDEO || config->audio_encode_type == BV_MEDIA_STREAM_TYPE_IPC_AUDIO) {
        config->devinfo = bv_mallocz(sizeof(BVMobileDevice));
        if (!config->devinfo) {
            bv_log(h, BV_LOG_ERROR, "malloc failed\n");
            goto error;
        }
        GET_VALUE(elem, "url", localctx->value, tmp);
        bv_strlcpy(((BVMobileDevice *)(config->devinfo))->url, localctx->value, sizeof(((BVMobileDevice *)(config->devinfo))->url));
            
        GET_VALUE(elem, "user", localctx->value, tmp);
        bv_strlcpy(((BVMobileDevice *)(config->devinfo))->user, localctx->value, sizeof(((BVMobileDevice *)(config->devinfo))->user));

        GET_VALUE(elem, "passwd", localctx->value, tmp);
        bv_strlcpy(((BVMobileDevice *)(config->devinfo))->pswd, localctx->value, sizeof(((BVMobileDevice *)(config->devinfo))->pswd));
    
        ((BVMobileDevice *)(config->devinfo))->timeout = 5;
    } else {
        config->devinfo = NULL;
    }

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}

static int local_set_media_device(BVConfigContext *h, int index, BVMediaDevice *config)
{
    int ret = 0;
    BVConfigObject *obj = NULL;
    BVConfigObject *elem = NULL;
    LocalContext *localctx = h->priv_data;

    obj = bv_config_get_member(h->pdb, h->pdb->root, "media_devices");
    DETERMINE_FIELD_IS_OR_NOT_EXIST(obj, "media_devices", ret);

    elem = bv_config_get_element(h->pdb, obj, index);
    DETERMINE_INDEX_IS_OR_NOT_VALID(elem, index, ret);
    
    SET_VALUE(elem, "name", config->name, tmp);
    if (config->video_encode_type == BV_MEDIA_STREAM_TYPE_IPC_VIDEO || config->audio_encode_type == BV_MEDIA_STREAM_TYPE_IPC_AUDIO) {
        bv_strlcpy(localctx->value, ((BVMobileDevice *)(config->devinfo))->user, sizeof(((BVMobileDevice *)(config->devinfo))->user));
        SET_VALUE(elem, "user", localctx->value, tmp);

        bv_strlcpy(localctx->value, ((BVMobileDevice *)(config->devinfo))->pswd, sizeof(((BVMobileDevice *)(config->devinfo))->pswd));
        SET_VALUE(elem, "passwd", localctx->value, tmp);

        bv_strlcpy(localctx->value, ((BVMobileDevice *)(config->devinfo))->url, sizeof(((BVMobileDevice *)(config->devinfo))->url));
        SET_VALUE(elem, "url", localctx->value, tmp);
    }

error:
    if (elem)
        bv_config_object_decref(h->pdb, elem);
    if (obj)
        bv_config_object_decref(h->pdb, obj);
    return ret;
}


#define OFFSET(x) offsetof(LocalContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { NULL }
};

static const BVClass local_class = {
    .class_name     = "local config",
    .item_name      = bv_default_item_name,
    .option         = options,
    .version        = LIBBVUTIL_VERSION_INT,
    .category       = BV_CLASS_CATEGORY_CONFIG,
};

BVConfig bv_local_config = {
    .name                       = "local",
    .type                       = BV_CONFIG_TYPE_LOCAL,
    .priv_data_size             = sizeof(LocalContext),
    .priv_class                 = &local_class,
    .config_probe               = local_probe,
    .config_open                = local_open,
    .config_close               = local_close,
    .get_device_info            = local_get_device_info,
    .get_video_encoder          = local_get_video_encoder,
    .set_video_encoder          = local_set_video_encoder,
    .get_video_decoder          = local_get_video_decoder,
    .get_video_encoder_options  = local_get_video_encoder_options,
    .get_audio_encoder          = local_get_audio_encoder,
    .set_audio_encoder          = local_set_audio_encoder,
    .get_audio_decoder          = local_get_audio_decoder,
    .get_audio_encoder_options  = local_get_audio_encoder_options,
    .get_ptz_device             = local_get_ptz_device,
    .get_media_device           = local_get_media_device,
    .set_media_device           = local_set_media_device,
    .get_media_encoder          = local_get_media_encoder,
    .get_media_decoder          = local_get_media_decoder,
    .get_talkback               = local_get_talkback,
    .get_video_source_device    = local_get_video_source_device,
    .get_audio_source_device    = local_get_audio_source_device,
    .get_video_output_device    = local_get_video_output_device,
    .get_audio_output_device    = local_get_audio_output_device,
    .get_video_source           = local_get_video_source,
    .get_audio_source           = local_get_audio_source,
    .get_video_output           = local_get_video_output,
    .get_audio_output           = local_get_audio_output,
};
