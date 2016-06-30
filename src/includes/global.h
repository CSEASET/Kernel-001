// GLOBAL DEFINES

#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define FALSE 0
#define TRUE 1
#define NULL 0



// data types

typedef unsigned int u_int;




// global buffers

// buffer for itoa

char itoaBuffer[200];
