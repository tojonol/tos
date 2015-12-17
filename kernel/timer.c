
#include <kernel.h>


PORT timer_port;


void sleep(int ticks)
{
    Timer_Message msg;
    msg.num_of_ticks = ticks;
    send(timer_port, &msg);
}

//process for the timer notifier slide 13
void timer_notifier_process (PROCESS self, PARAM param)
{
    while (1)
    {
    	wait_for_interrupt (TIMER_IRQ);
      //send message to timer process
    	message (timer_port, 0);
    }
}

//process for the timer
void timer_process (PROCESS self, PARAM param)
{
  int            ticks_left[MAX_PROCS];
  Timer_Message* msg;
  PROCESS        sender;
  int            i;

  //initialize the array containing the timers to zero
  for (i = 0; i < MAX_PROCS; i++)
    ticks_left[i] = 0;

  //create the timer notifier slide 9, 15
  create_process(timer_notifier_process, 7, 0, "Timer notifier");
  while(1)
  {
    //msg = receive();
    msg = (Timer_Message*) receive(&sender);

    //if (msg was sent from a client) {
    if (msg != NULL)
    {
      // register number of ticks client wants to sleep
      //continue
      i = sender - pcb;
      ticks_left[i] = msg->num_of_ticks;
    }
    else
    {
      // Message must have come from Timer Notifier
      //for (all clients doing a sleep) {
      for (i = 0; i < MAX_PROCS; i++)
      {
        if (ticks_left[i] == 1)
        {
          ticks_left[i] = 0;
          // Wake up client
          //reply to client;
          reply(&pcb[i]);
        }
        else if (ticks_left[i]>0)
        {
          //decrement tick counter;
          ticks_left[i]= ticks_left[i]-1;
        }
        //ignore if counter is at 0
        else
        {
          continue;
        }
      }
    }
  }
}

//Homework 8
void init_timer ()
{
  timer_port = create_process (timer_process, 6, 0, "Timer process");
  resign();
}
