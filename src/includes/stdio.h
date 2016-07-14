// this is a library of stdio funtions which interface with the kernel

extern void kputs(const char*, struct console *);
extern void knewLine(struct console *);
extern struct console* kgetConsole(u_int);
extern void kexit(u_int);


// puts: figures out which console to print to and then prints the string passed to it via a syscall

void putstr(const char* str, u_int pid){
    struct console *c = kgetConsole(pid);
    kputs(str, c);
}

// nl: prints newline character
void nl(u_int pid){
    struct console *c = kgetConsole(pid);
    knewLine(c);
}

// doexit: explicit call which tells the kernel that the process has finished

void doexit(u_int pid){
    kexit(pid);
}
