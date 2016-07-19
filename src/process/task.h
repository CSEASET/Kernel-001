#include "../bin/numeric.c"

//extern void loadp0(void);
//extern void loadp1(void);
//extern void loadp2(void);

extern WORD process1_stack_space;
extern WORD process2_stack_space;
extern WORD process3_stack_space;


typedef struct registers {
	DWORD eax, ebx, ecx, edx, esi, edi , ebp, eip, esp;
	WORD cs, ds, es, gs, fs, ss;
} regs;


typedef struct task_structure {
    DWORD esp;
    struct console * con;
    int valid;
    int processNo;
    int firstRun;
    int slot;
} task;


task task_q[MAX_TASKS];
int currentTask, nextTask;

struct MUTEX schedLock;

// context_switch: switch currentTask's context with nextTask's context

void context_switch(int cTask, int nTask){
    kputs(itoa(cTask,10), kernelConsole);
    kputs(itoa(nTask,10), kernelConsole);

    if(nTask == -1){
        kputs("no task running...", kernelConsole);
        return;
    }

    if(cTask == nTask){
        kputs("current task has been rescheduled! resuming it...\n", kernelConsole);
        return;
    }

    task * t = &task_q[nTask];

    if(t->firstRun){ // run it now
        return;
        t->firstRun = FALSE;
        //mutex_unlock(&schedLock);
        breakpoint();
        
        //ASM("movl %0, %%esp;" : : "m" (t->esp) :);
        
        enable_interrupts();
        
        breakpoint();
        
        switch(1){
            case 1:
                numeric(0);
                break;
        }
        return; // incase our process ends
    }
    

    // performing a context switch now

    // step 1 - saving currentTask PCB to task_q
    
    
    /*breakpoint();

    ASM ("movl %%ecx, %0;" : "=m" (r->eax) ::);
    
    kputs(itoa(r->eax, 16), kernelConsole);
    breakpoint();
    */
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
    task * t = &task_q[freeSlot];
    t->valid = TRUE;
    t->con = &consoles[freeSlot];
    t->firstRun = TRUE;
    t->processNo = processNo;
    t->slot = freeSlot;
    breakpoint();
    currentTask = freeSlot;
    // end critical section
    
    kputs(itoa(currentTask,10), kernelConsole);
    switch(freeSlot){
        case 0:
            t->esp = (DWORD) &process1_stack_space;
            break;
        case 1:
            t->esp = (DWORD) &process2_stack_space;
            break;
        case 2:
            t->esp = (DWORD) &process3_stack_space;
            break;
    } 
 
    // since its done, we should enable interrupts
    enable_interrupts();

    switch(processNo){
        case 1:
            numeric(freeSlot);
            return;
   }
}

// sched: goes through the task queue and selects a task to queue

void sched(){
    kputs("scheduling...\n", kernelConsole);
    int i;
    

    kputs(itoa(currentTask,10), kernelConsole);
    kputs(itoa(nextTask,10), kernelConsole);

    // lock our environment
//    mutex_lock(&schedLock);

    nextTask = currentTask;
    
    if(currentTask == (MAX_TASKS - 1)){
        nextTask = 0;
    }

    for(i=nextTask+1; i<MAX_TASKS; i++){
        if(!task_q[i].valid)
            continue;

        nextTask = i; // 1. we've selected the next task. 2. proceed to schedule it. 3. ??? 4. profit! 
        break;        
    }

    kputs(itoa(currentTask,10), kernelConsole);
    kputs(itoa(nextTask,10), kernelConsole);
    // calling master chief // this job has been given to a kernel dispatcher
    //context_switch(currentTask, nextTask);

    // we're done! finish up (enable interrupts and release mutex)
//    mutex_unlock(&schedLock);
}

// kgetConsole: returns a pointer to the console glued with the process specified by the process ID

struct console * kgetConsole(u_int pid){
    return task_q[pid].con;
}

// kexit: called by an exitting process, marks the process's slot in the queue as free

void kexit(u_int pid){
    task_q[pid].valid = FALSE;
}

void sched_dispatcher(){
    
    //kputs(itoa(ct,10), kernelConsole);
    //kputs(itoa(nextTask,10), kernelConsole);

    if(nextTask == -1){
        //kputs("no task running!", kernelConsole);
        return;
    }

    if(currentTask == nextTask){
        //kputs("single task running!", kernelConsole);  
        return;
    }

    task * t = &task_q[nextTask];
    currentTask = nextTask;
    // enabling interrupts as we are now going to switch to the next Task
    enable_interrupts();

    switch(t->processNo){
        case 1:
            continue_numeric(t->slot);
            break;
    }
}
