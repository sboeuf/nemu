/*
 * cacheinfo.c - helpers to query the host about its caches
 *
 * Copyright (C) 2017, Emilio G. Cota <cota@braap.org>
 * License: GNU GPL, version 2 or later.
 *   See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"

int qemu_icache_linesize = 0;
int qemu_dcache_linesize = 0;

/*
 * Operating system specific detection mechanisms.
 */

#if   defined(__APPLE__)  || defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
# include <sys/sysctl.h>
#  define SYSCTL_CACHELINE_NAME "machdep.cacheline_size"

static void sys_cache_info(int *isize, int *dsize)
{
    /* There's only a single sysctl for both I/D cache line sizes.  */
    long size;
    size_t len = sizeof(size);
    if (!sysctlbyname(SYSCTL_CACHELINE_NAME, &size, &len, NULL, 0)) {
        *isize = *dsize = size;
    }
}

#else
/* POSIX */

static void sys_cache_info(int *isize, int *dsize)
{
# ifdef _SC_LEVEL1_ICACHE_LINESIZE
    *isize = sysconf(_SC_LEVEL1_ICACHE_LINESIZE);
# endif
# ifdef _SC_LEVEL1_DCACHE_LINESIZE
    *dsize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
# endif
}
#endif /* sys_cache_info */

/*
 * Architecture (+ OS) specific detection mechanisms.
 */

#if defined(__aarch64__)

static void arch_cache_info(int *isize, int *dsize)
{
    if (*isize == 0 || *dsize == 0) {
        unsigned long ctr;

        /* The real cache geometry is in CCSIDR_EL1/CLIDR_EL1/CSSELR_EL1,
           but (at least under Linux) these are marked protected by the
           kernel.  However, CTR_EL0 contains the minimum linesize in the
           entire hierarchy, and is used by userspace cache flushing.  */
        asm volatile("mrs\t%0, ctr_el0" : "=r"(ctr));
        if (*isize == 0) {
            *isize = 4 << (ctr & 0xf);
        }
        if (*dsize == 0) {
            *dsize = 4 << ((ctr >> 16) & 0xf);
        }
    }
}

#else
static void arch_cache_info(int *isize, int *dsize) { }
#endif /* arch_cache_info */

/*
 * ... and if all else fails ...
 */

static void fallback_cache_info(int *isize, int *dsize)
{
    /* If we can only find one of the two, assume they're the same.  */
    if (*isize) {
        if (*dsize) {
            /* Success! */
        } else {
            *dsize = *isize;
        }
    } else if (*dsize) {
        *isize = *dsize;
    } else {
        /* Otherwise, 64 bytes is not uncommon.  */
        *isize = *dsize = 64;
    }
}

static void __attribute__((constructor)) init_cache_info(void)
{
    int isize = 0, dsize = 0;

    sys_cache_info(&isize, &dsize);
    arch_cache_info(&isize, &dsize);
    fallback_cache_info(&isize, &dsize);

    qemu_icache_linesize = isize;
    qemu_dcache_linesize = dsize;
}
