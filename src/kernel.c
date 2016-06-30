#include "includes/global.h"
#include "gdt.c"
#include "includes/kstdio.h"
#include "includes/keyboard.h"
#include "memory/mm.c"



// globals here






void __stack_chk_fail(void) {}



void kmain(multiboot_info_t* mbd, unsigned int magic)
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

    kputs(itoa(mbd->flags, 2), currentConsole);
    
    
    
    while(1)
        asm volatile ("hlt");
}

