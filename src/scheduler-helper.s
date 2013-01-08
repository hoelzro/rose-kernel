%include "struct-offsets.s"

; XXX should we include this ↓ in the generated file?
%define STRUCT_SCHEDULER_KERNEL_TASK_OFFSET 0x00
%define STRUCT_TASK_SCHEDULER_OFFSET        0x00
%define STRUCT_TASK_REGISTERS_OFFSET        0x0C
%define STRUCT_TASK_STACK_OFFSET            0x2C

global _resume_task
global _dealloc_and_jump_to_task
global _get_current_task
global scheduler_yield

extern memory_free_page

; struct task *task
; struct task *current
; EIP
; old EAX

_resume_task:
    PUSH EAX

    MOV EAX, ESP
    MOV EAX, [EAX + 8]

    ; we don't need to explicitly preserve EAX, as it is currently on the stack
    ; of the current task

    MOV [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EDI_OFFSET], EDI
    MOV [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_ESI_OFFSET], ESI
    MOV [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EBP_OFFSET], EBP
    MOV [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_ESP_OFFSET], ESP
    MOV [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EBX_OFFSET], EBX
    MOV [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EDX_OFFSET], EDX
    MOV [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_ECX_OFFSET], ECX

    ; we don't need to preserve EBX, as we've already stashed it in the task
    ; context above
    MOV EBX, [ESP]
    MOV [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EAX_OFFSET], EBX

    ; preserve SS register (not needed yet)
    ; preserve EIP (done implicitly by CALL)

    MOV EAX, ESP
    MOV EAX, [EAX + 12]

    MOV EDI, [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EDI_OFFSET]
    MOV ESI, [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_ESI_OFFSET]
    MOV EBP, [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EBP_OFFSET]
    MOV ESP, [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_ESP_OFFSET]
    MOV EBX, [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EBX_OFFSET]
    MOV EDX, [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EDX_OFFSET]
    MOV ECX, [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_ECX_OFFSET]
    ;MOV SS,  [EAX + STRUCT_TASK_STACK_OFFSET] ; do this after we integrate this code into the kernel
    MOV EAX, [EAX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EAX_OFFSET] ; must occur last

    ADD ESP, 4

    RET ; pops EIP and resumes execution of task

; struct task *kernel_task
; struct task *current_task
; EIP

_dealloc_and_jump_to_task:
    ; we can be careless about register usage, since
    ; they will be restored to the kernel task's registers
    ; soon enough
    MOV EAX, [ESP + 4] ; task to cleanup
    MOV EBX, [ESP + 8] ; task to resume

    ; XXX duplicate code
    MOV EDI, [EBX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EDI_OFFSET]
    MOV ESI, [EBX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_ESI_OFFSET]
    MOV EBP, [EBX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EBP_OFFSET]
    MOV ESP, [EBX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_ESP_OFFSET]
    MOV EDX, [EBX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EDX_OFFSET]
    MOV ECX, [EBX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_ECX_OFFSET]
    PUSH DWORD [EBX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EAX_OFFSET] ; we need to do it this way
    ;MOV SS,  [EBX + STRUCT_TASK_STACK_OFFSET] ; do this after we integrate this code into the kernel
    MOV EBX, [EBX + STRUCT_TASK_REGISTERS_OFFSET + STRUCT_REGISTERS_EBX_OFFSET] ; must happen last

    PUSH EAX
    CALL memory_free_page
    ADD ESP, 4

    POP EAX
    ADD ESP, 4
    RET

; XXX this works as long as we have a single page for the task
_get_current_task:
    MOV EAX, ESP
    AND EAX, 0xFFFFF000
    RET

scheduler_yield:
    PUSH EAX
    PUSH EBX

    CALL _get_current_task

    MOV EBX, EAX
    ; XXX can we conditionally compile this out?
    ADD EBX, STRUCT_TASK_SCHEDULER_OFFSET
    MOV EBX, [EBX]

    ; XXX can we conditionally compile this out?
    ADD EBX, STRUCT_SCHEDULER_KERNEL_TASK_OFFSET

    PUSH EBX
    PUSH EAX

    MOV EBX, [ESP + 8]
    MOV EAX, [ESP + 12]
    CALL _resume_task
    ADD ESP, 16

    RET
