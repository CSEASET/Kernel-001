#include "gdt.c"
#include "memory/multiboot.h"
#include "includes/kstdio.h"
#include "includes/keyboard.h"
#include "process/sched.h"

//#include "memory/mm.c"



// globals here






void __stack_chk_fail(void) {}



void kmain(multiboot_info_t* mbd, unsigned int magic)
{
    const char *str = "my first Kernel";
    
    // setup scheduler
    sched_init();
    

    gdt_init();
    idt_init();
    irq_init();

    // console handling
    console_init();
    currentConsole = &consoles[0];
    kernelConsole = &consoles[3];


    kputs(str, currentConsole);
    drawConsole(currentConsole);

    //kputs(itoa(mbd->flags, 2), currentConsole);
    
    // we should enable interrupts now as we have completed kernel initialization
    enable_interrupts();

    while(1)
        asm volatile ("hlt");
}

