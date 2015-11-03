
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


void send (PORT dest_port, void* data)
{
    //if (reciever is recieved blocked and port is open)
        //Change Reciever to STATE_READY
        //Change to STATE_REPLY_BLOCKED
    //else
        //Get on the send blocked list of the port
        //change state to STATE_SEND_BLOCKED
    
}


void message (PORT dest_port, void* data)
{
    //if (receiver is receive blocked and port is open)
        //Change receiver to STATE_READY
    //else
        //Get on the send blocked list of the port
        //Change to STATE_MESSAGE_BLOCKED
}



void* receive (PROCESS* sender)
{
    //Scanning send blocked list
    PORT p = active_proc->first_port;
    while (p != NULL) {
        if (p->open && p->blocked_list_head != NULL)
            // Found a process on the send blocked list
            p = p->next;
    }
    //if (send blocked list is not empty)
        //sender = first process on the send blocked list
        //if (sender is STATE_MESSAGE_BLOCKED)
            //Change state of sender to STATE_READY
        //if (sender is STATE_SEND_BLOCKED)
            //Change state of sender to STATE_REPLY_BLOCKED
    //else
        //Change to STATE_RECEIVED_BLOCKED
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
        port[i].user = FALSE;
        //give reference to next port
        if (i != MAX_PORTS-1)
            port[i].next = &port[i+1];
        //if port is last in ipc then give it null value
        else
            port[i].next = NULL;
    }
}
