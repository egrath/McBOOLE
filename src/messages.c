/***************************************************************************

NAME
	fatal_user_error, fatal_system_error, fatal_program_error,
	warning_user_error, send_user_message, send_file_message,
	send_user_dtime, send_file_dtime, send_user_etime, send_file_etime 

PURPOSE
	These functions send a string to the user or to the output file
	with the appropriate format. The different functions have different
	destinations for their string, different format and different action
	to take.

SYNOPSIS

	fatal_user_error(string)
	fatal_system_error(string)
	fatal_program_error(string)

	warning_user_error(string)

	send_user_message(string)
	send_file_message(string)

	send_user_dtime(string)
	send_file_dtime(string)

	send_user_etime(string)
	send_file_etime(string)

	char *string;

DESCRIPTION

    -fatal_user_error is called when the user is responsible for an
	unrecoverable situation. The string is printed on the stderr and
	the program stops. Usually the error is in the input data.

    -fatal_system_error is called when the program detects an error from the
	system. The string is printed on the stderr. Usually the error is
	encountered when the program was not able to allocate memory or
	if some operations required by the program cannot be performed.

    -fatal_program_error is called when the program detects itself that
	something is going wrong. At many places in the program, if some
	variable is known to have only a restricted set of valid values,
	some test might be performed to check that everything performs
	as it should be. The string is printed on stderr. Usually this
	occurs when the memory occupied by the program was messed for
	some reason or when some bugs were left in the program. When
	this occurs insure that the fault is in the program and not in
	the system and then contact the author with the exact circumstances
	during which the message occured. Hopefully the validation of
	the program was extensive enough and this function should never
	be useful.

    -warning_user_error is called when the user caused a recoverable error
	or when a strange but not impossible request was issued by the user.
	The string is printed on stderr but the execution continues.
 
    -send_user_message prints the string on the stdout. When the output file
	is stdout, this function calls send_file_message to insure that
	this message will be in a format acceptable for the output file.
	The messages for the user are printed normally as the messages for
	the file are put in comment to insure that the file will be correctly
	processed by the other programs that will have to read the file.

    -send_file_message prints the string as a comment on the output file.

    -send_user_dtime prints the CPU time elapsed since the last call to 
	this function to the stdout. The system and user time are printed.
	The string that describes what the time represents is also printed by
	this function and all the functions similar to this one.

    -send_file_dtime prints the CPU time elapsed as a comment on the output
	file.

    -send_user_etime prints the total CPU time elapsed on the stdout.

    -send_file_etime prints the total CPU time elapsed as a comment on the
	output file.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

#include <stdio.h>
#include <stdlib.h>

extern FILE *output_file;

fatal_user_error(string)

char *string;
{
  fprintf(stderr,"McBOOLE fatal user error : \n** ");
  fprintf(stderr,string);
  fprintf(stderr," **\n");
  send_user_etime("Crash status"); 
  exit(1);
}

/**************************************************************************/

fatal_system_error(string)

char *string;
{
  fprintf(stderr,
      "McBOOLE fatal system error (some resource is not avalaible) : \n** ");
  fprintf(stderr,string);
  fprintf(stderr," **\n");
  send_user_etime("Crash status");
  exit(1);
}

/*************************************************************************/

fatal_program_error(string)

char *string;
{
  fprintf(stderr,
  "McBOOLE fatal error, an assertion of the program is not fulfilled.\n"); 
  fprintf(stderr,"This should never happen, insure that no external factor\n");
  fprintf(stderr,"is involved and contact the author at McGill University\n");
  fprintf(stderr,"to report the problem.\n\n** ");
  fprintf(stderr,string);
  fprintf(stderr," **\n");
  send_user_etime("Crash status");
  exit(1);
}

/***************************************************************************/

warning_user_error(string)

char *string;
{
  fprintf(stderr," McBOOLE warning : \n** ");
  fprintf(stderr,string);
  fprintf(stderr," **\n");
  return ;
}

/**************************************************************************/

send_user_message(string)

char *string;
{
  if(output_file == stdout) send_file_message(string);
  else fprintf(stdout,"McBOOLE : %s\n\n",string);
}

/*************************************************************************/

send_file_message(string)

char *string; 
{
  fprintf(output_file,"/*\n\nMcBOOLE : %s\n\n*/\n",string);
}

/*************************************************************************/

long get_time();

send_user_dtime(string)

char *string; 
{
  static long
	utime,			/* time elapsed in user mode */
	old_utime = 0;		/* time elapsed in user mode */

  if(output_file == stdout)send_file_dtime(string);
  else
   { utime = get_time();
     fprintf(stdout,
	     "McBOOLE : CPU time %lds, %s\n\n",utime - old_utime,string);
     old_utime = utime;
   }
}

/***********************************************************************/

send_file_dtime(string)

char *string;
{
  static long
	utime,			/* time elapsed in user mode */
	old_utime = 0;		/* time elapsed in user mode */

  utime = get_time();
  fprintf(output_file,
	   "/*\n\nMcBOOLE : CPU time %lds, %s\n*/\n",utime - old_utime,string);
  
  old_utime = utime;
}

/*************************************************************************/

send_user_etime(string)

char *string;
{
  long
	utime;			/* time elapsed in user mode */

  if(output_file == stdout)send_file_etime(string);

  else
   { utime = get_time();
 
     fprintf(stdout,
	     "McBOOLE : total CPU time %lds, %s\n\n",utime,string);
   }
}

/*************************************************************************/

send_file_etime(string)

char *string;
{
  long
	utime;			/* time elapsed in user mode */

  utime = get_time();

  fprintf(output_file,
          "/*\n\nMcBOOLE : total CPU time %ld, %s\n*/\n",utime,string);
}

