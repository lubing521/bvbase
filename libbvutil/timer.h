/*
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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
 * high precision timer, useful to profile code
 */

#ifndef BVUTIL_TIMER_H
#define BVUTIL_TIMER_H

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "config.h"

#if BV_HAVE_MACH_MACH_TIME_H
#include <mach/mach_time.h>
#endif

#include "log.h"

#if   BV_ARCH_ARM
#   include "arm/timer.h"
#elif BV_ARCH_PPC
#   include "ppc/timer.h"
#elif BV_ARCH_X86
#   include "x86/timer.h"
#endif

#if !defined(BV_READ_TIME)
#   if BV_HAVE_GETHRTIME
#       define BV_READ_TIME gethrtime
#   elif BV_HAVE_MACH_ABSOLUTE_TIME
#       define BV_READ_TIME mach_absolute_time
#   endif
#endif

#ifndef BV_TIMER_UNITS
#   define BV_TIMER_UNITS "UNITS"
#endif

#ifdef BV_READ_TIME
#define START_TIMER                             \
    uint64_t tend;                              \
    uint64_t tstart = BV_READ_TIME();           \

#define STOP_TIMER(id)                                                    \
    tend = BV_READ_TIME();                                                \
    {                                                                     \
        static uint64_t tsum   = 0;                                       \
        static int tcount      = 0;                                       \
        static int tskip_count = 0;                                       \
        if (tcount < 2                        ||                          \
            tend - tstart < 8 * tsum / tcount ||                          \
            tend - tstart < 2000) {                                       \
            tsum+= tend - tstart;                                         \
            tcount++;                                                     \
        } else                                                            \
            tskip_count++;                                                \
        if (((tcount + tskip_count) & (tcount + tskip_count - 1)) == 0) { \
            bv_log(NULL, BV_LOG_ERROR,                                    \
                   "%"PRIu64" " BV_TIMER_UNITS " in %s, %d runs, %d skips\n",          \
                   tsum * 10 / tcount, id, tcount, tskip_count);          \
        }                                                                 \
    }
#else
#define START_TIMER
#define STOP_TIMER(id) { }
#endif

#endif /* BVUTIL_TIMER_H */
