// Keyboard handling

#include "keyboard_map.h"



#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

#define ENTER_KEY_CODE 0x1C

// externs here

extern void keyboard_handler(void);
extern void timer_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long * idt_ptr);


struct IDT_entry{
    unsigned short int offset_lowerbits;
    unsigned short int selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

void idt_init(void)
{
    unsigned long keyboard_address;
    unsigned long timer_address;
    unsigned long idt_address;
    unsigned long idt_ptr[2];

    /* populate IDT entry of keyboard's interrupt */
    keyboard_address = (unsigned long)keyboard_handler; 
    IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
    IDT[0x21].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[0x21].zero = 0;
    IDT[0x21].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

    /* populate IDT entry of timer's interrupt */
    timer_address = (unsigned long) timer_handler; 
    IDT[0x20].offset_lowerbits = timer_address & 0xffff;
    IDT[0x20].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[0x20].zero = 0;
    IDT[0x20].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[0x20].offset_higherbits = (timer_address & 0xffff0000) >> 16;
    

    /*     Ports
    *    PIC1   PIC2
    *Command 0x20   0xA0
    *Data    0x21   0xA1
    */

    /* ICW1 - begin initialization */
    write_port(0x20 , 0x11);
    write_port(0xA0 , 0x11);

    /* ICW2 - remap offset address of IDT */
    /*
    * In x86 protected mode, we have to remap the PICs beyond 0x20 because
    * Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
    */
    write_port(0x21 , 0x20);
    write_port(0xA1 , 0x28);

    /* ICW3 - setup cascading */
    write_port(0x21 , 0x00);  
    write_port(0xA1 , 0x00);  

    /* ICW4 - environment info */
    write_port(0x21 , 0x01);
    write_port(0xA1 , 0x01);
    /* Initialization finished */

    /* mask interrupts */
    write_port(0x21 , 0xff);
    write_port(0xA1 , 0xff);

    /* fill the IDT descriptor */
    idt_address = (unsigned long)IDT ;
    idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16 ;

    load_idt(idt_ptr);
}

void irq_init(void){
    /* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
    // but we also need IRQ0 0xFC is 11111100
    write_port(0x21 , 0xFC);
}


void keyboard_handler_main(void) {
    //putchar('a', current_loc);
    //current_loc += 2;
    unsigned char status;
    char keycode;

    /* write EOI */
    write_port(0x20, 0x20);

    status = read_port(KEYBOARD_STATUS_PORT);
    /* Lowest bit of status will be set if buffer is not empty */
    if (status & 0x01) {
        keycode = read_port(KEYBOARD_DATA_PORT);
        if(keycode < 0)
            return;

        switch(keycode){
            case 0:
                break;

            case 58:
                GLOBAL_CAPSLOCK = !GLOBAL_CAPSLOCK;
                break;
            
            case 59:
                currentConsole = &consoles[0];
                break;

            case 60:
                currentConsole = &consoles[1];
                break;

            case 61:
                currentConsole = &consoles[2];
                break;

            case 62:
                currentConsole = &consoles[3];
                break;

            case 14:
                if(currentConsole->currentLocation > 0){
                    currentConsole->vidptr[currentConsole->currentLocation-2] = ' ';
                    currentConsole->vidptr[currentConsole->currentLocation-1] = 0x07;
                    currentConsole->currentLocation -= 2;
                }
                break;

            case 28:
                kbnewLine(currentConsole);
                break;
 
            default:
                if(GLOBAL_CAPSLOCK){
                    kbputchar((char) (keyboard_map[(unsigned char) keycode] - 32), currentConsole);
                 //   currentConsole->vidptr[currentConsole->currentLocation++] = ;
                 //   currentConsole->vidptr[currentConsole->currentLocation++] = 0x07;   
                } else {
                    kbputchar(keyboard_map[(unsigned char) keycode], currentConsole);
                    //currentConsole->vidptr[currentConsole->currentLocation++] = keyboard_map[(unsigned char) keycode];
                    //currentConsole->vidptr[currentConsole->currentLocation++] = 0x07;
                }
        }

        drawConsole(currentConsole);
    }
}
