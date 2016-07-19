#include "../includes/stdio.h"

extern void breakpoint(void);
static WORD num_i[MAX_TASKS];

void numeric(u_int pid){
    num_i[pid] = 0;
    for(; num_i[pid]<100000000; num_i[pid]++){
        putstr(itoa(num_i[pid], 10), pid);
        nl(pid);
        //breakpoint();
    }

    // fnish up!
    // explicit syscall to notify kernel that we're done:

    doexit(pid);
}

void continue_numeric(u_int pid){
    for(; num_i[pid]<100000000; num_i[pid]++){
        putstr(itoa(num_i[pid], 10), pid);
        nl(pid);
        //breakpoint();
    }

    // fnish up!
    // explicit syscall to notify kernel that we're done:

    doexit(pid);
}
