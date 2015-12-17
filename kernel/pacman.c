#include <kernel.h>

#define MAZE_WIDTH  19
#define MAZE_HEIGHT 16
#define GHOST_CHAR  0x02
#define MAX_GHOSTS 5

typedef struct {
    int x;
    int y;
    int used;
} GHOST;
GHOST ghost_array[MAX_GHOSTS];
int ghosttotal;
WINDOW* pacman_wnd;



char *maze[] = {
    "r--------T--------i",
    "|        |        |",
    "| ri r-i | r-i ri |",
    "| Ll L-l | L-l Ll |",
    "|                 |",
    "| -- | --T-- | -- |",
    "|    |   |   |    |",
    "E--- L--   --l ---e",
    "|        |        |",
    "| -i --- | --- r- |",
    "|  |           |  |",
    "E- | | --T-- | | -e",
    "|    |   |   |    |",
    "| ---t-- | --t--- |",
    "|                 |",
    "L-----------------l",
    NULL
};


void draw_maze_char(char maze_char)
{
    char ch = ' ';

    // For details of PC-ASCII characters see:
    // http://www.jimprice.com/jim-asc.shtml
    switch (maze_char) {
    case '|':
	ch = 0xB3;
	break;
    case '-':
	ch = 0xC4;
	break;
    case 'r':
	ch = 0xDA;
	break;
    case 'i':
	ch = 0xBF;
	break;
    case 'L':
	ch = 0xC0;
	break;
    case 'l':
	ch = 0xD9;
	break;
    case 'T':
	ch = 0xC2;
	break;
    case 't':
	ch = 0xC1;
	break;
    case 'E':
	ch = 0xC3;
	break;
    case 'e':
	ch = 0xB4;
	break;
    }
    output_char(pacman_wnd, ch);
}



void draw_maze()
{
    int x, y;

    clear_window(pacman_wnd);
    y = 0;
    while (maze[y] != NULL)
    {
      char* row = maze[y];
      x = 0;
      while (row[x] != '\0')
      {
        char ch = row[x];
        draw_maze_char(ch);
        x++;
      }
      y++;
    }
    wprintf(pacman_wnd, "Jonol's PacMan ");
}



// Pseudo random number generator
// http://cnx.org/content/m13572/latest/
int seed = 17489;
int last_random_number = 0;

int random()
{
    last_random_number = (25173 * last_random_number + 13849) % 65536;
    return last_random_number;
}

void init_ghost(int ghost)
{
    while (1)
    {
      	int x = random() % MAZE_WIDTH;
      	int y = random() % MAZE_HEIGHT;
      	if (maze[y][x] != ' ')
            continue;
      	ghost_array[ghost].x = x;
      	ghost_array[ghost].y = y;
      	break;
    }
}


void next_direction(int* dx, int* dy)
{
    *dx = 0;
    *dy = 0;
    int dir = random() % 4;
    switch (dir)
    {
      case 0:
        *dx=-1;
        break;
      case 1:
	      *dx=1;
        break;
      case 2:
        *dy=-1;
        break;
      case 3:
        *dy=1;
        break;
    }
}

BOOL move_ghost(int ghost, int dx, int dy)
{
    int prev_x = ghost_array[ghost].x;
    int prev_y = ghost_array[ghost].y;
    int new_x = prev_x + dx;
    int new_y = prev_y + dy;
    //if there is no open space
    if (maze[new_y][new_x] != ' ')
    {
      /*wprintf(pacman_wnd, ">>");
      wprintf(pacman_wnd, maze[new_y][new_x]);
      wprintf(pacman_wnd, "<<");*/
      return FALSE;
    }
    remove_cursor(pacman_wnd);
    move_cursor(pacman_wnd, prev_x, prev_y);
    move_cursor(pacman_wnd, new_x, new_y);
    show_cursor(pacman_wnd);
    ghost_array[ghost].x = new_x;
    ghost_array[ghost].y = new_y;
    return TRUE;
}

void create_new_ghost(PROCESS self, PARAM param)
{
    PORT outport = (PORT) param;

    int dx, dy;
    int data = 3;

    //init_ghost(&ghost);
    while (1)
    {
      next_direction(&dx, &dy);
      while (move_ghost(self->name, dx, dy) == FALSE)
      {
        next_direction(&dx, &dy);

        //wprintf(pacman_wnd, "send");
        //wprintf(pacman_wnd, self->name);
      }
      //message(outport, data);
      resign();
    }
}

void ghost_process(PROCESS self, PARAM param)
{
    create_new_ghost(self, param);
}

void create_main_process(PROCESS self, PORT inport)
{
    int *data1, counter;
    PROCESS ghost;
    resign();
    while(1)
    {
      //wprintf(pacman_wnd, "waiting");
      data1 = (int*) receive(&ghost);

      resign();
      //reply(&ghost);

      //wprintf(pacman_wnd, self->name);
    }
}

void main_process(PROCESS self, PARAM param)
{

  //  wprintf(pacman_wnd, "MAINP ");
    create_main_process(self, (PORT)param);
}

void init_pacman(WINDOW* wnd, int num_ghosts)
{
    PORT comport = NULL;
    int j;
    ghosttotal = num_ghosts;

    for(j=0;j<MAX_GHOSTS;j++)
    {
      init_ghost(j);
      if(j>=ghosttotal)
        ghost_array[j].used = 0;
      else
        ghost_array[j].used = 1;
    }

    pacman_wnd = wnd;
    pacman_wnd->width = MAZE_WIDTH;
    pacman_wnd->height = MAZE_HEIGHT + 1;
    pacman_wnd->cursor_char = GHOST_CHAR;

    draw_maze();
    //comport = create_process(main_process, 3, 0, "Main");


    for (j = 0; j < num_ghosts; j++)
    {
        create_process(ghost_process, 4, comport, j);
    }
    resign();
}
