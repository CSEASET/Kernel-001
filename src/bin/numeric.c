#include "../includes/stdio.h"

extern void breakpoint(void);

void numeric(u_int pid){
    DWORD i=0;
    for(; i<100000000; i++){
        putstr(itoa(i, 10), pid);
        nl(pid);
        //breakpoint();
    }

    // fnish up!
    // explicit syscall to notify kernel that we're done:

    doexit(pid);
}
