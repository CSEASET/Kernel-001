#include "includes/stdio.h"

void numbers(u_int pid){
    DWORD i=0;
    for(i<100000000; i++){
        puts(atoi(i, 10), pid);
        nl(pid);
    }

    // fnish up!
    // explicit syscall to notify kernel that we're done:

    exit(pid);
}
