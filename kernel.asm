;;kernel.asm

;nasm directive - 32 bit
bits 32
section .text
        ;multiboot spec
        align 4
        dd 0x1BADB002            ;magic
        dd 0x00                  ;flags
        dd - (0x1BADB002 + 0x00) ;checksum. m+f+c should be zero

global start
global load_idt
global keyboard_handler
global read_port
global write_port

extern kmain            ;kmain is defined in the c file
extern keyboard_handler_main

start:
  cli           ;block interrupts
  mov esp, stack_space  ;set stack pointer
  call kmain
  hlt           ;halt the CPU


load_idt:
    mov edx, [esp + 4]
    lidt [edx]
    sti
    ret

keyboard_handler:                 
    call    keyboard_handler_main
    iretd

read_port:
    mov edx, [esp + 4]
    in al, dx   
    ret

write_port:
    mov   edx, [esp + 4]    
    mov   al, [esp + 4 + 4]  
    out   dx, al  
    ret


section .bss
resb 8192       ;8KB for stack
stack_space:
