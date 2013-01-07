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

#include <rose/memory.h>
#include <rose/scheduler.h>
#include <rose/stdint.h>
#include <rose/string.h>

struct registers {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
} __attribute__((packed));

struct task {
    struct task *next;
    struct task *previous;
    struct registers registers;
    char stack[];
} __attribute__((packed));

struct scheduler {
    struct task kernel_task;
    struct task *next_task;
} __attribute__((packed));

static struct scheduler global_scheduler;

extern void _resume_task(struct task *current, struct task *task);
extern void _dealloc_and_jump_to_task(struct task *current, struct task *kernel);

static void
_cleanup_task(void)
{
    struct scheduler *scheduler = &global_scheduler;
    struct task *current        = scheduler->next_task;

    if(current == current->next) {
        scheduler->next_task = NULL;
    } else {
        current->previous->next = current->next;
        current->next->previous = current->previous;
        scheduler->next_task    = current->previous;
    }

    _dealloc_and_jump_to_task(current, &scheduler->kernel_task);
}

static void
_push_to_task_stack(struct task *task, uint32_t value)
{
    task->registers.esp -= 4;
    *((uint32_t *) task->registers.esp) = value;
}

void
scheduler_init(void)
{
    memset(&global_scheduler, 0, sizeof(struct scheduler));
}

void
scheduler_add_process(void (*code)(void))
{
    struct scheduler *scheduler = &global_scheduler;
    struct task *task;

    task = memory_allocate_page(); // a single page is enough (for now)
    memset(task, 0, MEMORY_PAGE_SIZE);

    task->registers.esp = ((uint32_t) task) + MEMORY_PAGE_SIZE;

    _push_to_task_stack(task, (uint32_t) _cleanup_task); /* set up exit trampoline */
    _push_to_task_stack(task, (uint32_t) code);
    _push_to_task_stack(task, 0); /* set up EAX */

    if(scheduler->next_task) {
        task->previous                       = scheduler->next_task->previous;
        scheduler->next_task->previous->next = task;
        scheduler->next_task->previous       = task;
    } else {
        task->previous       = task;
        scheduler->next_task = task;
    }
    task->next = scheduler->next_task;

    // set up segments (CS, DS, SS, ES, GS, FS)
    //   all segments are the same as the kernel (for now)
    // set up task segment (we'll do this later, as we're not really switching tasks yet)
    // set up paging table for new process (not yet)
}

void
scheduler_run(void)
{
    struct scheduler *scheduler = &global_scheduler;

    while(scheduler->next_task) {
        _resume_task(&scheduler->kernel_task, scheduler->next_task);

        if(scheduler->next_task) {
            scheduler->next_task = scheduler->next_task->next;
        }
    }
}

void
scheduler_yield(void)
{
    _resume_task(global_scheduler.next_task, &global_scheduler.kernel_task);
}
