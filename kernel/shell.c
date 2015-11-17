#include <kernel.h>

//slide 6
void shell_process(PROCESS self, PARAM param)
{
	while(1)
	{
		//read command from Keyboard

		//when user hits <enter> execute command
			//commands help
			//clear window
			//print processes
			//make train stop and go
			//change position of the switches
			//start pacman
	}
}

void init_shell()
{

	 WINDOW pacman_window=  {0, 0, 80, 25, 0, 0, ' '};
	 clear_window(&pacman_window);
	 //initialize pacman
	 init_pacman(&pacman_window, 3);//window, # ghosts
}
