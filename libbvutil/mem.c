/*
 * default memory allocator for libbvutil
 * Copyright (c) 2002 Fabrice Bellard
 *
 * This file is part of BVBase.
 *
 * BVBase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVBase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVBase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * default memory allocator for libbvutil
 */

#define _XOPEN_SOURCE 600

#include "config.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#if BV_HAVE_MALLOC_H
#include <malloc.h>
#endif

#include "bvassert.h"
#include "bvutil.h"
#include "common.h"
#include "dynarray.h"
#include "intreadwrite.h"
#include "mem.h"

#ifdef MALLOC_PREFIX

#define malloc         BV_JOIN(MALLOC_PREFIX, malloc)
#define memalign       BV_JOIN(MALLOC_PREFIX, memalign)
#define posix_memalign BV_JOIN(MALLOC_PREFIX, posix_memalign)
#define realloc        BV_JOIN(MALLOC_PREFIX, realloc)
#define free           BV_JOIN(MALLOC_PREFIX, free)

void *malloc(size_t size);
void *memalign(size_t align, size_t size);
int   posix_memalign(void **ptr, size_t align, size_t size);
void *realloc(void *ptr, size_t size);
void  free(void *ptr);

#endif /* MALLOC_PREFIX */

#define ALIGN (BV_HAVE_AVX ? 32 : 16)

/* NOTE: if you want to override these functions with your own
 * implementations (not recommended) you have to link libav* as
 * dynamic libraries and remove -Wl,-Bsymbolic from the linker flags.
 * Note that this will cost performance. */

static size_t max_alloc_size= INT_MAX;

void bv_max_alloc(size_t max){
    max_alloc_size = max;
}

void *bv_malloc(size_t size)
{
    void *ptr = NULL;
#if BV_CONFIG_MEMALIGN_HACK
    long diff;
#endif

    /* let's disallow possibly ambiguous cases */
    if (size > (max_alloc_size - 32))
        return NULL;

#if BV_CONFIG_MEMALIGN_HACK
    ptr = malloc(size + ALIGN);
    if (!ptr)
        return ptr;
    diff              = ((~(long)ptr)&(ALIGN - 1)) + 1;
    ptr               = (char *)ptr + diff;
    ((char *)ptr)[-1] = diff;
#elif BV_HAVE_POSIX_MEMALIGN
    if (size) //OS X on SDK 10.6 has a broken posix_memalign implementation
    if (posix_memalign(&ptr, ALIGN, size))
        ptr = NULL;
#elif BV_HAVE_ALIGNED_MALLOC
    ptr = _aligned_malloc(size, ALIGN);
#elif BV_HAVE_MEMALIGN
#ifndef __DJGPP__
    ptr = memalign(ALIGN, size);
#else
    ptr = memalign(size, ALIGN);
#endif
    /* Why 64?
     * Indeed, we should align it:
     *   on  4 for 386
     *   on 16 for 486
     *   on 32 for 586, PPro - K6-III
     *   on 64 for K7 (maybe for P3 too).
     * Because L1 and L2 caches are aligned on those values.
     * But I don't want to code such logic here!
     */
    /* Why 32?
     * For AVX ASM. SSE / NEON needs only 16.
     * Why not larger? Because I did not see a difference in benchmarks ...
     */
    /* benchmarks with P3
     * memalign(64) + 1          3071, 3051, 3032
     * memalign(64) + 2          3051, 3032, 3041
     * memalign(64) + 4          2911, 2896, 2915
     * memalign(64) + 8          2545, 2554, 2550
     * memalign(64) + 16         2543, 2572, 2563
     * memalign(64) + 32         2546, 2545, 2571
     * memalign(64) + 64         2570, 2533, 2558
     *
     * BTW, malloc seems to do 8-byte alignment by default here.
     */
#else
    ptr = malloc(size);
#endif
    if(!ptr && !size) {
        size = 1;
        ptr= bv_malloc(1);
    }
#if BV_CONFIG_MEMORY_POISONING
    if (ptr)
        memset(ptr, BV_MEMORY_POISON, size);
#endif
    return ptr;
}

void *bv_realloc(void *ptr, size_t size)
{
#if BV_CONFIG_MEMALIGN_HACK
    int diff;
#endif

    /* let's disallow possibly ambiguous cases */
    if (size > (max_alloc_size - 32))
        return NULL;

#if BV_CONFIG_MEMALIGN_HACK
    //FIXME this isn't aligned correctly, though it probably isn't needed
    if (!ptr)
        return bv_malloc(size);
    diff = ((char *)ptr)[-1];
    bv_assert0(diff>0 && diff<=ALIGN);
    ptr = realloc((char *)ptr - diff, size + diff);
    if (ptr)
        ptr = (char *)ptr + diff;
    return ptr;
#elif BV_HAVE_ALIGNED_MALLOC
    return _aligned_realloc(ptr, size + !size, ALIGN);
#else
    return realloc(ptr, size + !size);
#endif
}

void *bv_realloc_f(void *ptr, size_t nelem, size_t elsize)
{
    size_t size;
    void *r;

    if (bv_size_mult(elsize, nelem, &size)) {
        bv_free(ptr);
        return NULL;
    }
    r = bv_realloc(ptr, size);
    if (!r && size)
        bv_free(ptr);
    return r;
}

int bv_reallocp(void *ptr, size_t size)
{
    void **ptrptr = ptr;
    void *ret;

    if (!size) {
        bv_freep(ptr);
        return 0;
    }
    ret = bv_realloc(*ptrptr, size);

    if (!ret) {
        bv_freep(ptr);
        return BVERROR(ENOMEM);
    }

    *ptrptr = ret;
    return 0;
}

void *bv_realloc_array(void *ptr, size_t nmemb, size_t size)
{
    if (!size || nmemb >= INT_MAX / size)
        return NULL;
    return bv_realloc(ptr, nmemb * size);
}

int bv_reallocp_array(void *ptr, size_t nmemb, size_t size)
{
    void **ptrptr = ptr;
    *ptrptr = bv_realloc_f(*ptrptr, nmemb, size);
    if (!*ptrptr && nmemb && size)
        return BVERROR(ENOMEM);
    return 0;
}

void bv_free(void *ptr)
{
#if BV_CONFIG_MEMALIGN_HACK
    if (ptr) {
        int v= ((char *)ptr)[-1];
        bv_assert0(v>0 && v<=ALIGN);
        free((char *)ptr - v);
    }
#elif BV_HAVE_ALIGNED_MALLOC
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

void bv_freep(void *arg)
{
    void **ptr = (void **)arg;
    bv_free(*ptr);
    *ptr = NULL;
}

void *bv_mallocz(size_t size)
{
    void *ptr = bv_malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

void *bv_calloc(size_t nmemb, size_t size)
{
    if (size <= 0 || nmemb >= INT_MAX / size)
        return NULL;
    return bv_mallocz(nmemb * size);
}

char *bv_strdup(const char *s)
{
    char *ptr = NULL;
    if (s) {
        int len = strlen(s) + 1;
        ptr = bv_realloc(NULL, len);
        if (ptr)
            memcpy(ptr, s, len);
    }
    return ptr;
}

char *bv_strndup(const char *s, size_t len)
{
    char *ret = NULL, *end;

    if (!s)
        return NULL;

    end = memchr(s, 0, len);
    if (end)
        len = end - s;

    ret = bv_realloc(NULL, len + 1);
    if (!ret)
        return NULL;

    memcpy(ret, s, len);
    ret[len] = 0;
    return ret;
}

void *bv_memdup(const void *p, size_t size)
{
    void *ptr = NULL;
    if (p) {
        ptr = bv_malloc(size);
        if (ptr)
            memcpy(ptr, p, size);
    }
    return ptr;
}

int bv_dynarray_add_nofree(void *tab_ptr, int *nb_ptr, void *elem)
{
    void **tab = *(void ***)tab_ptr;

    BV_DYNARRAY_ADD(INT_MAX, sizeof(*tab), tab, *nb_ptr, {
        tab[*nb_ptr] = elem;
        *(void ***)tab_ptr = tab;
    }, {
        return BVERROR(ENOMEM);
    });
    return 0;
}

void bv_dynarray_add(void *tab_ptr, int *nb_ptr, void *elem)
{
    void **tab = *(void ***)tab_ptr;

    BV_DYNARRAY_ADD(INT_MAX, sizeof(*tab), tab, *nb_ptr, {
        tab[*nb_ptr] = elem;
        *(void ***)tab_ptr = tab;
    }, {
        *nb_ptr = 0;
        bv_freep(tab_ptr);
    });
}

void *bv_dynarray2_add(void **tab_ptr, int *nb_ptr, size_t elem_size,
                       const uint8_t *elem_data)
{
    uint8_t *tab_elem_data = NULL;

    BV_DYNARRAY_ADD(INT_MAX, elem_size, *tab_ptr, *nb_ptr, {
        tab_elem_data = (uint8_t *)*tab_ptr + (*nb_ptr) * elem_size;
        if (elem_data)
            memcpy(tab_elem_data, elem_data, elem_size);
        else if (BV_CONFIG_MEMORY_POISONING)
            memset(tab_elem_data, BV_MEMORY_POISON, elem_size);
    }, {
        bv_freep(tab_ptr);
        *nb_ptr = 0;
    });
    return tab_elem_data;
}

static void fill16(uint8_t *dst, int len)
{
    uint32_t v = BV_RN16(dst - 2);

    v |= v << 16;

    while (len >= 4) {
        BV_WN32(dst, v);
        dst += 4;
        len -= 4;
    }

    while (len--) {
        *dst = dst[-2];
        dst++;
    }
}

static void fill24(uint8_t *dst, int len)
{
#if BV_HAVE_BIGENDIAN
    uint32_t v = BV_RB24(dst - 3);
    uint32_t a = v << 8  | v >> 16;
    uint32_t b = v << 16 | v >> 8;
    uint32_t c = v << 24 | v;
#else
    uint32_t v = BV_RL24(dst - 3);
    uint32_t a = v       | v << 24;
    uint32_t b = v >> 8  | v << 16;
    uint32_t c = v >> 16 | v << 8;
#endif

    while (len >= 12) {
        BV_WN32(dst,     a);
        BV_WN32(dst + 4, b);
        BV_WN32(dst + 8, c);
        dst += 12;
        len -= 12;
    }

    if (len >= 4) {
        BV_WN32(dst, a);
        dst += 4;
        len -= 4;
    }

    if (len >= 4) {
        BV_WN32(dst, b);
        dst += 4;
        len -= 4;
    }

    while (len--) {
        *dst = dst[-3];
        dst++;
    }
}

static void fill32(uint8_t *dst, int len)
{
    uint32_t v = BV_RN32(dst - 4);

    while (len >= 4) {
        BV_WN32(dst, v);
        dst += 4;
        len -= 4;
    }

    while (len--) {
        *dst = dst[-4];
        dst++;
    }
}

void bv_memcpy_backptr(uint8_t *dst, int back, int cnt)
{
    const uint8_t *src = &dst[-back];
    if (!back)
        return;

    if (back == 1) {
        memset(dst, *src, cnt);
    } else if (back == 2) {
        fill16(dst, cnt);
    } else if (back == 3) {
        fill24(dst, cnt);
    } else if (back == 4) {
        fill32(dst, cnt);
    } else {
        if (cnt >= 16) {
            int blocklen = back;
            while (cnt > blocklen) {
                memcpy(dst, src, blocklen);
                dst       += blocklen;
                cnt       -= blocklen;
                blocklen <<= 1;
            }
            memcpy(dst, src, cnt);
            return;
        }
        if (cnt >= 8) {
            BV_COPY32U(dst,     src);
            BV_COPY32U(dst + 4, src + 4);
            src += 8;
            dst += 8;
            cnt -= 8;
        }
        if (cnt >= 4) {
            BV_COPY32U(dst, src);
            src += 4;
            dst += 4;
            cnt -= 4;
        }
        if (cnt >= 2) {
            BV_COPY16U(dst, src);
            src += 2;
            dst += 2;
            cnt -= 2;
        }
        if (cnt)
            *dst = *src;
    }
}

void *bv_fast_realloc(void *ptr, unsigned int *size, size_t min_size)
{
    if (min_size < *size)
        return ptr;

    min_size = BBMAX(17 * min_size / 16 + 32, min_size);

    ptr = bv_realloc(ptr, min_size);
    /* we could set this to the unmodified min_size but this is safer
     * if the user lost the ptr and uses NULL now
     */
    if (!ptr)
        min_size = 0;

    *size = min_size;

    return ptr;
}

static inline int bb_fast_malloc(void *ptr, unsigned int *size, size_t min_size, int zero_realloc)
{
    void **p = ptr;
    if (min_size < *size)
        return 0;
    min_size = BBMAX(17 * min_size / 16 + 32, min_size);
    bv_free(*p);
    *p = zero_realloc ? bv_mallocz(min_size) : bv_malloc(min_size);
    if (!*p)
        min_size = 0;
    *size = min_size;
    return 1;
}

void bv_fast_malloc(void *ptr, unsigned int *size, size_t min_size)
{
    bb_fast_malloc(ptr, size, min_size, 0);
}

