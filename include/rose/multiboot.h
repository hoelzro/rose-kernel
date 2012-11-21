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

struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    void    *cmdline;
    uint32_t mods_count;
    void    *mods_addr; /* XXX fill in later */
    uint32_t syms1; /* XXX fill this in with a real struct later */
    uint32_t syms2;
    uint32_t syms3;
    uint32_t syms4;
    uint32_t mmap_length;

    struct multiboot_memory *mmap_addr;
};

#endif
