/*
 * Copyright (c) 2012 Rob Hoelz <rob at hoelz.ro>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef ROSE_MULTIBOOT_H
#define ROSE_MULTIBOOT_H

#include <rose/stdint.h>

enum mboot_flags {
    MBOOT_MEMORY_BOUNDS  = 1 << 0,
    MBOOT_BOOT_DEVICE    = 1 << 1,
    MBOOT_CMDLINE        = 1 << 2,
    MBOOT_MODULES        = 1 << 3,
    MBOOT_AOUT_SYMBOLS   = 1 << 4,
    MBOOT_ELF_SYMBOLS    = 1 << 5,
    MBOOT_MEMORY_MAP     = 1 << 6,
    MBOOT_DRIVES         = 1 << 7,
    MBOOT_CONFIG_TABLE   = 1 << 8,
    MBOOT_LOADER_NAME    = 1 << 9,
    MBOOT_APM_TABLE      = 1 << 10,
    MBOOT_GRAPHICS_TABLE = 1 << 11,
};

struct multiboot_memory {
    uint32_t size;
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
};

struct multiboot_module {
    void *mod_start;
    void *mod_end;
    char *string;
    uint32_t reserved;
};

struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    void    *cmdline;
    uint32_t mods_count;
    struct multiboot_module *mods_addr;
    uint32_t syms1; /* XXX fill this in with a real struct later */
    uint32_t syms2;
    uint32_t syms3;
    uint32_t syms4;
    uint32_t mmap_length;

    struct multiboot_memory *mmap_addr;

    /* XXX fill in other fields in the multiboot struct */
};

#define MBOOT_MMAP_END(mboot)\
    ((struct multiboot_memory *) (((char *) mboot->mmap_addr) + mboot->mmap_length))

#define MBOOT_MMAP_NEXT(mem)\
    ((struct multiboot_memory *) (((char *) mem) + mem->size + 4))

#endif
