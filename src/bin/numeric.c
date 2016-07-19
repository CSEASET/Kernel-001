#include "../includes/stdio.h"

extern void breakpoint(void);
static char num_i;

void numeric(u_int pid){
    num_i = 0;
    for(; num_i<100000000; num_i++){
        putstr(itoa(num_i, 10), pid);
        nl(pid);
        //breakpoint();
    }

    // fnish up!
    // explicit syscall to notify kernel that we're done:

    doexit(pid);
}

void continue_numeric(u_int pid){
    for(; num_i<100000000; num_i++){
        putstr(itoa(num_i, 10), pid);
        nl(pid);
        //breakpoint();
    }

    // fnish up!
    // explicit syscall to notify kernel that we're done:

    doexit(pid);
}
