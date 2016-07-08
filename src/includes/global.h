

// GLOBAL DEFINES

#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define TRUE				0x01
#define FALSE				0x00

#define SUCCESS				0
#define FAIL				-1

#define ASM					__asm__ __volatile__
#define PACKED				__attribute__( (packed) )

#define NULL				((void *)0x00000000)

#define BLANK_CHAR ' '
#define PROMPT_STR "root:$ "
// data types

typedef int					BOOL;
typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef unsigned long		DWORD;
typedef unsigned int u_int;


extern void enable_interrupts(void);
extern void disable_interrupts(void);
extern void disable_cursor(void);

// global buffers

// buffer for itoa

char itoaBuffer[200];




#include "../sync/mutex.h"
