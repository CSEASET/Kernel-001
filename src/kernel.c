#include "includes/global.h"
#include "gdt.c"
#include "includes/kstdio.h"
#include "includes/keyboard.h"



// globals here






void __stack_chk_fail(void) {}



void kmain(void)
{
    const char *str = "my first Kernel";
    
    

    gdt_init();
    idt_init();
    kb_init();

    // console handling
    console_init();
    currentConsole = &consoles[0];


    kputs(str, currentConsole);
    drawConsole(currentConsole);
    
    
    
    while(1)
        asm volatile ("hlt");
}

