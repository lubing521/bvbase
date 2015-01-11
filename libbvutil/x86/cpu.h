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

#ifndef BVUTIL_X86_CPU_H
#define BVUTIL_X86_CPU_H

#include "config.h"
#include "libbvutil/cpu.h"
#include "libbvutil/cpu_internal.h"

#define BV_CPU_FLAG_AMD3DNOW    BV_CPU_FLAG_3DNOW
#define BV_CPU_FLAG_AMD3DNOWEXT BV_CPU_FLAG_3DNOWEXT

#define X86_AMD3DNOW(flags)         CPUEXT(flags, AMD3DNOW)
#define X86_AMD3DNOWEXT(flags)      CPUEXT(flags, AMD3DNOWEXT)
#define X86_MMX(flags)              CPUEXT(flags, MMX)
#define X86_MMXEXT(flags)           CPUEXT(flags, MMXEXT)
#define X86_SSE(flags)              CPUEXT(flags, SSE)
#define X86_SSE2(flags)             CPUEXT(flags, SSE2)
#define X86_SSE3(flags)             CPUEXT(flags, SSE3)
#define X86_SSSE3(flags)            CPUEXT(flags, SSSE3)
#define X86_SSE4(flags)             CPUEXT(flags, SSE4)
#define X86_SSE42(flags)            CPUEXT(flags, SSE42)
#define X86_AVX(flags)              CPUEXT(flags, AVX)
#define X86_XOP(flags)              CPUEXT(flags, XOP)
#define X86_FMA3(flags)             CPUEXT(flags, FMA3)
#define X86_FMA4(flags)             CPUEXT(flags, FMA4)
#define X86_AVX2(flags)             CPUEXT(flags, AVX2)

#define EXTERNAL_AMD3DNOW(flags)    CPUEXT_SUFFIX(flags, _EXTERNAL, AMD3DNOW)
#define EXTERNAL_AMD3DNOWEXT(flags) CPUEXT_SUFFIX(flags, _EXTERNAL, AMD3DNOWEXT)
#define EXTERNAL_MMX(flags)         CPUEXT_SUFFIX(flags, _EXTERNAL, MMX)
#define EXTERNAL_MMXEXT(flags)      CPUEXT_SUFFIX(flags, _EXTERNAL, MMXEXT)
#define EXTERNAL_SSE(flags)         CPUEXT_SUFFIX(flags, _EXTERNAL, SSE)
#define EXTERNAL_SSE2(flags)        CPUEXT_SUFFIX(flags, _EXTERNAL, SSE2)
#define EXTERNAL_SSE3(flags)        CPUEXT_SUFFIX(flags, _EXTERNAL, SSE3)
#define EXTERNAL_SSSE3(flags)       CPUEXT_SUFFIX(flags, _EXTERNAL, SSSE3)
#define EXTERNAL_SSE4(flags)        CPUEXT_SUFFIX(flags, _EXTERNAL, SSE4)
#define EXTERNAL_SSE42(flags)       CPUEXT_SUFFIX(flags, _EXTERNAL, SSE42)
#define EXTERNAL_AVX(flags)         CPUEXT_SUFFIX(flags, _EXTERNAL, AVX)
#define EXTERNAL_XOP(flags)         CPUEXT_SUFFIX(flags, _EXTERNAL, XOP)
#define EXTERNAL_FMA3(flags)        CPUEXT_SUFFIX(flags, _EXTERNAL, FMA3)
#define EXTERNAL_FMA4(flags)        CPUEXT_SUFFIX(flags, _EXTERNAL, FMA4)
#define EXTERNAL_AVX2(flags)        CPUEXT_SUFFIX(flags, _EXTERNAL, AVX2)

#define INLINE_AMD3DNOW(flags)      CPUEXT_SUFFIX(flags, _INLINE, AMD3DNOW)
#define INLINE_AMD3DNOWEXT(flags)   CPUEXT_SUFFIX(flags, _INLINE, AMD3DNOWEXT)
#define INLINE_MMX(flags)           CPUEXT_SUFFIX(flags, _INLINE, MMX)
#define INLINE_MMXEXT(flags)        CPUEXT_SUFFIX(flags, _INLINE, MMXEXT)
#define INLINE_SSE(flags)           CPUEXT_SUFFIX(flags, _INLINE, SSE)
#define INLINE_SSE2(flags)          CPUEXT_SUFFIX(flags, _INLINE, SSE2)
#define INLINE_SSE3(flags)          CPUEXT_SUFFIX(flags, _INLINE, SSE3)
#define INLINE_SSSE3(flags)         CPUEXT_SUFFIX(flags, _INLINE, SSSE3)
#define INLINE_SSE4(flags)          CPUEXT_SUFFIX(flags, _INLINE, SSE4)
#define INLINE_SSE42(flags)         CPUEXT_SUFFIX(flags, _INLINE, SSE42)
#define INLINE_AVX(flags)           CPUEXT_SUFFIX(flags, _INLINE, AVX)
#define INLINE_XOP(flags)           CPUEXT_SUFFIX(flags, _INLINE, XOP)
#define INLINE_FMA3(flags)          CPUEXT_SUFFIX(flags, _INLINE, FMA3)
#define INLINE_FMA4(flags)          CPUEXT_SUFFIX(flags, _INLINE, FMA4)
#define INLINE_AVX2(flags)          CPUEXT_SUFFIX(flags, _INLINE, AVX2)

void bb_cpu_cpuid(int index, int *eax, int *ebx, int *ecx, int *edx);
void bb_cpu_xgetbv(int op, int *eax, int *edx);
int  bb_cpu_cpuid_test(void);

#endif /* BVUTIL_X86_CPU_H */
