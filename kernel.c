#include "includes/keyboard_map.h"

/* there are 25 lines each of 80 columns; each element takes 2 bytes */
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C


// globals here

char *vidptr = (char*)0xb8000;  //video mem begins here.
unsigned int current_loc = 0;


// externs here

extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long * idt_ptr);

void __stack_chk_fail(void) {}


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
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	/* populate IDT entry of keyboard's interrupt */
	keyboard_address = (unsigned long)keyboard_handler; 
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;
	

	/*     Ports
	*	 PIC1	PIC2
	*Command 0x20	0xA0
	*Data	 0x21	0xA1
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

void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	write_port(0x21 , 0xFD);
}


void keyboard_handler_main(void) {
    putchar('a', current_loc);
    current_loc += 2;
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
		vidptr[current_loc++] = keyboard_map[(unsigned char) keycode];
		vidptr[current_loc++] = 0x07;	
	}
}

void kmain(void)
{
    const char *str = "my first kernel";
    current_loc = 0;
    unsigned int j = 0;

    /* this loops clears the screen
    * there are 25 lines each of 80 columns; each element takes 2 bytes */
    while(j < 80 * 25 * 2) {
        /* blank character */
        vidptr[j] = '_';
        /* attribute-byte - light grey on black screen */
        vidptr[j+1] = 0x07;         
        j = j + 2;
    
    }

    j = 0;

    /* this loop writes the string to video memory */
    while(str[j] != '\0') {
        /* the character's ascii */
        vidptr[current_loc] = str[j];
        /* attribute-byte: give character black bg and light grey fg */
        vidptr[current_loc+1] = 0x07;
        ++j;
        current_loc = current_loc + 2;
    
    }

    //putchar('a', current_loc);

    idt_init();
    kb_init();

    while(1);
    return;

}

void putchar(unsigned char ch, unsigned int pos){
    vidptr[pos] = keyboard_map[ch];
    vidptr[pos+1] = 0x07;
}
