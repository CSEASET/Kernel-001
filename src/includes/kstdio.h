// Kernel STDIO library
#include "global.h"

/* there are 25 lines each of 80 columns; each element takes 2 bytes */
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES


struct console {
	char *vidptr;
	int currentLocation;
    char cno[3];
};

// prototypes
void kclearScreen(struct console* c);
void drawConsole();
void knewLine(struct console* c);
void kputs(const char *, struct console *);
char * strcpy(char *, char *);
void scrollConsole(struct console*);
void scrollConsoleNL(struct console*);

struct console consoles[4];
char con0[SCREENSIZE], con1[SCREENSIZE], con2[SCREENSIZE], con3[SCREENSIZE];
struct console *currentConsole;
char *vidptr = (char*)0xb8000;  //video mem begins here.
int GLOBAL_CAPSLOCK = FALSE;
struct console phyConsole;

// console_init: initialize console buffers

void console_init(){
    int i=0;
    phyConsole.vidptr = vidptr;
    phyConsole.currentLocation = 0;

    consoles[0].vidptr = (char*)con0;
    consoles[0].currentLocation = 0;
    strcpy(consoles[0].cno, "F1");
    consoles[1].vidptr = con1;
    consoles[1].currentLocation = 0;
    strcpy(consoles[1].cno, "F2");
    consoles[2].vidptr = con2;
    consoles[2].currentLocation = 0;
    strcpy(consoles[2].cno, "F3");
    consoles[3].vidptr = con3;
    consoles[3].currentLocation = 0;
    strcpy(consoles[3].cno, "F4");

    for(; i<4; i++){
        kclearScreen(&consoles[i]);
        kputs("Console : ", &consoles[i]);
        kputs(consoles[i].cno, &consoles[i]);
        kputs("\n", &consoles[i]);
    }

    kclearScreen(&phyConsole);
}

// clearScreen: clears the video buffer pointed by *vidptr

void kclearScreen(struct console* c){
    if(c == NULL)
        c = currentConsole;
	unsigned int j = 0;
	while(j < 80 * 25 * 2) {
        /* blank character */
        *(c->vidptr + j) = '-';
        /* attribute-byte - light grey on black screen */
        *(c->vidptr + j+1) = 0x07;         
        j += 2;
    }
    drawConsole(c);
}

// kprintchar: prints char to video buffer

void kputchar(unsigned char ch, struct console* c ){
    if(c == NULL)
      c = currentConsole;

    if(c->currentLocation == SCREENSIZE)
        scrollConsole(c);

    if(ch == '\n'){
    	knewLine(c);
	    return;
    }

    c->vidptr[c->currentLocation++] = ch;
    c->vidptr[c->currentLocation++] = 0x07;


    drawConsole(c);
}

// kputs: prints str to buffer

void kputs(const char *str, struct console* c){
    if(c == NULL)
        c = currentConsole;
	unsigned int i = 0;
	while (str[i] != '\0') {
		kputchar(str[i], c);
        i++;
	}
}

// knewLine: moves cursor to next line in the buffer

void knewLine(struct console *c ){
    if(c == NULL)
        c = currentConsole;
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	c->currentLocation = c->currentLocation + (line_size - c->currentLocation % (line_size));
    if(c->currentLocation > SCREENSIZE){
        c->currentLocation -= (BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE);
        scrollConsoleNL(c);
    }

    kputs("<root>$ ", c);
    drawConsole(c);
}

char * strcpy(char *dest, char *source){
    int i=0;
    while(source[i] != '\0'){
        *(dest + i) = *(source + i);
        i++;
    }
    dest[i] = '\0';
    return dest;
}

// kmemcopy: copies memory from source to dest

void kmemcopy(char *source, char *dest, unsigned int size){
	unsigned int i=0;
	while(i<size){
		*(dest + i) = *(source + i);
        i++;
	}
}

// drawConsole: copies the current video buffer to the video buffer

void drawConsole(struct console *c){
    kmemcopy(c->vidptr, phyConsole.vidptr, SCREENSIZE);
} 

// scrollConsole: scrolls the console buffer by one line 

void scrollConsole(struct console *c){
    u_int i,j,k;
    i = j  = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
    k = SCREENSIZE - i -i;

//    for(; i<SCREENSIZE; i+= j){
        kmemcopy((c->vidptr +i +i), (c->vidptr+i), k);
  //  }
    
    c->currentLocation -= i;
//    return;
    for(k=0; k<COLUMNS_IN_LINE; k++){
        kputchar('-', c);
    }

    c->currentLocation -= i;
}


// scrollConsoleNL: scrolls the console buffer by one line 

void scrollConsoleNL(struct console *c){
    u_int i,j,k;
    i = j  = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
    k = SCREENSIZE - i -i;

        kmemcopy((c->vidptr +i +i), (c->vidptr+i), k);
    
    c->currentLocation = SCREENSIZE - i;
    
    for(k=0; k<COLUMNS_IN_LINE; k++){
        kputchar('-', c);
    }

    c->currentLocation = SCREENSIZE - i;
}
