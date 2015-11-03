
#include <kernel.h>

PORT_DEF port[MAX_PORTS];
PORT next_port;

//creates a port to the active process
PORT create_port()
{
  PORT newport = create_new_port(active_proc)
  return newport;
}


PORT create_new_port (PROCESS owner)
{
    //get next port and advance port reference
    PORT newport = next_port;
    next_port = next_port->next;

    //Port Data Structure
    newport->magic = MAGIC_PORT;
    newport->used = TRUE;
    newport->open = TRUE;
    newport->owner =  owner;
    newport->blocked_list_head = NULL;
    newport->blocked_list_tail = NULL;
    if(owner->first_port != NULL)
    {
        newport->next = owner->first_port;
    }
    else
    {
        newport->next = NULL;
    }
    owner->first_port = newport;
    
    return newport;
}


//Sets port to open
void open_port (PORT port)
{
    port->open = TRUE;
}

//Sets port to closed
void close_port (PORT port)
{
    port->open = FALSE;
}

//Sends data to specified port
void send (PORT dest_port, void* data)
{
    Process dest_process = dest_port->owner;
    //if (reciever is recieved blocked and port is open)
    if (dest_port->open && dest_process->state == STATE_RECEIVE_BLOCKED)
    {
        dest_process->param_proc = active_proc;
        dest_process->param_data = data;
        //Change Reciever to STATE_READY
        //taken care of by dispatch
        add_ready_queue (dest_process);
        //Change to STATE_REPLY_BLOCKED
        active_proc->state   = STATE_REPLY_BLOCKED;
    }
    //else
    else
    {
        active_proc->param_data = data;
        //Get on the send blocked list of the port
        if (dest_port->blocked_list_head == NULL)
            dest_port->blocked_list_head = active_proc;
        else
            dest_port->blocked_list_tail->next_blocked = active_proc;
        dest_port->blocked_list_tail = active_proc;
        active_proc->next_blocked = NULL;
        //change state to STATE_SEND_BLOCKED
        active_proc->state = STATE_SEND_BLOCKED;
    }
    active_proc->param_data = data;
    remove_ready_queue (active_proc);
    resign();
}

//Sends data to specified port
void message (PORT dest_port, void* data)
{
    Process dest_process = dest_port->owner;
    //if (receiver is receive blocked and port is open)
    if (dest_process->state == STATE_RECEIVE_BLOCKED && dest_port->open)
    {
        dest_process->param_proc  = active_proc;
        dest_process->param_data = data;
        //Change receiver to STATE_READY
        add_ready_queue (dest_process);
    }
    //else
    else
    {
        active_proc->param_data = data;
        //Get on the send blocked list of the port
        if (dest_port->blocked_list_head == NULL)
            dest_port->blocked_list_head = active_proc;
        else
            dest_port->blocked_list_tail->next_blocked = active_proc;
        dest_port->blocked_list_tail = active_proc;
        active_proc->next_blocked = NULL;
        remove_ready_queue (active_proc);
        //Change to STATE_MESSAGE_BLOCKED
        active_proc->state = STATE_MESSAGE_BLOCKED;
        active_proc->param_data = data;
    }
    resign();
}



void* receive (PROCESS* sender)
{
    PROCESS receiver_process;
    //Scanning send blocked list
    PORT p = active_proc->first_port;
    while (p != NULL) {
        if (p->open && p->blocked_list_head != NULL)
            // Found a process on the send blocked list
            break;
        p = p->next;
    }
    //if (send blocked list is not empty)
    if (p != NULL)
    {
        
        //sender = first process on the send blocked list
        receiver_process = p->blocked_list_head;
        *sender = receiver_process;
        
        //data = receiver_process->param_data;
        //cleanup pointer to next process
        p->blocked_list_head = p->blocked_list_head->next_blocked;
        if (p->blocked_list_head == NULL)
            p->blocked_list_tail = NULL;
        
        //if (sender is STATE_MESSAGE_BLOCKED)
        if (receiver_process->state == STATE_MESSAGE_BLOCKED)
        {
            //Change state of sender to STATE_READY
            add_ready_queue (deliver_proc);
            return receiver_process->param_data;
        }
        //if (sender is STATE_SEND_BLOCKED)
        else if (receiver_process->state == STATE_SEND_BLOCKED)
        {
            //Change state of sender to STATE_REPLY_BLOCKED
            receiver_process->state = STATE_REPLY_BLOCKED;
            return receiver_process->param_data;
        }
    }
    //else
    remove_ready_queue (active_proc);
    active_proc->param_data = NULL;
    active_proc->state = STATE_RECEIVE_BLOCKED;
    //Change to STATE_RECEIVED_BLOCKED
    resign();
    *sender = active_proc->param_proc;
    return active_proc->param_data;
}


void reply (PROCESS sender)
{
    //Add the process replied to back to the ready queue
    add_ready_queue (sender);
    resign();
    //resign()
}

//Initializes ipc
void init_ipc()
{
    int i;
    next_port = port;
    //initialize all ports
    for (i = 0; i< MAX_PORTS; i++)
    {
        port[i].magic = MAGIC_PORT;
        port[i].used = FALSE;
        //give reference to next port
        if (i != MAX_PORTS-1)
        {
            port[i].next = &port[i+1];
        }
        //if port is last in ipc then give it null value
        else
        {
            port[i].next = NULL;
        }
    }
}
