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

#ifndef ROSE_MEMORY_H
#define ROSE_MEMORY_H

struct multiboot_info;

void memory_init_gdt(void);
void memory_init_paging(void *kernel_start, void *kernel_end);
void memory_detect(void *kernel_end, struct multiboot_info *mboot);
void *memory_allocate_page(void);
void memory_free_page(void *page);

#define MEMORY_PAGE_SIZE (1 << 12)

/* XXX define these in terms of MEMORY_PAGE_SIZE */
#define MEMORY_PAGE_ALIGN(addr)\
    ((void *) (((long)addr) & ~0xFFF))

#define MEMORY_IS_PAGE_ALIGNED(addr)\
    ((((long)addr) & 0xFFF) == 0)

#endif
