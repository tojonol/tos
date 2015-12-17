#include <kernel.h>

//creates a null process which runs idlely
//HW 9
void null_process (PROCESS self, PARAM param)
{
    while (1) ;
}

//initializes the null process
void init_null_process()
{
    create_process (null_process, 0, 0, "Null process");
}
