/****************************************************************************

NAME
	get_time

PURPOSE
	get the information about the CPU time elapsed for the execution
	of the current process. It is the only system dependant call
	used in my program and it can easily be changed to a dummy routine
	if no equivalent facility is avalaible on a different system.

SYNOPSIS
	get_time(utime,stime,ttime)
	float *utime, *stime, *ttime;

DESCRIPTION
	This function calls the getrusage function avalaible on UNIX 4.2 BSD
	to get the information about the cpu time elapsed in user mode and
	in system calls. The time elapsed in user mode is put in utime,
	the time elapsed in system calls is put in stime and the total of the
	two is placed in ttime.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	21 july 1984

AUTHOR
	Michel DAGENAIS

*************************************************************************/

#ifdef UNIX
#include <sys/time.h>
#include <sys/resource.h>

get_time(utime,stime,ttime)

float *utime,*stime,*ttime;
{
  struct rusage buffer;		/* struct to receive the information */

  (void)getrusage(0,&buffer);

  *utime = buffer.ru_utime.tv_sec + buffer.ru_utime.tv_usec / 1e6 ;
  *stime = buffer.ru_stime.tv_sec + buffer.ru_stime.tv_usec / 1e6 ;
  *ttime = *utime + *stime; 
  return(buffer.ru_maxrss);
}

#else
#include <time.h>

static int begin = 1;
static time_t begin_time;

long get_time()
{
  time_t elapsed_time;

  if(begin)
   { begin = 0;
     (void) time(&begin_time);
   }

  (void) time(&elapsed_time);
  return(elapsed_time - begin_time);
}
  
#endif
