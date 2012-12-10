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

#include <rose.h>
#include <rose/memory.h>
#include <rose/multiboot.h>
#include <rose/stdint.h>
#include <rose/string.h>

#define OP_SIZE_16BIT         0
#define OP_SIZE_32BIT         1
#define DPL_KERNEL            0
#define DPL_USER              3
#define GRANULARITY_BYTE      0
#define GRANULARITY_KBYTE     1
#define DESC_TYPE_SYSTEM      0
#define DESC_TYPE_CODEDATA    1

#define DESC_TYPE_DATA        0
#define DESC_TYPE_CODE        (1 << 3)
#define DESC_TYPE_EXPAND_UP   0
#define DESC_TYPE_EXPAND_DOWN (1 << 2)
#define DESC_TYPE_RO          0
#define DESC_TYPE_RW          (1 << 1)
#define DESC_TYPE_XO          0
#define DESC_TYPE_RX          (1 << 1)

#define PAGE_SIZE_4KB 0
#define PAGE_SIZE_4MB 1

struct gdt_entry {
    uint16_t limit_lower;
    uint16_t address_lower;
    uint8_t  address_middle;
    uint8_t  type                   : 4;
    uint8_t  descriptor_type        : 1;
    uint8_t  dpl                    : 2;
    uint8_t  segment_present        : 1;
    uint8_t  limit_high             : 4;
    uint8_t  avl                    : 1;
    uint8_t  is_64                  : 1;
    uint8_t  default_operation_size : 1;
    uint8_t  granularity            : 1;
    uint8_t  address_high;
} __attribute__((packed));

struct gdt_pointer {
    uint16_t limit;
    struct gdt_entry *first;
} __attribute__((packed));

struct page_table_entry {
    uint8_t  present                  : 1;
    uint8_t  is_rw                    : 1;
    uint8_t  is_user_permitted        : 1;
    uint8_t  page_level_write_through : 1;
    uint8_t  page_level_cache_disable : 1;
    uint8_t  accessed                 : 1;
    uint8_t  dirty                    : 1;
    uint8_t  pat                      : 1;
    uint8_t  is_global                : 1;
    uint8_t  reserved0                : 3;
    uint32_t page                     : 20; /* we need to right-pad this by 12 0 bits */

} __attribute__((packed));

struct page_table {
    struct page_table_entry entries[MEMORY_PAGE_SIZE / sizeof(struct page_table_entry)];
} __attribute__((packed));

struct page_directory_entry {
    uint8_t  present                  : 1;
    uint8_t  is_rw                    : 1;
    uint8_t  is_user_permitted        : 1;
    uint8_t  page_level_write_through : 1;
    uint8_t  page_level_cache_disable : 1;
    uint8_t  accessed                 : 1;
    uint8_t  reserved0                : 1;
    uint8_t  page_size                : 1;
    uint8_t  reserved1                : 4;
    uint32_t page_table               : 20; /* we need to right-pad this by 12 0 bits */
} __attribute__((packed));

struct page_directory {
    struct page_directory_entry entries[MEMORY_PAGE_SIZE / sizeof(struct page_directory_entry)];
} __attribute__((packed));

struct gdt_entry gdt[5] __attribute__((aligned (8)));
struct gdt_pointer gdt_ptr;

struct page_directory kernel_pages __attribute__((aligned (MEMORY_PAGE_SIZE)));

#ifdef ROSE_TESTING
extern void _gdt_set(struct gdt_pointer *gdt);
#else
/* NOTE: If this is ever used outside of the context of
 *       setting up for protected mode, we need to refresh
 *       the segment selectors.
 */
static void
_gdt_set(struct gdt_pointer *gdt)
{
    asm("MOV EAX, %0;"
        "LGDT [EAX];"
       :
       : "m"(gdt)
       : "eax"
       );
}
#endif

static void
_gdt_entry_set_base(struct gdt_entry *entry, uint32_t base)
{
    entry->address_lower  = base & 0xFFFF;
    entry->address_middle = (base >> 16) & 0xFF;
    entry->address_high   = (base >> 24) & 0xFF;
}

static void
_gdt_entry_set_limit(struct gdt_entry *entry, uint32_t limit)
{
    entry->limit_lower = limit & 0xFFFF;
    entry->limit_high  = (limit >> 16) & 0x0F;
}

#ifdef ROSE_TESTING
extern void _cr3_set(struct page_directory *dir);
#else
static void
_cr3_set(struct page_directory *dir)
{
    asm(
        "MOV EAX, %0;"
        "MOV CR3, EAX;"
        "MOV EAX, CR0;"
        "OR EAX, 1 << 31;" /* XXX can we get rid of this magic number? */
        "MOV CR0, EAX;"
       :
       : "m"(dir)
       : "eax"
       );
}
#endif

/* The GDT should contain five segment descriptors:
 *
 *  0: Null Segment
 *  1: Kernelspace Code Segment
 *  2: Kernelspace Data Segment
 *  3: Userspace Code Segment
 *  4: Userspace Data Segment
 *
 */

void
memory_init_gdt(void)
{
    int i;

    _gdt_entry_set_base(gdt, 0);
    _gdt_entry_set_limit(gdt, 0);

    gdt[0].segment_present        = 0;
    gdt[0].is_64                  = 0;
    gdt[0].default_operation_size = 0;
    gdt[0].granularity            = 0;
    gdt[0].avl                    = 0;
    gdt[0].type                   = 0;
    gdt[0].descriptor_type        = 0;
    gdt[0].dpl                    = 0;

    for(i = 1; i < 5; i++) {
        _gdt_entry_set_base(gdt + i, 0);
        _gdt_entry_set_limit(gdt + i, 0xFFFFFFFF);

        gdt[i].segment_present        = 1;
        gdt[i].is_64                  = 0;
        gdt[i].default_operation_size = OP_SIZE_32BIT;
        gdt[i].granularity            = GRANULARITY_KBYTE;
        gdt[i].avl                    = 0;
        gdt[i].descriptor_type        = DESC_TYPE_CODEDATA;
    }

    gdt[1].type = gdt[3].type = DESC_TYPE_CODE | DESC_TYPE_RX;
    gdt[2].type = gdt[4].type = DESC_TYPE_DATA | DESC_TYPE_RW;

    gdt[1].dpl = gdt[2].dpl = DPL_KERNEL;
    gdt[3].dpl = gdt[4].dpl = DPL_USER;

    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.first = gdt;

    _gdt_set(&gdt_ptr);
}

struct free_pages {
    int num_pages;
    struct free_pages *next;
};

struct free_pages *free_list = NULL;

static void
identity_map(void *addr)
{
    uint32_t page = (uint32_t) addr;
    struct page_table *table;
    uint16_t directory_entry_index;
    uint16_t table_entry_index;
    int has_allocated_page = 0;

    directory_entry_index = page >> 22;
    table_entry_index     = (page >> 12) & 0x03ff;

    if(! kernel_pages.entries[directory_entry_index].present) {
        kernel_pages.entries[directory_entry_index].present    = 1;
        kernel_pages.entries[directory_entry_index].is_rw      = 1;
        kernel_pages.entries[directory_entry_index].page_table = ((uint32_t) memory_allocate_page()) >> 12; /* XXX encapsulate */
        has_allocated_page = 1;
    }
    table = (struct page_table *) (kernel_pages.entries[directory_entry_index].page_table << 12);

    table->entries[table_entry_index].present = 1;
    table->entries[table_entry_index].is_rw   = 1;
    table->entries[table_entry_index].page    = page >> 12;

    if(has_allocated_page) {
        identity_map(table);
    }
}

void
memory_init_paging(void *kernel_start, void *kernel_end)
{
    void *alloc_end;
    void *page;

    alloc_end = MEMORY_IS_PAGE_ALIGNED(kernel_end) ? kernel_end : MEMORY_PAGE_ALIGN(kernel_end) + MEMORY_PAGE_SIZE;

    memset(&kernel_pages, 0, sizeof(kernel_pages));
    for(page = MEMORY_PAGE_ALIGN(kernel_start); page < alloc_end; page += MEMORY_PAGE_SIZE) {
        identity_map(page);
    }

    _cr3_set(&kernel_pages);
}

/* Optimization opportunities:
 *
 *  - We can reuse addresses starting at 0x0
 *  - We can use non-aligned addresses to store stuff (as long as the page
 *    that contains it is owned by the kernel)
 */
void
memory_detect(void *kernel_end, struct multiboot_info *mboot)
{
    struct multiboot_memory *chunk = mboot->mmap_addr;
    struct multiboot_memory *end   = MBOOT_MMAP_END(mboot);
    struct free_pages **previous   = &free_list;

    if(! (mboot->flags & MBOOT_MEMORY_MAP)) {
        /* XXX uh-oh! */
        return;
    }

    for(; chunk < end; chunk = MBOOT_MMAP_NEXT(chunk)) {
        struct free_pages *pages;
        char *start_address;
        char *end_address;

        if(chunk->type != 1) { /* free (XXX magic number!) */
            continue;
        }
        if(chunk->base_addr == 0) {
            continue;
        }

        start_address = (char *) (uint32_t) chunk->base_addr;
        end_address   = (char *) (uint32_t) (chunk->base_addr + chunk->length);

        if(((void *) start_address) < kernel_end) {
            start_address = kernel_end;
        }

        end_address = MEMORY_PAGE_ALIGN(end_address);
        if(! MEMORY_IS_PAGE_ALIGNED(start_address)) {
            start_address  = MEMORY_PAGE_ALIGN(start_address);
            start_address += MEMORY_PAGE_SIZE;
        }

        pages = (struct free_pages *) start_address;

        *previous        = pages;
        pages->num_pages = (end_address - start_address) / MEMORY_PAGE_SIZE;
        pages->next      = NULL;
        previous         = &(pages->next);
    }
}

void *
memory_allocate_page(void)
{
    struct free_pages *pages = free_list;
    struct free_pages *next;

    if(! pages) {
        /* Freak out! (at least until swapping is implemented) */
        panic("Out of memory");
    }

    if(pages->num_pages == 1) {
        next = pages->next;
    } else {
        next = (struct free_pages *) (((char *) pages) + MEMORY_PAGE_SIZE);
        memcpy(next, pages, sizeof(struct free_pages));
        next->num_pages--;
    }

    free_list = next;

    return (void *) pages;
}

static int
free_page_blocks_are_adjacent(struct free_pages *first, struct free_pages *second)
{
    if(first > second) {
        struct free_pages *tmp = first;
        first  = second;
        second = tmp;
    }

    return ((char *) first) + first->num_pages * MEMORY_PAGE_SIZE == ((char *) second);
}

static void
free_page_blocks_merge(struct free_pages *first, struct free_pages *second)
{
    ROSE_ASSERT(first < second);
    ROSE_ASSERT(free_page_blocks_are_adjacent(first, second));
    ROSE_ASSERT(first->next == second);

    first->num_pages += second->num_pages;
    first->next       = second->next;
}

void
memory_free_page(void *page)
{
    /* XXX Is NULL an ok value for this? */
    struct free_pages *previous_node = NULL;
    struct free_pages *next_node     = free_list;
    struct free_pages *new_node      = (void *) page;

    if(! MEMORY_IS_PAGE_ALIGNED(page)) {
        panic("Address %p is not page-aligned!", page);
    }

    while(next_node && next_node < new_node) {
        previous_node = next_node;
        next_node     = next_node->next;
    }

    new_node->num_pages = 1;
    new_node->next      = next_node;

    if(previous_node) {
        previous_node->next = new_node;
    } else {
        free_list = new_node;
    }

    if(next_node && free_page_blocks_are_adjacent(new_node, next_node)) {
        free_page_blocks_merge(new_node, next_node);
    }
    if(previous_node && free_page_blocks_are_adjacent(previous_node, new_node)) {
        free_page_blocks_merge(previous_node, new_node);
    }
}

int
memory_test_paged_address(void *addr)
{
    uint16_t directory_entry_index;
    uint16_t table_entry_index;
    struct page_table *pt;

    directory_entry_index = ((uint32_t) addr) >> 22;
    table_entry_index     = (((uint32_t) addr) >> 12) & 0x03ff;

    if(! kernel_pages.entries[directory_entry_index].present) {
        return 0;
    }
    pt = (struct page_table *) (kernel_pages.entries[directory_entry_index].page_table << 12);

    return pt->entries[table_entry_index].present;
}
