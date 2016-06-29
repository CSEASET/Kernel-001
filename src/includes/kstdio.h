// Kernel STDIO library
#include "global.h"

/* there are 25 lines each of 80 columns; each element takes 2 bytes */
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES


// prototypes
void kclearScreen(struct console* c);


struct console {
	char *vidptr;
	int currentLocation;
};

struct console consoles[4];
char con0[SCREENSIZE], con1[SCREENSIZE], con2[SCREENSIZE], con3[SCREENSIZE];
struct console *currentVidPtr;
char *vidptr = (char*)0xb8000;  //video mem begins here.
int GLOBAL_CAPSLOCK = FALSE;

// console_init: initialize console buffers

void console_init(){
    int i=0;
    consoles[0].vidptr = (char*)con0;
    consoles[0].currentLocation = 0;
    consoles[1].vidptr = con1;
    consoles[1].currentLocation = 0;
    consoles[2].vidptr = con2;
    consoles[2].currentLocation = 0;
    consoles[3].vidptr = con3;
    consoles[3].currentLocation = 0;

    for(; i<4; i++){
        kclearScreen(&consoles[i]);
    }
}

// clearScreen: clears the video buffer pointed by *vidptr

void kclearScreen(struct console* c){
    if(c == 0)
        c = currentVidPtr;
	int j = 0;
	while(j < 80 * 25 * 2) {
        /* blank character */
        c->vidptr[j] = '_';
        /* attribute-byte - light grey on black screen */
        c->vidptr[j+1] = 0x07;         
        j += 2;
    
    }
    drawConsole();
}

// kprintchar: prints char to video buffer

void kputchar(unsigned char ch, struct console* c = NULL){
    if(c == NULL)
        c = currentVidPtr;
    if(ch == '\n'){
    	knewLine(c);
	    return;
    }
    c->vidptr[c->currentLocation++] = ch;
    c->vidptr[c->currentLocation++] = 0x07;
    drawConsole();
}

// kputs: prints str to buffer

void kputs(const char *str, struct console* c = NULL){
    if(c == NULL)
        c = currentVidPtr;
	unsigned int i = 0;
	while (str[i] != '\0') {
		kputchar(str[i], c);
	}
}

// knewLine: moves cursor to next line in the buffer

void knewLine(struct console *c = NULL){
    if(c == NULL)
        c = currentVidPtr;
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	c->currentLocation = c->currentLocation + (line_size - c->currentLocation % (line_size));
    drawConsole();
}

// kmemcopy: copies memory from source to dest

void kmemcopy(char *source, char *dest, int size){
	int i=0;
	while(i<size){
		*(source + i) = *(dest + i);
	}
}

// drawConsole: copies the current video buffer to the video buffer

void drawConsole(){
    kmemcopy(currentVidPtr->vidptr, vidptr, SCREENSIZE);
} 


