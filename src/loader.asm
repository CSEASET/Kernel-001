
bits 32
section .text
        ;multiboot spec
        align 4
        dd 0x1BADB002              ;magic
        dd 0x00                    ;flags
        dd - (0x1BADB002 + 0x00)   ;checksum. m+f+c should be zero

global start
global keyboard_handler, timer_handler
global read_port
global write_port
global load_idt
global load_gdt, disable_cursor, enable_interrupts, disable_interrupts, breakpoint
global process1_stack_space, process2_stack_space, process3_stack_space
global save_kernel_stack, load_kernel_stack, switch_esp

extern kmain 		;this is defined in the c file
extern keyboard_handler_main, timer_handler_main

breakpoint:
    xchg bx, bx
    ret

disable_cursor:
	 mov   bx,0x1c1b                     ; set cursor invisable
     call  ChangeCursor

ChangeCursor:
          pushad
          mov   dx,0x3D4
          mov   al,0x0A
          mov   ah,bh
          out   dx,ax
          inc   ax
          mov   ah,bl
          out   dx,ax
          popad
          ret

read_port:
	mov edx, [esp + 4]
			;al is the lower 8 bits of eax
	in al, dx	;dx is the lower 16 bits of edx
	ret

write_port:
	mov   edx, [esp + 4]    
	mov   al, [esp + 4 + 4]  
	out   dx, al  
	ret

load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	;sti 				;turn on interrupts
	ret

load_gdt:
	mov edx, [esp + 4]
	lgdt [edx]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    jmp 8:flush_cpu
flush_cpu:
	ret

keyboard_handler:                 
    cli     ;; disable interrupts while we handle this
	pushad
    pushfd
    ;; call load_kernel_stack
	call    keyboard_handler_main

    popfd
	popad
    sti     ;; enable interrupts as we are finished
	iretd

timer_handler:
    cli     ;; disable interrupts while we handle this
    pushad
    pushfd
    
    ;;call load_kernel_stack    

    call    timer_handler_main
    
    popfd
    popad
    sti     ;; enable interrupts
    iretd

load_kernel_stack:
    mov esp, [kernel_stack]
    ret

save_kernel_stack:
    mov [kernel_stack], esp;
    ret

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

start:
    cli
    call breakpoint
	mov esp, kernel_stack_space
    push ebx
	call kmain
    cli
.loop:
	hlt 				;halt the CPU
    jmp .loop


switch_esp: ;; load the esp in eax but save the kernel stack too
    mov [kernel_stack], esp
    mov esp, eax
    ret

section .bss
resb 8192   ;; 8KB for kernel stack
kernel_stack_space:

resb 8192   ;; 8KB for process 1
process1_stack_space:

resb 8192   ;; another 8KB for process 2
process2_stack_space:

resb 8192   ;; another 8KB for process 3
process3_stack_space:

kernel_stack: resb 4
