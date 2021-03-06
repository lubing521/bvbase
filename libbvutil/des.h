/*
 * DES encryption/decryption
 * Copyright (c) 2007 Reimar Doeffinger
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

#ifndef BVUTIL_DES_H
#define BVUTIL_DES_H

#include <stdint.h>

struct BVDES {
    uint64_t round_keys[3][16];
    int triple_des;
};

/**
 * @brief Initializes an BVDES context.
 *
 * @param key_bits must be 64 or 192
 * @param decrypt 0 for encryption/CBC-MAC, 1 for decryption
 */
int bv_des_init(struct BVDES *d, const uint8_t *key, int key_bits, int decrypt);

/**
 * @brief Encrypts / decrypts using the DES algorithm.
 *
 * @param count number of 8 byte blocks
 * @param dst destination array, can be equal to src, must be 8-byte aligned
 * @param src source array, can be equal to dst, must be 8-byte aligned, may be NULL
 * @param iv initialization vector for CBC mode, if NULL then ECB will be used,
 *           must be 8-byte aligned
 * @param decrypt 0 for encryption, 1 for decryption
 */
void bv_des_crypt(struct BVDES *d, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int decrypt);

/**
 * @brief Calculates CBC-MAC using the DES algorithm.
 *
 * @param count number of 8 byte blocks
 * @param dst destination array, can be equal to src, must be 8-byte aligned
 * @param src source array, can be equal to dst, must be 8-byte aligned, may be NULL
 */
void bv_des_mac(struct BVDES *d, uint8_t *dst, const uint8_t *src, int count);

#endif /* BVUTIL_DES_H */
