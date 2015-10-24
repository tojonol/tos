
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
  newport->magic = MAGIC_PORT
  newport->used = ;
  newport->open = ;
  newport->owner = ;
  newport->
}




void open_port (PORT port)
{
}



void close_port (PORT port)
{
}


void send (PORT dest_port, void* data)
{
}


void message (PORT dest_port, void* data)
{
}



void* receive (PROCESS* sender)
{
}


void reply (PROCESS sender)
{
}

//Initializes ipc
void init_ipc()
{

}
