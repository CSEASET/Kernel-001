#include "../bin/numeric.c"

typedef struct registers {
	DWORD eax, ebx, ecx, edx, esi, edi , ebp, eip, esp;
	WORD cs, ds, es, gs, fs, ss;
} regs;


typedef struct task_structure {
    regs r;
    struct console * con;
    int valid;
} task;



task task_q[MAX_TASKS];
int currentTask, nextTask;

struct MUTEX schedLock;

// context_switch: switch currentTask's context with nextTask's context

void context_switch(int cTask, int nTask){
    kputs(itoa(cTask,10), kernelConsole);
    kputs(itoa(nTask,10), kernelConsole);
    if(cTask == nTask){
        kputs("current task has been rescheduled! resuming it...\n", kernelConsole);
        return;
    }

    // performing a context switch now

    // step 1 - saving currentTask PCB to task_q
    

    
}

// newTask: run a new task on an available console

void newTask(int processNo){
    int i=0, freeSlot = -1;
    for(; i<MAX_TASKS; i++){
        if(task_q[i].valid == FALSE){
           // found a free task!
           freeSlot = i;
           break;
        }
    }

    if(freeSlot == -1) {
        kputs("Error: MAX_TASKS are already running!", kernelConsole);
        return;
    }
    
    // critical section starts
   
    task_q[freeSlot].valid = TRUE;
    task_q[freeSlot].con = &consoles[freeSlot];


    //currentTask = freeSlot;
    // end critical section
    // since its done, we should enable interrupts
    
   // enable_interrupts();
/*
    switch(processNo){
        case 1:
            numeric(freeSlot);
            return;
   }*/
}

// sched: goes through the task queue and selects a task to queue

void sched(){
    kputs("started scheduling...\n", kernelConsole);
    int i;
    
    disable_interrupts();

    // lock our environment
    mutex_lock(&schedLock);

    nextTask = currentTask;
    
    if(currentTask == (MAX_TASKS - 1)){
        nextTask = 0;
    }

    for(i=nextTask; i<MAX_TASKS; i++){
        if(!task_q[i].valid)
            continue;

        nextTask = i; // 1. we've selected the next task. 2. proceed to schedule it. 3. ??? 4. profit! 
        break;        
    }

    // calling our master switcher
    context_switch(currentTask, nextTask);

    // we're done! finish up (enable interrupts and release mutex)
    enable_interrupts();
    mutex_unlock(&schedLock);
}

// kgetConsole: returns a pointer to the console glued with the process specified by the process ID

struct console * kgetConsole(u_int pid){
    return task_q[pid].con;
}

// kexit: called by an exitting process, marks the process's slot in the queue as free

void kexit(u_int pid){
    task_q[pid].valid = FALSE;
}
