/*
 * Copyright (c) 2007 Mans Rullgard
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

#ifndef BVUTIL_AVSTRING_H
#define BVUTIL_AVSTRING_H

#include <stddef.h>
#include <stdint.h>
#include "attributes.h"

/**
 * @addtogroup lavu_string
 * @{
 */

/**
 * Return non-zero if pfx is a prefix of str. If it is, *ptr is set to
 * the address of the first character in str after the prefix.
 *
 * @param str input string
 * @param pfx prefix to test
 * @param ptr updated if the prefix is matched inside str
 * @return non-zero if the prefix matches, zero otherwise
 */
int bv_strstart(const char *str, const char *pfx, const char **ptr);

/**
 * Return non-zero if pfx is a prefix of str independent of case. If
 * it is, *ptr is set to the address of the first character in str
 * after the prefix.
 *
 * @param str input string
 * @param pfx prefix to test
 * @param ptr updated if the prefix is matched inside str
 * @return non-zero if the prefix matches, zero otherwise
 */
int bv_stristart(const char *str, const char *pfx, const char **ptr);

/**
 * Locate the first case-independent occurrence in the string haystack
 * of the string needle.  A zero-length string needle is considered to
 * match at the start of haystack.
 *
 * This function is a case-insensitive version of the standard strstr().
 *
 * @param haystack string to search in
 * @param needle   string to search for
 * @return         pointer to the located match within haystack
 *                 or a null pointer if no match
 */
char *bv_stristr(const char *haystack, const char *needle);

/**
 * Locate the first occurrence of the string needle in the string haystack
 * where not more than hay_length characters are searched. A zero-length
 * string needle is considered to match at the start of haystack.
 *
 * This function is a length-limited version of the standard strstr().
 *
 * @param haystack   string to search in
 * @param needle     string to search for
 * @param hay_length length of string to search in
 * @return           pointer to the located match within haystack
 *                   or a null pointer if no match
 */
char *bv_strnstr(const char *haystack, const char *needle, size_t hay_length);

/**
 * Copy the string src to dst, but no more than size - 1 bytes, and
 * null-terminate dst.
 *
 * This function is the same as BSD strlcpy().
 *
 * @param dst destination buffer
 * @param src source string
 * @param size size of destination buffer
 * @return the length of src
 *
 * @warning since the return value is the length of src, src absolutely
 * _must_ be a properly 0-terminated string, otherwise this will read beyond
 * the end of the buffer and possibly crash.
 */
size_t bv_strlcpy(char *dst, const char *src, size_t size);

/**
 * Append the string src to the string dst, but to a total length of
 * no more than size - 1 bytes, and null-terminate dst.
 *
 * This function is similar to BSD strlcat(), but differs when
 * size <= strlen(dst).
 *
 * @param dst destination buffer
 * @param src source string
 * @param size size of destination buffer
 * @return the total length of src and dst
 *
 * @warning since the return value use the length of src and dst, these
 * absolutely _must_ be a properly 0-terminated strings, otherwise this
 * will read beyond the end of the buffer and possibly crash.
 */
size_t bv_strlcat(char *dst, const char *src, size_t size);

/**
 * Append output to a string, according to a format. Never write out of
 * the destination buffer, and always put a terminating 0 within
 * the buffer.
 * @param dst destination buffer (string to which the output is
 *  appended)
 * @param size total size of the destination buffer
 * @param fmt printf-compatible format string, specifying how the
 *  following parameters are used
 * @return the length of the string that would have been generated
 *  if enough space had been available
 */
size_t bv_strlcatf(char *dst, size_t size, const char *fmt, ...) bv_printf_format(3, 4);

/**
 * Get the count of continuous non zero chars starting from the beginning.
 *
 * @param len maximum number of characters to check in the string, that
 *            is the maximum value which is returned by the function
 */
static inline size_t bv_strnlen(const char *s, size_t len)
{
    size_t i;
    for (i = 0; i < len && s[i]; i++)
        ;
    return i;
}

/**
 * Print arguments following specified format into a large enough auto
 * allocated buffer. It is similar to GNU asprintf().
 * @param fmt printf-compatible format string, specifying how the
 *            following parameters are used.
 * @return the allocated string
 * @note You have to free the string yourself with bv_free().
 */
char *bv_asprintf(const char *fmt, ...) bv_printf_format(1, 2);

/**
 * Convert a number to a bv_malloced string.
 */
char *bv_d2str(double d);

/**
 * Unescape the given string until a non escaped terminating char,
 * and return the token corresponding to the unescaped string.
 *
 * The normal \ and ' escaping is supported. Leading and trailing
 * whitespaces are removed, unless they are escaped with '\' or are
 * enclosed between ''.
 *
 * @param buf the buffer to parse, buf will be updated to point to the
 * terminating char
 * @param term a 0-terminated list of terminating chars
 * @return the malloced unescaped string, which must be bv_freed by
 * the user, NULL in case of allocation failure
 */
char *bv_get_token(const char **buf, const char *term);

/**
 * Split the string into several tokens which can be accessed by
 * successive calls to bv_strtok().
 *
 * A token is defined as a sequence of characters not belonging to the
 * set specified in delim.
 *
 * On the first call to bv_strtok(), s should point to the string to
 * parse, and the value of saveptr is ignored. In subsequent calls, s
 * should be NULL, and saveptr should be unchanged since the previous
 * call.
 *
 * This function is similar to strtok_r() defined in POSIX.1.
 *
 * @param s the string to parse, may be NULL
 * @param delim 0-terminated list of token delimiters, must be non-NULL
 * @param saveptr user-provided pointer which points to stored
 * information necessary for bv_strtok() to continue scanning the same
 * string. saveptr is updated to point to the next character after the
 * first delimiter found, or to NULL if the string was terminated
 * @return the found token, or NULL when no token is found
 */
char *bv_strtok(char *s, const char *delim, char **saveptr);

/**
 * Locale-independent conversion of ASCII isdigit.
 */
bv_const int bv_isdigit(int c);

/**
 * Locale-independent conversion of ASCII isgraph.
 */
bv_const int bv_isgraph(int c);

/**
 * Locale-independent conversion of ASCII isspace.
 */
bv_const int bv_isspace(int c);

/**
 * Locale-independent conversion of ASCII characters to uppercase.
 */
static inline bv_const int bv_toupper(int c)
{
    if (c >= 'a' && c <= 'z')
        c ^= 0x20;
    return c;
}

/**
 * Locale-independent conversion of ASCII characters to lowercase.
 */
static inline bv_const int bv_tolower(int c)
{
    if (c >= 'A' && c <= 'Z')
        c ^= 0x20;
    return c;
}

/**
 * Locale-independent conversion of ASCII isxdigit.
 */
bv_const int bv_isxdigit(int c);

/**
 * Locale-independent case-insensitive compare.
 * @note This means only ASCII-range characters are case-insensitive
 */
int bv_strcasecmp(const char *a, const char *b);

/**
 * Locale-independent case-insensitive compare.
 * @note This means only ASCII-range characters are case-insensitive
 */
int bv_strncasecmp(const char *a, const char *b, size_t n);


/**
 * Thread safe basename.
 * @param path the path, on DOS both \ and / are considered separators.
 * @return pointer to the basename substring.
 */
const char *bv_basename(const char *path);

/**
 * Thread safe dirname.
 * @param path the path, on DOS both \ and / are considered separators.
 * @return the path with the separator replaced by the string terminator or ".".
 * @note the function may change the input string.
 */
const char *bv_dirname(char *path);

/**
 * Match instances of a name in a comma-separated list of names.
 * @param name  Name to look for.
 * @param names List of names.
 * @return 1 on match, 0 otherwise.
 */
int bv_match_name(const char *name, const char *names);

enum BVEscapeMode {
    BV_ESCAPE_MODE_AUTO,      ///< Use auto-selected escaping mode.
    BV_ESCAPE_MODE_BACKSLASH, ///< Use backslash escaping.
    BV_ESCAPE_MODE_QUOTE,     ///< Use single-quote escaping.
};

/**
 * Consider spaces special and escape them even in the middle of the
 * string.
 *
 * This is equivalent to adding the whitespace characters to the special
 * characters lists, except it is guaranteed to use the exact same list
 * of whitespace characters as the rest of libbvutil.
 */
#define BV_ESCAPE_FLAG_WHITESPACE 0x01

/**
 * Escape only specified special characters.
 * Without this flag, escape also any characters that may be considered
 * special by bv_get_token(), such as the single quote.
 */
#define BV_ESCAPE_FLAG_STRICT 0x02

/**
 * Escape string in src, and put the escaped string in an allocated
 * string in *dst, which must be freed with bv_free().
 *
 * @param dst           pointer where an allocated string is put
 * @param src           string to escape, must be non-NULL
 * @param special_chars string containing the special characters which
 *                      need to be escaped, can be NULL
 * @param mode          escape mode to employ, see BV_ESCAPE_MODE_* macros.
 *                      Any unknown value for mode will be considered equivalent to
 *                      BV_ESCAPE_MODE_BACKSLASH, but this behaviour can change without
 *                      notice.
 * @param flags         flags which control how to escape, see BV_ESCAPE_FLAG_ macros
 * @return the length of the allocated string, or a negative error code in case of error
 * @see bv_bprint_escape()
 */
int bv_escape(char **dst, const char *src, const char *special_chars,
              enum BVEscapeMode mode, int flags);

#define BV_UTF8_FLAG_ACCEPT_INVALID_BIG_CODES          1 ///< accept codepoints over 0x10FFFF
#define BV_UTF8_FLAG_ACCEPT_NON_CHARACTERS             2 ///< accept non-characters - 0xFFFE and 0xFFFF
#define BV_UTF8_FLAG_ACCEPT_SURROGATES                 4 ///< accept UTF-16 surrogates codes
#define BV_UTF8_FLAG_EXCLUDE_XML_INVALID_CONTROL_CODES 8 ///< exclude control codes not accepted by XML

#define BV_UTF8_FLAG_ACCEPT_ALL \
    BV_UTF8_FLAG_ACCEPT_INVALID_BIG_CODES|BV_UTF8_FLAG_ACCEPT_NON_CHARACTERS|BV_UTF8_FLAG_ACCEPT_SURROGATES

/**
 * Read and decode a single UTF-8 code point (character) from the
 * buffer in *buf, and update *buf to point to the next byte to
 * decode.
 *
 * In case of an invalid byte sequence, the pointer will be updated to
 * the next byte after the invalid sequence and the function will
 * return an error code.
 *
 * Depending on the specified flags, the function will also fail in
 * case the decoded code point does not belong to a valid range.
 *
 * @note For speed-relevant code a carefully implemented use of
 * GET_UTF8() may be preferred.
 *
 * @param codep   pointer used to return the parsed code in case of success.
 *                The value in *codep is set even in case the range check fails.
 * @param bufp    pointer to the address the first byte of the sequence
 *                to decode, updated by the function to point to the
 *                byte next after the decoded sequence
 * @param buf_end pointer to the end of the buffer, points to the next
 *                byte past the last in the buffer. This is used to
 *                avoid buffer overreads (in case of an unfinished
 *                UTF-8 sequence towards the end of the buffer).
 * @param flags   a collection of BV_UTF8_FLAG_* flags
 * @return >= 0 in case a sequence was successfully read, a negative
 * value in case of invalid sequence
 */
int bv_utf8_decode(int32_t *codep, const uint8_t **bufp, const uint8_t *buf_end,
                   unsigned int flags);

/**
 * Check if a name is in a list.
 * @returns 0 if not found, or the 1 based index where it has been found in the
 *            list.
 */
int bv_match_list(const char *name, const char *list, char separator);

/**
 * @}
 */

#endif /* BVUTIL_AVSTRING_H */
