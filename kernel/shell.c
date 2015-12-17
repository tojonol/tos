#include <kernel.h>
static const int MAX_BUFFER = 80;
static const WINDOW shell_wnd=  {0, 11, 80, 14, 0, 0, ' '};
static WINDOW train_wnd= {0, 0, 80, 11, 0, 0, ' '};

void checkswitch(char* command)
{
	if (0 == k_memcmp(command,"switchR", 7))
	{
		if (command[7] == '1')
			send_command("M1R\015");
		else if (command[7] == '2')
			send_command("M2R\015");
		else if (command[7] == '3')
			send_command("M3R\015");
		else if (command[7] == '4')
			send_command("M4R\015");
		else if (command[7] == '5')
			send_command("M5R\015");
		else if (command[7] == '6')
			send_command("M6R\015");
		else if (command[7] == '7')
			send_command("M7R\015");
		else if (command[7] == '8')
			send_command("M8R\015");
		else if (command[7] == '9')
			send_command("M9R\015");
	}
	else if (0 == k_memcmp(command,"switchG", 7))
	{
		if (command[7] == '1')
			send_command("M1G\015");
		else if (command[7] == '2')
			send_command("M2G\015");
		else if (command[7] == '3')
			send_command("M3G\015");
		else if (command[7] == '4')
			send_command("M4G\015");
		else if (command[7] == '5')
			send_command("M5G\015");
		else if (command[7] == '6')
			send_command("M6G\015");
		else if (command[7] == '7')
			send_command("M7G\015");
		else if (command[7] == '8')
			send_command("M8G\015");
		else if (command[7] == '9')
			send_command("M9G\015");
	}
}

void displayHelpMenu()
{
	wprintf(&shell_wnd, "TOS Help Menu\n");
	wprintf(&shell_wnd, "help...............display help options \n");
	wprintf(&shell_wnd, "clear..............clear the screen\n");
	wprintf(&shell_wnd, "process............print current processes\n");
	wprintf(&shell_wnd, "train..............launch the train\n");
	wprintf(&shell_wnd, "reverse............reverse direction of the train\n");
	wprintf(&shell_wnd, "go.................move the train\n");
	wprintf(&shell_wnd, "stop...............stop the train\n");
	wprintf(&shell_wnd, "switchG/R#.........change the position of switches [ex switchG2]\n");
	wprintf(&shell_wnd, "pacman.............launch the pacman game [not implemented :()]\n");
}
void displayNotYet()
{
	wprintf(&shell_wnd, "This option is not yet enabled\n");
	wprintf(&shell_wnd, "Please try another command (need 'help'?)");
}


void execute(char* command)
{
	int strlen = k_strlen(command);
	//slide 7
	//commands help
	if(0 == k_memcmp(command,"help", strlen))
	{
		displayHelpMenu();
	}
	//reverse
	else if (0 == k_memcmp(command,"reverse", strlen))
	{
		send_command("L20D\015");
	}
	//clear window
	else if (0 == k_memcmp(command,"clear", strlen))
	{
		int counter = 10000;
		while (counter>0)
		{
			wprintf(&shell_wnd, " ");
			counter= counter-1;
		}
		clear_window(&shell_wnd);
		clear_window(&train_wnd);
	}
	//print processes
	else if (0 == k_memcmp(command,"process", strlen))
	{
		print_all_processes(&shell_wnd);
	}
	//make train stop
	else if (0 == k_memcmp(command,"stop", strlen))
	{
		send_command("L20S0\015");
	}
	//init train
	else if (0 == k_memcmp(command,"train", strlen))
	{
		init_train(&shell_wnd);
	}
	//make train go
	else if (0 == k_memcmp(command,"go", strlen))
	{
		send_command("L20S4\015");
	}
	//change position of the switches
	else if (0 == k_memcmp(command,"switch", 6))
	{
		checkswitch(command);
	}
	//start pacman
	else if (0 == k_memcmp(command,"pacman", strlen))
	{
		displayNotYet();
	}
}

//slide 6
void shell_process(PROCESS self, PARAM param)
{
	char cmdBuffer[MAX_BUFFER];
	char ch;
 	Keyb_Message msg;
	int i;

 	clear_window(&shell_wnd);
	wprintf(&shell_wnd,"TOS SHELL\n");
	output_string(&shell_wnd,"@-:");
	while(1)
	{
		//read command from Keyboard
		msg.key_buffer = &ch;
		send(keyb_port, &msg);

		switch(ch)
		{
				//backspace
				case '\b':
					if (i == 0)
						continue;
					i--;
					reverse_cursor(&shell_wnd);
					show_cursor(&shell_wnd);
					break;
				//return
				case 13:
			  	//when user hits <enter> execute command
					cmdBuffer[i]='\0';
					wprintf(&shell_wnd, "\n");
					execute(&cmdBuffer[0]);
					//RESET buffer
					i=0;
					output_string(&shell_wnd,"@-:");
					break;

				//any other key
				default:
					cmdBuffer[i] = ch;
					if(i<MAX_BUFFER-1)
					{
						i++;
						wprintf(&shell_wnd, "%c", ch);
					}
					break;
			}
	}
}

void init_shell()
{
	int counter = 10000;
	while (counter>0)
	{
		wprintf(&shell_wnd, " ");
		counter= counter-1;
	}
	clear_window(&train_wnd);
	clear_window(&shell_wnd);
	//initialize pacman
	create_process(shell_process,3,0,"shell");
	//	 init_pacman(&shell_wnd, 7);//window, # ghosts
}
