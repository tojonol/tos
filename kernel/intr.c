#include <kernel.h>

BOOL interrupts_initialized = FALSE;

IDT idt [MAX_INTERRUPTS];
PROCESS interrupt_table [MAX_INTERRUPTS];
PROCESS p;


void load_idt (IDT* base)
{
    unsigned short           limit;
    volatile unsigned char   mem48 [6];
    volatile unsigned       *base_ptr;
    volatile short unsigned *limit_ptr;

    limit      = MAX_INTERRUPTS * IDT_ENTRY_SIZE - 1;
    base_ptr   = (unsigned *) &mem48[2];
    limit_ptr  = (short unsigned *) &mem48[0];
    *base_ptr  = (unsigned) base;
    *limit_ptr = limit;
    asm ("lidt %0" : "=m" (mem48));
}

//Homework 6
void init_idt_entry (int intr_no, void (*isr) (void))
{
  //initialize slide 20
  idt[intr_no].offset_0_15  = (unsigned) isr & 0xffff;
  idt[intr_no].offset_16_31 = ((unsigned) isr >> 16) & 0xffff;
  idt[intr_no].unused       = 3;
  idt[intr_no].p            = 1;
  idt[intr_no].dpl          = 0;
  idt[intr_no].dt           = 0;
  idt[intr_no].type         = 0xe;
  idt[intr_no].dword_count  = 0;
  idt[intr_no].selector     = CODE_SELECTOR;
}

/*
 * Timer ISR
 */
void isr_timer ();
void dummy_isr_timer ()
{
    /*
     *	PUSHL	%EAX		; Save process' context
     *  PUSHL   %ECX
     *  PUSHL   %EDX
     *  PUSHL   %EBX
     *  PUSHL   %EBP
     *  PUSHL   %ESI
     *  PUSHL   %EDI
     */
    asm ("isr_timer:"); //DEFINE TIMER
    asm ("pushl %eax;pushl %ecx;pushl %edx");
    asm ("pushl %ebx;pushl %ebp;pushl %esi;pushl %edi");

    /* Save the context pointer ESP to the PCB */
    asm ("movl %%esp,%0" : "=m" (active_proc->esp) : );

    p = interrupt_table[TIMER_IRQ];

    /* Add event handler to ready queue */
    if (p && p->state == STATE_INTR_BLOCKED)
    {
      add_ready_queue (p);
    }
    active_proc = dispatcher();

    /* Restore context pointer ESP */
    asm ("movl %0,%%esp" : : "m" (active_proc->esp) );

    /*
     *	MOVB  $0x20,%AL	; Reset interrupt controller
     *	OUTB  %AL,$0x20
     *	POPL  %EDI      ; Restore previously saved context
     *  POPL  %ESI
     *  POPL  %EBP
     *  POPL  %EBX
     *  POPL  %EDX
     *  POPL  %ECX
     *  POPL  %EAX
     *	IRET		; Return to new process
     */
    asm ("movb $0x20,%al;outb %al,$0x20");
    asm ("popl %edi;popl %esi;popl %ebp;popl %ebx");
    asm ("popl %edx;popl %ecx;popl %eax");
    asm ("iret");
}



/*
 * COM1 ISR
 */
void isr_com1 ();
void dummy_isr_com1 ()
{
    /*
     *	PUSHL	%EAX		; Save process' context
     *  PUSHL   %ECX
     *  PUSHL   %EDX
     *  PUSHL   %EBX
     *  PUSHL   %EBP
     *  PUSHL   %ESI
     *  PUSHL   %EDI
     */
    asm ("isr_com1:");
    asm ("pushl %eax;pushl %ecx;pushl %edx");
    asm ("pushl %ebx;pushl %ebp;pushl %esi;pushl %edi");
    p = interrupt_table[COM1_IRQ];
    /* Save the context pointer ESP to the PCB */
    asm ("movl %%esp,%0" : "=m" (active_proc->esp) : );

    /* Add event handler to ready queue */
    add_ready_queue (p);

    active_proc = dispatcher();

    /* Restore context pointer ESP */
    asm ("movl %0,%%esp" : : "m" (active_proc->esp) );

    /*
     *	MOVB  $0x20,%AL	; Reset interrupt controller
     *	OUTB  %AL,$0x20
     *	POPL  %EDI      ; Restore previously saved context
     *  POPL  %ESI
     *  POPL  %EBP
     *  POPL  %EBX
     *  POPL  %EDX
     *  POPL  %ECX
     *  POPL  %EAX
     *	IRET		; Return to new process
     */
    asm ("movb $0x20,%al;outb %al,$0x20");
    asm ("popl %edi;popl %esi;popl %ebp;popl %ebx");
    asm ("popl %edx;popl %ecx;popl %eax");
    asm ("iret");

}


/*
 * Keyboard ISR
 */
void isr_keyb();
void dummy_isr_keyb()
{
    /*
     *	PUSHL	%EAX		; Save process' context
     *  PUSHL   %ECX
     *  PUSHL   %EDX
     *  PUSHL   %EBX
     *  PUSHL   %EBP
     *  PUSHL   %ESI
     *  PUSHL   %EDI
     */
    asm ("isr_keyb:");
    asm ("pushl %eax;pushl %ecx;pushl %edx");
    asm ("pushl %ebx;pushl %ebp;pushl %esi;pushl %edi");

    /* Save the context pointer ESP to the PCB */
    asm ("movl %%esp,%0" : "=m" (active_proc->esp) : );

    p = interrupt_table[KEYB_IRQ];

    if (p == NULL) {
	panic ("service_intr_0x61: Spurious interrupt");
    }

    if (p->state != STATE_INTR_BLOCKED) {
	panic ("service_intr_0x61: No process waiting");
    }

    /* Add event handler to ready queue */
    add_ready_queue (p);

    active_proc = dispatcher();

    /* Restore context pointer ESP */
    asm ("movl %0,%%esp" : : "m" (active_proc->esp) );

    /*
     *	MOVB  $0x20,%AL	; Reset interrupt controller
     *	OUTB  %AL,$0x20
     *	POPL  %EDI      ; Restore previously saved context
     *  POPL  %ESI
     *  POPL  %EBP
     *  POPL  %EBX
     *  POPL  %EDX
     *  POPL  %ECX
     *  POPL  %EAX
     *	IRET		; Return to new process
     */
    asm ("movb $0x20,%al;outb %al,$0x20");
    asm ("popl %edi;popl %esi;popl %ebp;popl %ebx");
    asm ("popl %edx;popl %ecx;popl %eax");
    asm ("iret");
}

//HOMEWORK 8
void wait_for_interrupt (int intr_no)
{
  volatile int flag;
  DISABLE_INTR (flag);

  //save process in interrupt tabale
  interrupt_table [intr_no] = active_proc;
  remove_ready_queue (active_proc);
  active_proc->state = STATE_INTR_BLOCKED;
  resign();

  //upon returning remove the process
  interrupt_table [intr_no] = NULL;
  ENABLE_INTR (flag);
}


void delay ()
{
    asm ("nop;nop;nop");
}

void re_program_interrupt_controller ()
{
    /* Shift IRQ Vectors so they don't collide with the
       x86 generated IRQs */

    // Send initialization sequence to 8259A-1
    asm ("movb $0x11,%al;outb %al,$0x20;call delay");
    // Send initialization sequence to 8259A-2
    asm ("movb $0x11,%al;outb %al,$0xA0;call delay");
    // IRQ base for 8259A-1 is 0x60
    asm ("movb $0x60,%al;outb %al,$0x21;call delay");
    // IRQ base for 8259A-2 is 0x68
    asm ("movb $0x68,%al;outb %al,$0xA1;call delay");
    // 8259A-1 is the master
    asm ("movb $0x04,%al;outb %al,$0x21;call delay");
    // 8259A-2 is the slave
    asm ("movb $0x02,%al;outb %al,$0xA1;call delay");
    // 8086 mode for 8259A-1
    asm ("movb $0x01,%al;outb %al,$0x21;call delay");
    // 8086 mode for 8259A-2
    asm ("movb $0x01,%al;outb %al,$0xA1;call delay");
    // Don't mask IRQ for 8259A-1
    asm ("movb $0x00,%al;outb %al,$0x21;call delay");
    // Don't mask IRQ for 8259A-2
    asm ("movb $0x00,%al;outb %al,$0xA1;call delay");
}

/*
 * dummy_isr
 */
 void default_isr();
 void dummy_default_isr()
 {
   asm ("default_isr:");
   asm ("push %eax; push %ecx; push %edx");
   asm ("push %ebx; push %ebp; push %esi; push %edi");
   asm ("movb $0x20,%al");
   asm ("outb %al,$0x20");
   asm ("pop %edi; pop %esi; pop %ebp; pop %ebx");
   asm ("pop %edx; pop %ecx; pop %eax");
   asm("iret");
 }

void isr_error();
void dummy_isr_error()
{
  error();
}
void error()
{
  WINDOW error_window = {10, 12, 80, 1, 0, 0, ' '};
  wprintf (&error_window, "Fatal exception (%s)", active_proc->name);
  //infinite loop
  while (1) ;
}



//Homework 6
void init_interrupts()
{
  int i;

  load_idt (idt);
  for (i = 0; i < MAX_INTERRUPTS; i++)
  {
    init_idt_entry (i, dummy_default_isr);
    if (i < 16)
      init_idt_entry (i, dummy_isr_error);
  }

  //initialize timer, com, keyboard interrupt
  init_idt_entry (TIMER_IRQ, isr_timer);
  init_idt_entry (COM1_IRQ, isr_com1);
  init_idt_entry (KEYB_IRQ,isr_keyb);
  re_program_interrupt_controller();
  for (i = 0; i < MAX_INTERRUPTS; i++)
 	  interrupt_table [i] = NULL;
  interrupts_initialized = TRUE;
  asm ("sti");
}
