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

#define NUM_PAGE_DIRECTORY_ENTRIES (MEMORY_PAGE_SIZE / sizeof(struct page_directory_entry))
#define NUM_PAGE_TABLE_ENTRIES     (MEMORY_PAGE_SIZE / sizeof(struct page_table_entry))

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
    struct page_table_entry entries[NUM_PAGE_TABLE_ENTRIES];
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
    struct page_directory_entry entries[NUM_PAGE_DIRECTORY_ENTRIES];
} __attribute__((packed));

struct gdt_entry gdt[5] __attribute__((aligned (8)));
struct gdt_pointer gdt_ptr;

struct page_directory kernel_pages __attribute__((aligned (MEMORY_PAGE_SIZE)));
static struct page_table *dummy_page_table;

/* These are two logical addresses that we keep free for pages that we don't
 * have a permanent mapping for */
static void *free_logical_address1 = 0;
static void *free_logical_address2 = 0;

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
    struct free_pages *next; /* PHYSICAL address */
};

/* The PHYSICAL address of the beginning of the free list */
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
        /* we do this because memory_allocate_page() can return a page
         * that would end up in the same page directory as the page
         * we are mapping.  If that's the case, then we need a temporary
         * dummy page to store the entry so we can work on it while allocating
         * the real page table.  Afterwards, the allocated page is identity
         * mapped correctly. */
        kernel_pages.entries[directory_entry_index].page_table = ((uint32_t) dummy_page_table) >> 12;

        table = memory_allocate_page();
        memset(table, 0, sizeof(struct page_table));

        kernel_pages.entries[directory_entry_index].page_table = ((uint32_t) table) >> 12; /* XXX encapsulate */
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

static void
remove_identity_map(void *addr)
{
    uint16_t directory_entry_index;
    uint16_t table_entry_index;
    struct page_table *pt;

    /* XXX we should really abstract this away */
    directory_entry_index = ((uint32_t) addr) >> 22;
    table_entry_index     = (((uint32_t) addr) >> 12) & 0x03ff;

    pt = (struct page_table *) (kernel_pages.entries[directory_entry_index].page_table << 12);

    pt->entries[table_entry_index].present = 0;

    /* XXX if a page table has no present entries, we should probably return
     *     it to the memory pool */
}

static void *
_physical_to_logical(void *addr)
{
    /* XXX naïve assumption */
    return ((char *) addr) + 0xC0000000;
}

static struct page_table *
_get_page_table(struct page_directory *pd, void *addr)
{
    uint32_t upper = (uint32_t) addr;
    upper >>= 22;
    return (struct page_table *) (pd->entries[upper].page_table << 12);
}

static void
_set_page(struct page_table *table, void *logical, void *physical)
{
    uint32_t middle   = (uint32_t) logical;
    middle          >>= 12;
    middle           &= 0x3FF;

    /* XXX probably should allow more control over is_rw and such */
    struct page_table_entry *entry = table->entries + middle;
    entry->present = 1;
    entry->is_rw   = 1;
    entry->page    = ((uint32_t) physical) >> 12;
}

static void *
_find_free_logical_address(void)
{
    struct page_directory *cr3;
    struct page_table *kernel_land;
    unsigned int i;

    /* retrieve the physical address of the
     * page directory */
    asm("MOV %0, CR3;"
       :"=r"(cr3)
       );

    /* get its logical address */
    cr3 = _physical_to_logical(cr3);

    /* XXX magic number */
    ROSE_ASSERT(cr3->entries[768].present);

    /* get the page table corresponding to the kernel */
    kernel_land = _get_page_table(cr3, (void *) 0xC0000000);
    kernel_land = _physical_to_logical(kernel_land);

    /* iterate over the entries in the kernel's page table to
     * find a non-present (ie. free) one */
    for(i = 0; i < NUM_PAGE_TABLE_ENTRIES; i++) {
        if(! kernel_land->entries[i].present) {
            void *addr = (void *) (0xC0000000 | (i << 12));
            if(addr != free_logical_address1 && addr != free_logical_address2) {
                return addr;
            }
        }
    }
    ROSE_ASSERT(0);

    return NULL;
}

static void *
_map_physical_address(void *physical, void *logical)
{
    struct page_directory *cr3;
    struct page_table *table;

    /* retrieve the physical address of the
     * page directory */
    asm("MOV %0, CR3;"
       :"=r"(cr3)
       );

    cr3 = _physical_to_logical(cr3);
    /* XXX we are just assuming that the page table is present!
     *     we are also potentially clobbering the page table entry
     *     already present!
     */
    table = _physical_to_logical(_get_page_table(cr3, logical));
    _set_page(table, logical, physical);

    return logical;
}

static void
_flush_tlb(void *addr)
{
    asm("INVLPG [%0]"
       :
       :"r"(addr)
       );
}

void
memory_init_paging(void *kernel_start, void *kernel_end)
{
    void *alloc_end;
    void *page;
    struct free_pages *pages;

    /* we need to do this by hand since our normal allocation routines
     * do some funky stuff with pages */
    dummy_page_table = (struct page_table *) free_list;
    if(free_list->num_pages == 1) {
        free_list = free_list->next;
    } else {
        struct free_pages *new_free_list = (struct free_pages *) (((char *) free_list) + MEMORY_PAGE_SIZE);

        memcpy(new_free_list, free_list, sizeof(struct free_pages));
        free_list = new_free_list;
        free_list->num_pages--;
    }

    alloc_end = MEMORY_IS_PAGE_ALIGNED(kernel_end) ? kernel_end : MEMORY_PAGE_ALIGN(kernel_end) + MEMORY_PAGE_SIZE;

    memset(&kernel_pages, 0, sizeof(kernel_pages));
    for(page = MEMORY_PAGE_ALIGN(kernel_start); page < alloc_end; page += MEMORY_PAGE_SIZE) {
        identity_map(page);
    }
    identity_map(dummy_page_table);

    pages = free_list;
    while(pages) {
        identity_map(pages); /* XXX this can alter the free list */
        pages = pages->next;
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
    struct free_pages *logical_pages;

    free_logical_address1 = _find_free_logical_address();
    free_logical_address2 = _find_free_logical_address();

    logical_pages = free_logical_address1;

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
        _map_physical_address(pages, logical_pages);
        _flush_tlb(logical_pages);

        *previous                = pages;
        logical_pages->num_pages = (end_address - start_address) / MEMORY_PAGE_SIZE;
        logical_pages->next      = NULL;
        previous                 = &(logical_pages->next);

        if(logical_pages == free_logical_address1) {
            logical_pages = free_logical_address2;
        } else {
            logical_pages = free_logical_address1;
        }
    }
    _map_physical_address(0x0, free_logical_address1);
    _flush_tlb(free_logical_address1);
    _map_physical_address(0x0, free_logical_address2);
    _flush_tlb(free_logical_address2);
}

void *
memory_allocate_page(void)
{
    void *pages;
    void *next;
    struct free_pages *logical_pages;

    if(! free_list) {
        /* Freak out! (at least until swapping is implemented) */
        panic("Out of memory");
    }

    pages         = free_list;
    logical_pages = _map_physical_address(pages, free_logical_address1);

    if(logical_pages->num_pages == 1) {
        next = logical_pages->next;
    } else {
        struct free_pages *logical_next;

        next         = (((char *) pages) + MEMORY_PAGE_SIZE);
        logical_next = _map_physical_address(next, free_logical_address2);
        memcpy(logical_next, logical_pages, sizeof(struct free_pages));
        logical_next->num_pages--;

        _map_physical_address(0x0, logical_next);
        _flush_tlb(logical_next);
    }
    free_list = next;

    _map_physical_address(0x0, logical_pages);
    _flush_tlb(logical_pages);

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

    remove_identity_map(second); /* XXX is this ok? */
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

    /* We assume that the incoming page to be freed is identity
     * mapped; this may change! */
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

void *
memory_map_physical_address(void *addr)
{
    void *logical_addr;

    logical_addr = _find_free_logical_address();

    _map_physical_address(addr, logical_addr);

    return logical_addr;
}
