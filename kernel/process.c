#include <kernel.h>

PCB pcb[MAX_PROCS];
PCB *next_proc_ptr;

//helper method for maintaining the stack
MEM_ADDR pushStack(MEM_ADDR esp, LONG ptr)
{
    esp = esp - 4;
    poke_l (esp, ptr);
    return esp;
}

//HOMEWORK 3
PORT create_process (void (*ptr_to_new_proc) (PROCESS, PARAM),
		     int prio,
		     PARAM param,
		     char *name)
{

    //initialize the process values
    LONG default_value = 0;
    MEM_ADDR     esp;
    PROCESS      new_proc;
    PORT         new_port;

    //HW7
    volatile int flag;//disable interrupts
    DISABLE_INTR(flag);

    new_proc = next_proc_ptr;
    next_proc_ptr = new_proc->next;

    //HW7
    ENABLE_INTR (flag);//reenable interrupts
    new_proc->magic             = MAGIC_PCB;
    new_proc->used              = TRUE;
    new_proc->state             = STATE_READY;
    new_proc->priority          = prio;
    new_proc->first_port        = NULL;
    new_proc->name              = name;

    new_port = create_new_port(new_proc);
    esp = 640 * 1024 - (new_proc - pcb) * 16 * 1024;

    //PARAM
    esp = pushStack(esp, (LONG)param);
    //PROCESS
    esp = pushStack(esp, (LONG)new_proc);
    //RETURN ADDRESS
    esp = pushStack(esp, default_value);

    //Push interrupt flags if initialized
    //HW7 - slide 20 EFLAGS
    if (interrupts_initialized)
    {
	     esp = pushStack(esp, 512);
    }
    else
    {
	     esp = pushStack(esp, default_value);
    }
    //Push CodeSelector
    esp = pushStack(esp, CODE_SELECTOR);


    //ADDRESS OF NEW PROCESS
    esp = pushStack(esp, (LONG)ptr_to_new_proc);
    //EAX
    esp = pushStack(esp, default_value);
    //ECX
    esp = pushStack(esp, default_value);
    //EDX
    esp = pushStack(esp, default_value);
    //EBX
    esp = pushStack(esp, default_value);
    //EBP
    esp = pushStack(esp, default_value);
    //ESI
    esp = pushStack(esp, default_value);
    //EDI
    esp = pushStack(esp, default_value);
    new_proc->esp = esp;

    add_ready_queue (new_proc);
    return new_port;
}

//untouched
PROCESS fork()
{
    // Dummy return to make gcc happy
    return (PROCESS) NULL;
}

//HOMEWORK 3
void print_process(WINDOW* wnd, PROCESS p)
{
    wprintf(wnd, "State      Active  Prio  Name\n");
    wprintf(wnd, "-----------------------------------------------\n");

    if (p == active_proc)
        wprintf(wnd, "READY      *     %d %s\n", p->priority,p->name);
    else
        wprintf(wnd, "READY            %d %s\n", p->priority,p->name);
}

//HOMEWORK 3
void print_all_processes(WINDOW* wnd)
{
    int i;
    PCB* p = pcb;
    wprintf(wnd, "State      Active  Prio  Name\n");
    wprintf(wnd, "-----------------------------------------------\n");
    for (i = 0; i < MAX_PROCS; i++)
    {
	if (p->used)
	{
	     if (p == active_proc)
        	wprintf(wnd, "READY      *     %d %s\n", p->priority,p->name);
    	     else
        	wprintf(wnd, "READY            %d %s\n", p->priority,p->name);
	     p++;
	}
    }
}

//HOMEWORK 3
void init_process()
{
    int i;
    //initialize the boot process
    pcb[0].magic = MAGIC_PCB;
    pcb[0].used = TRUE;
    pcb[0].state = STATE_READY;
    pcb[0].priority = 1;
    pcb[0].first_port = NULL;
    pcb[0].name = "Boot process";

    for (i = 1; i < MAX_PROCS; i++)
    {
	     pcb [i].magic = 0;
    	pcb [i].used = FALSE;
    	if (i == MAX_PROCS-1)
    	    pcb [i].next = NULL;
    	else
    	    pcb [i].next = &pcb [i+1];
    }

    next_proc_ptr = &pcb [1];
    active_proc = pcb;
}
