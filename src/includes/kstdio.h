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
    char lineBuffer[2000];
    int lbLoc;
};

// prototypes
void kclearScreen(struct console* c);
void drawConsole();
void knewLine(struct console* c);
void kputs(const char *, struct console *);
char * strcpy(char *, char *);
void scrollConsole(struct console*);
void scrollConsoleNL(struct console*);
void parseKernelCommand(struct console*);



struct console consoles[4];
char con0[SCREENSIZE], con1[SCREENSIZE], con2[SCREENSIZE], con3[SCREENSIZE];

// points to the currently displayed console
struct console *currentConsole;

// points to the kernel debug console
struct console *kernelConsole;

char *vidptr = (char*)0xb8000;  //video mem begins here.
int GLOBAL_CAPSLOCK = FALSE;
struct console phyConsole;

// console_init: initialize console buffers

void console_init(){
    int i=0;
    phyConsole.vidptr = vidptr;
    phyConsole.currentLocation = 0;

    consoles[0].vidptr = con0;
    consoles[0].currentLocation = 0;
    strcpy(consoles[0].cno, "F1");
    consoles[0].lineBuffer[0] = '\0';
    consoles[0].lbLoc = 0;

    consoles[1].vidptr = con1;
    consoles[1].currentLocation = 0;
    strcpy(consoles[1].cno, "F2");
    consoles[1].lineBuffer[0] = '\0';
    consoles[1].lbLoc = 0;

    consoles[2].vidptr = con2;
    consoles[2].currentLocation = 0;
    strcpy(consoles[2].cno, "F3");
    consoles[2].lineBuffer[0] = '\0';
    consoles[2].lbLoc = 0;

    consoles[3].vidptr = con3;
    consoles[3].currentLocation = 0;
    strcpy(consoles[3].cno, "F4");
    consoles[3].lineBuffer[0] = '\0';
    consoles[3].lbLoc = 0;

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
        *(c->vidptr + j) = BLANK_CHAR;
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

    // save character in lineBuffer of the console
    c->lineBuffer[c-lbLoc++] = ch;

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

    kputs(PROMPT_STR, c);

    // handle command input on kernelConsole
    if(c == kernelConsole){
        c->lineBuffer[c->lbLoc] = '\0';
        parseKernelCommand(c); 

        // reset lineBuffer now
        c->lbLoc = 0;
    }

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
    if(currentConsole == c){ // only copy if it is the current active console 
        kmemcopy(c->vidptr, phyConsole.vidptr, SCREENSIZE);
    }
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
        kputchar(BLANK_CHAR, c);
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
        kputchar(BLANK_CHAR, c);
    }

    c->currentLocation = SCREENSIZE - i;
}

void swap(char * a, char * b){
    char t;
    t = *a;
    *a = *b;
    *b = t;
}

void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        swap((str+start), (str+end));
        start++;
        end--;
    }
}

// itoa: convert int to string 

char* itoa(int num, int base)
{
    int i = 0;
    int isNegative = FALSE;
    char * str = itoaBuffer;
 
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = TRUE;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str, i);
 
    return str;
}

// parseKernelCommand: parse commands entered in the kernelConsole and takes action accordingly

void parseKernelCommand(struct console *c){
    char * l = c->lineBuffer;

    switch(l[0]){
        case '1':
    }
}
