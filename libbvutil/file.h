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

#ifndef BVUTIL_FILE_H
#define BVUTIL_FILE_H

#include <stdint.h>

#include "bvutil.h"

/**
 * @file
 * Misc file utilities.
 */

/**
 * Read the file with name filename, and put its content in a newly
 * allocated buffer or map it with mmap() when available.
 * In case of success set *bufptr to the read or mmapped buffer, and
 * *size to the size in bytes of the buffer in *bufptr.
 * The returned buffer must be released with bv_file_unmap().
 *
 * @param log_offset loglevel offset used for logging
 * @param log_ctx context used for logging
 * @return a non negative number in case of success, a negative value
 * corresponding to an BVERROR error code in case of failure
 */
int bv_file_map(const char *filename, uint8_t **bufptr, size_t *size,
                int log_offset, void *log_ctx);

/**
 * Unmap or free the buffer bufptr created by bv_file_map().
 *
 * @param size size in bytes of bufptr, must be the same as returned
 * by bv_file_map()
 */
void bv_file_unmap(uint8_t *bufptr, size_t size);

/**
 * Wrapper to work around the lack of mkstemp() on mingw.
 * Also, tries to create file in /tmp first, if possible.
 * *prefix can be a character constant; *filename will be allocated internally.
 * @return file descriptor of opened file (or negative value corresponding to an
 * BVERROR code on error)
 * and opened file name in **filename.
 * @note On very old libcs it is necessary to set a secure umask before
 *       calling this, bv_tempfile() can't call umask itself as it is used in
 *       libraries and could interfere with the calling application.
 */
int bv_tempfile(const char *prefix, char **filename, int log_offset, void *log_ctx);

#endif /* BVUTIL_FILE_H */
