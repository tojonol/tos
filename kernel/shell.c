#include <kernel.h>
static WINDOW pacman_window=  {0, 0, 80, 25, 0, 0, ' '};



void init_shell()
{
	//initialize pacman
	init_pacman(&pacman_window, 5);//window, # ghosts
}
