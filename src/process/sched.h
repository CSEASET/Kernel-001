// we define our round robin scheduling algorithm here

extern void write_port(unsigned short port, unsigned char data);


// set pic frequency for interrupting
void timer_phase(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    write_port(0x43, 0x36);             /* Set our command byte 0x36 */
    write_port(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    write_port(0x40, divisor >> 8);     /* Set high byte of divisor */
}


/* This will keep track of how many ticks that the system
*  has been running for */
int timer_ticks = 0;

/* Handles the timer. In this case, it's very simple: We
*  increment the 'timer_ticks' variable every time the
*  timer fires. By default, the timer fires 18.222 times
*  per second. Why 18.222Hz? Some engineer at IBM must've
*  been smoking something funky */

void timer_handler_main(){
    /* Increment our 'tick count' */
    timer_ticks++;

        //kputs("One second has passed\n", kernelConsole);

    /* Every 18 clocks (approximately 1 second), we will
    *  display a message on the screen */
    if (timer_ticks % 100 == 0)
    {
        kputs("One second has passed\n", kernelConsole);
    }

    // send ACK to PIC, idiot!
    write_port(0x20, 0x20); 
}

/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
void sched_init(){
    // set timer frequency to 100 Hz 
    timer_phase(100);

   /* Installs 'timer_handler' to IRQ0 */
    //irq_install_handler(0, timer_handler); // idt_init handles that

}
