
#include <kernel.h>

#include "disptable.c"


PROCESS active_proc;


/*
 * Ready queues for all eight priorities.
 */
PCB *ready_queue [MAX_READY_QUEUES];
int ready_processes[MAX_READY_QUEUES];

/*
 * add_ready_queue
 *----------------------------------------------------------------------------
 * The process pointed to by p is put the ready queue.
 * The appropiate ready queue is determined by p->priority.
 */
//ASSIGNMENT 3
void add_ready_queue (PROCESS proc)
{
    //if there is nothing in the ready queue at a given priority
    //add the process at that level and set pointers to self
    if (ready_queue[proc->priority] == NULL)
    {
	ready_queue[proc->priority] = proc;
	proc->next = proc;
	proc->prev = proc;
        ready_processes[proc->priority]=1;
    }
    //otherwise there is a process there
    //add the process before the existing process
    //increase the count of processes ready at that level
    else
    {
	proc->next = ready_queue[proc->priority];
	proc->prev = ready_queue[proc->priority]->prev;
	ready_queue[proc->priority]->prev->next = proc;
	ready_queue[proc->priority]->prev = proc;
        ready_processes[proc->priority]= 1+ ready_processes[proc->priority];
    }
}



/*
 * remove_ready_queue
 *----------------------------------------------------------------------------
 * The process pointed to by p is dequeued from the ready
 * queue.
 */
//ASSIGNMENT 3
void remove_ready_queue (PROCESS proc)
{
    //if it is the only item in the ready_queue
    //remove it
    if (proc->next == proc)
    {
	ready_queue[proc->priority] = NULL;
	ready_processes[proc->priority] = 0;
    }
    //otherwise we clean up the pointers removing the process
    else
    {
	ready_queue[proc->priority] = proc->next;
	proc->next->prev = proc->prev;
	proc->prev->next = proc->next;
    }
}

/*
 * dispatcher
 *----------------------------------------------------------------------------
 * Determines a new process to be dispatched. The process
 * with the highest priority is taken. Within one priority
 * level round robin is used.
 */
//ASSIGNMENT 3
PROCESS dispatcher()
{
    int i, j;
    //go through each process, if there is a process at that priority
    //and grab the highest priority queue's next process
    for(j = MAX_READY_QUEUES-1; j > -1; j--)
    {
    	if (ready_processes[j] >= 1 )
	{
       	    return active_proc->next;
	}
    }
}



/*
 * resign
 *----------------------------------------------------------------------------
 * The current process gives up the CPU voluntarily. The
 * next running process is determined via dispatcher().
 * The stack of the calling process is setup such that it
 * looks like an interrupt.
 */
//ASSIGNMENT 4
void resign()
{
	active_proc = dispatcher();
	//check to make sure we retrieved something
	check_active();
}

void check_active()
{
	assert(active_proc != NULL);
}

/*
 * init_dispatcher
 *----------------------------------------------------------------------------
 * Initializes the necessary data structures.
 */
//ASSIGNMENT 3
void init_dispatcher()
{
    int i;

    for (i = 0; i < MAX_READY_QUEUES; i++)
    {
	ready_queue [i] = NULL;
    	ready_processes[i] = 0;
    }
    add_ready_queue (active_proc);
}
