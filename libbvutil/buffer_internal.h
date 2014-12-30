/*
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

#ifndef BVUTIL_BUFFER_INTERNAL_H
#define BVUTIL_BUFFER_INTERNAL_H

#include <stdint.h>

#include "buffer.h"
#include "thread.h"

/**
 * The buffer is always treated as read-only.
 */
#define BUFFER_FLAG_READONLY      (1 << 0)
/**
 * The buffer was bv_realloc()ed, so it is reallocatable.
 */
#define BUFFER_FLAG_REALLOCATABLE (1 << 1)

struct BVBuffer {
    uint8_t *data; /**< data described by this buffer */
    int      size; /**< size of data in bytes */

    /**
     *  number of existing BVBufferRef instances referring to this buffer
     */
    volatile int refcount;

    /**
     * a callback for freeing the data
     */
    void (*free)(void *opaque, uint8_t *data);

    /**
     * an opaque pointer, to be used by the freeing callback
     */
    void *opaque;

    /**
     * A combination of BUFFER_FLAG_*
     */
    int flags;
};

typedef struct BufferPoolEntry {
    uint8_t *data;

    /*
     * Backups of the original opaque/free of the BVBuffer corresponding to
     * data. They will be used to free the buffer when the pool is freed.
     */
    void *opaque;
    void (*free)(void *opaque, uint8_t *data);

    BVBufferPool *pool;
    struct BufferPoolEntry *next;
} BufferPoolEntry;

struct BVBufferPool {
    AVMutex mutex;
    BufferPoolEntry *pool;

    /*
     * This is used to track when the pool is to be freed.
     * The pointer to the pool itself held by the caller is considered to
     * be one reference. Each buffer requested by the caller increases refcount
     * by one, returning the buffer to the pool decreases it by one.
     * refcount reaches zero when the buffer has been uninited AND all the
     * buffers have been released, then it's safe to free the pool and all
     * the buffers in it.
     */
    volatile int refcount;

    volatile int nb_allocated;

    int size;
    BVBufferRef* (*alloc)(int size);
};

#endif /* BVUTIL_BUFFER_INTERNAL_H */
