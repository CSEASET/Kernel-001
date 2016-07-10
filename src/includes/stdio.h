// this is a library of stdio funtions which interface with the kernel

extern struct console;
extern void kputs(const char*, console *);
extern void knewLine(console *);
extern void kgetConsole(u_int);
extern void kexit(u_int);


// puts: figures out which console to print to and then prints the string passed to it via a syscall

void puts(const char* str, u_int pid){
    console *c = kgetConsole(pid);
    kputs(str, c);
}

// nl: prints newline character
void nl(u_int pid){
    console *c = kgetConsole(pid);
    knewLine(c);
}

// exit: explicit call which tells the kernel that the process has finished

void exit(u_int pid){
    kexit(pid);
}
