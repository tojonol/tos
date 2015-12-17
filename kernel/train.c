
#include <kernel.h>

static WINDOW train_wnd= {0, 0, 80, 11, 0, 0, ' '};
static WINDOW train_wnd;

//helperfunctions
void go()
{
	send_command("L20S4\015");
}
void stop()
{
	send_command("L20S0\015");
}
void reverse()
{
	send_command("L20D\015");
}

//check zamboni location
void check_zamboni()
{
	int counter = 0, result = 0;
	//check if config 3
	if (probe_switch(11))
	{
			//config3with zamboni
			if (probe_switch(6) || probe_switch(7))
			{
				wprintf(&train_wnd,"We've got a zamboni!  Waiting for it to pass\n");
				result = 1;
				init_switches_config3();
				send_command("L20S5\015");
				while(0==probe_switch(6))
				{
					wprintf(&train_wnd,".");
				}
				send_command("M4R\015");
				send_command("M3R\015");

				while(0==probe_switch(3))
				{
					wprintf(&train_wnd,".");
				}
				send_command("M1R\015");
				send_command("M2R\015");
				send_command("M7R\015");
				while(0==probe_switch(12))
				{
					wprintf(&train_wnd,".");
				}
				send_command("M1G\015");
				stop();
				while(0==probe_switch(14))
				{
					wprintf(&train_wnd,".");
				}

				send_command("L20S5\015");

				send_command("M1R\015");
				while(0==probe_switch(12))
				{
					wprintf(&train_wnd,".");
				}

				send_command("M1G\015");
				while(0==probe_switch(3))
				{
					wprintf(&train_wnd,".");
				}

				send_command("M1R\015");

				while(0==probe_switch(6))
				{
					wprintf(&train_wnd,".");
				}
				stop();

				reverse();

				send_command("M4R\015");

				send_command("M3R\015");
				go();
				while(0==probe_switch(5))
				{
					wprintf(&train_wnd,".");
				}
				stop();
				wprintf(&train_wnd,"Mission Accomplished\n");
				while(1);
			}

	}
	else
	{
		init_switches_default();
	}
}

//config 1 or 2 w/o zambonie
void configuration1()
{
	go();
	send_command("M4R\015");
	send_command("M3G\015");
	wprintf(&train_wnd,"Waiting for contact");
	while(0==probe_switch(1))
	{
		wprintf(&train_wnd,".");
	}
	stop();
	wprintf(&train_wnd,"Got the Wagon!\n");
	reverse();
	go();

	send_command("M5R\015");
	send_command("M6R\015");

	wprintf(&train_wnd,"Bringin' Home the bacon");
	while(0==probe_switch(8))
	{
		wprintf(&train_wnd,".");
	}
	stop();
	wprintf(&train_wnd,"We have arrived!\n");
}

//configuration 3 w/o zambonie
void configuration3()
{
	//go to 13
	go();
	send_command("M5G\015");
	wprintf(&train_wnd,"Going to 13");
	while(0==probe_switch(13))
	{
		wprintf(&train_wnd,".");
	}
	stop();
	reverse();
	send_command("M8R\015");

	//go to 12
	go();
	wprintf(&train_wnd,"Done!\nWaiting for contact");
	while(0==probe_switch(12))
	{
		wprintf(&train_wnd,".");
	}
	stop();

	wprintf(&train_wnd,"Got the Wagon!\n");
	reverse();
	send_command("M7G\015");
	go();

	send_command("M4R\015");
	send_command("M3R\015");

	wprintf(&train_wnd,"Bringin' Home the bacon");
	while(0==probe_switch(5))
	{
		wprintf(&train_wnd,".");
	}
	stop();
	wprintf(&train_wnd,"We have arrived!\n");
}
void configuration4()
{
	int initialcounter = 600, zamcounter = 15, zam =0;
	while(zamcounter>0)
	{
		if (probe_switch(13)==1)
		{

			wprintf(&train_wnd,"We have zam");
			zam=1;
		}
		zamcounter=zamcounter-1;

	}

	go();
	send_command("M9G\015");
	wprintf(&train_wnd,"Waiting for contact");
	while(0==probe_switch(14))
	{
		wprintf(&train_wnd,".");
	}
	stop();

	wprintf(&train_wnd,"What is it doing out on 16?\n");
	while(probe_switch(16))
	{
		if (initialcounter!=600)
		{
			wprintf(&train_wnd,"Drat, I'm going to have to try again\n");
		}
		wprintf(&train_wnd,"Lets try get it");
		go();
		initialcounter+=50;
		sleep(initialcounter);
		stop();
		reverse();
		go();
		while(!probe_switch(14))
		{
			wprintf(&train_wnd,".");
		}
		stop();
		if(probe_switch(16))
		{
			reverse();
		}
	}
	go();
	wprintf(&train_wnd,"Whaddya know, we got it!\n");
	send_command("M8G\015");
	send_command("M4R\015");
	send_command("M5R\015");
	send_command("M3R\015");

	wprintf(&train_wnd,"Bringin' Home the bacon");
	while(0==probe_switch(5))
	{
		wprintf(&train_wnd,".");
	}
	stop();
	wprintf(&train_wnd,"We have arrived!\n");
}

//initialize the zamboni in a small circle to the right
void init_switches_config3()
{
	send_command("M5G\015");
	send_command("M1G\015");
	send_command("M2R\015");
	send_command("M3G\015");
	send_command("M4G\015");
	send_command("M6G\015");
	send_command("M7R\015");
	send_command("M8G\015");
	send_command("M9R\015");
}

//initialize the zamboni in a small circle to the right
void init_switches_default()
{
	send_command("M1R\015");
	send_command("M2R\015");
	send_command("M3G\015");
	send_command("M4G\015");
	send_command("M5G\015");
	send_command("M6G\015");
	send_command("M7R\015");
	send_command("M8R\015");
	send_command("M9R\015");
}

//probe a specific switch
int probe_switch(int switchno)
{
	char cmd[10];
	send_command("R\015");
	switch (switchno)
	{
		case 1:
			send_command("C1\015");
			break;
		case 2:
			send_command("C2\015");
			break;
		case 3:
			send_command("C3\015");
			break;
		case 4:
			send_command("C4\015");
			break;
		case 5:
			send_command("C5\015");
			break;
		case 6:
			send_command("C6\015");
			break;
		case 7:
			send_command("C7\015");
			break;
		case 8:
			send_command("C8\015");
			break;
		case 9:
			send_command("C9\015");
			break;
		case 10:
			send_command("C10\015");
			break;
		case 11:
			send_command("C11\015");
			break;
		case 12:
			send_command("C12\015");
			break;
		case 13:
			send_command("C13\015");
			break;
		case 14:
			send_command("C14\015");
			break;
		case 15:
			send_command("C15\015");
			break;
		case 16:
			send_command("C16\015");
			break;
	}
	COM_Message msg;
	char* input;
	int len=3;

	msg.output_buffer = cmd;
 	msg.len_input_buffer = len;
 	msg.input_buffer = input;

 	sleep(14);
 	send(com_port,&msg);
 	if(msg.input_buffer[1]=='1')
		return 1;
 	else
		return 0;
}

//send comand to Train Simulator
void send_command(char* input)
{
	char temp[10];
	COM_Message msg;
	int len;

	msg.output_buffer = input;
  msg.len_input_buffer = len;
  msg.input_buffer = temp;
  sleep(14);
  send(com_port, &msg);
}

//process containing train movement
void train_process(PROCESS self, PARAM param)
{
  clear_window(&train_wnd);
	wprintf(&train_wnd,"Starting Train Process!\n");

	int zam = 0, temp = 0, configuration = 0;
	wprintf(&train_wnd,"Initializing Switches & Finding Zambonie\n");
	check_zamboni();
	wprintf(&train_wnd,"Checking Configuration\n");

	temp = probe_switch(2)+probe_switch(8);
	if (temp==2)
	{
		configuration = 1;
		wprintf(&train_wnd,"Configuration is 1 or 2\n");
	}
	else
	{
		temp = 0;
		temp = probe_switch(11)+probe_switch(5);
		if (temp==2)
		{
			wprintf(&train_wnd,"Configuration is 3\n");
			configuration = 3;
		}
		else
		{
			configuration = 4;
			wprintf(&train_wnd,"Configuration is 4\n");
		}
	}

	switch (configuration)
	{
		case 1:
			configuration1();
			break;
		case 3:
			configuration3();
			break;
		case 4:
			configuration4();
			break;

	}



	wprintf(&train_wnd,"Mission Accomplished\n");
	while(1);

}


void init_train(WINDOW* wnd)
{
	create_process(train_process, 4, 0, "Train Process");
}
