
extern void enable_interrupts(void);
extern void disable_interrupts(void);


typedef struct registers {
	DWORD eax, ebx, ecx, edx, esi, edi , ebp, eip, esp;
	WORD cs, ds, es, gs, fs, ss;
} regs;


typedef struct task_structure {
    regs r;
    console * con;
    int valid;
} task;



task task_q[3];
int currentTask, nextTask;

MUTEX schedLock;

// context_switch: switch currentTask's context with nextTask's context

void context_switch(int currentTask, int nextTask){
    if()
} 

// sched: goes through the task queue and selects a task to queue

void sched(){
    kputs("started scheduling...\n", kernelConsole);
    int i;
    
    disable_interrupts();

    // lock our environment
    mutex_lock(&schedLock);

    nextTask = currentTask + 1;
    
    if(currentTask == 2){
        nextTask = 0;
    }

    for(i=nextTask; i<3; i++){
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


