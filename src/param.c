/***********************************************************************

NAME 
	param

PURPOSE
	This function is very useful and should be used by every program,
	it does the dirty interface with the user, checking everything he
	inputs, opening files, setting switches and so on.

SYNOPSIS
	param(parmc,parmv,argc,argv)
	struct parameter parmv[];
	char ** argv;
	int parmc,argc;

DESCRIPTION
	The user places in parmv the name and type of all the switches
	that the program is to recognize. For each of them he gives detailed
	information about the values expected and where to place the value
	read, which parameter is mandatory and which is optional. To know how
	to set this information you should look to the structures defined in
	param.h and also to some program already using this function. 

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	2 july 1984

AUTHOR
	Michel DAGENAIS

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "param.h"
#define ASK 1
#define NOASK 0
#define TRUE 1
#define FALSE 0

double atof();			/* convert float string in float binary */
char *copy_and_alloc_string();	/* copy a string into new storage */
char *sfind_next_car();		/* finds a specified character in a string */

static char *types[] =		/* description of the types */
	{ "file name",
	  "name for two files",
	  "integer value",
	  "float value",
	  "character string",
	  "single character",
	  "logical value" };

static char *options[] = 	/* description of the options */
	{ "is mandatory",
	  "was entered",
	  "is taken by default when no switch is present"};

static char *prerequisite[] =  /* description of the different links */
	{ "when present requires",
	  "when present excludes",
	  "when absent requires",
	  "when absent excludes"};

param(parmc,parmv,argc,argv)

struct parameter parmv[];
char **argv;
int parmc,argc;

/*
	each possible parameter should be in the list sent to the function;
	anything else will not be accepted.

	Each switch has a name which can be more then one character long;
        When we try to match the names, the list is scanned top to bottom
        so the longest names should be placed in the list before the shorter 
	ones begining by the same letters. For example "slow" should be placed
	in the list before "s" otherwise it will never be recognized. Beside 
	the name we have the type of parameter which tells what action is to be
	taken. Also a flag tells if the parameter is needed and if it has some  
	prerequisites or allergy. It contains also the 
	information if the parameter was received or not. We check that all
	needed parameters are received, that all parameters received that 
	having pre-requisite parameters get them satisfied. Also we check
	that no two parameters which are mutually exclusive are received.
									*/


{  int 
	flag,		/* temp storage for the flag of the parameter */
	reqa,		/* logic value set if a is required */
	reqb,		/* logic value set if b is required */
	reja,		/* logic value set if a is rejected */
	rejb,		/* logic value set if b is rejected */
	i,		/* loop counter for the argv table */
	j,		/* loop for the parmv table  */
	k,		/* loop for the options table */
	indirect,	/* index of a parameter referred by another one */
	satisfied,	/* logic value tells if all pre-requisite were satis*/
	length;		/* length of a character string */

   char *cursor;	/* pointer where we process in the command line */

/* This first loop is to process all the parameters entered directly on the
   command line by the user. We scan the whole argv table		*/

   for(i = 1 ; i < argc ; i++)
    { if(*(argv[i]) != '-')
       { for(j = 0 ; j < parmc ; j++)
          { if((parmv[j].flag & P_DEFAULT) && (parmv[j].flag & P_PRESENT) == 0)
	     								break;
	  }

/* There is no - before the switch and no parameter 'by default' was expected */ 
	 if(j >= parmc)
          { fprintf(stderr,
            "%s: switches should be preeceded by '-', unexpected %s\n",
            argv[0],argv[i]);
            exit(1);
          }

/* It is a parameter taken 'by default' with no switch preeceding it */

	 parmv[j].flag |= P_PRESENT;
	 i--;
	 set_value(parmv + j,&i,argc,argv,NOASK);
       }

/* We scan the list of options and try to match them with the options names
   recognized that are in the parmv table				*/

      else
       { cursor = argv[i] + 1;
         for(; *cursor != 0 ;)
          { for(j = 0 ; j < parmc ; j++)
             { if((length = match_string(parmv[j].name,cursor)) > 0)

/* a switch was recognized it will be processed */

                { cursor += length;
                  if(parmv[j].flag & P_PRESENT)
                   { fprintf(stderr,"%s: attempt to set twice switch -%s\n",
                                 argv[0],parmv[j].name);
                     exit(1);
                   }
                  parmv[j].flag = parmv[j].flag | P_PRESENT;

/* the parameter value was on the command line so we call set_value with
   the status noask.							*/

                  set_value(parmv + j,&i,argc,argv,NOASK);
                  break;
                }
             }

/* the switch was not found in the parmv table, we will print the 
   complete list of recognized switches.				*/

            if(j == parmc)
	     { fprintf(stderr,"%s: unknown switch -%s\n",argv[0],cursor);
	       fprintf(stderr,"\nThe switches recognized are :\n");
	       for(j = 0 ; j < parmc ; j++)
		{ fprintf(stderr,"    -%s %s, %s\n",
			parmv[j].name,parmv[j].descr,types[parmv[j].type - 1]);

		  k = 0;
		  for(i = 1 ; i <= P_DEFAULT ; i = i << 1)
		   { if(parmv[j].flag & i)
			fprintf(stderr,"       This parameter %s\n",options[k]);
		     k++;
		   }

		  k = 0;
		  for(; i <= P_ABSENT_A_EXCL ; i = i << 1)
		   { if(parmv[j].flag & i)
		        fprintf(stderr,"       This parameter %s -%s\n",
				prerequisite[k],parmv[parmv[j].a].name);
		     k++;
		   }
	
		  k = 0;
		  for(; i <= P_ABSENT_B_EXCL ; i = i << 1)
		   { if(parmv[j].flag & i)
			fprintf(stderr,"       This parameter %s -%s\n",
				prerequisite[k],parmv[parmv[j].b].name);
		     k++;
		   }
		}
	       exit(1);
	     } 
          }
       }
    }

/* All the options entered on the command line were processed. Now we will
   scan the parmv table to see which parameters are mandatory and were not
   entered.								*/ 

   for(j = 0 ; j < parmc ; j++)
    { if((parmv[j].flag & P_NEEDED) && (parmv[j].flag & P_PRESENT) == 0)
       { set_value(parmv + j,&i,argc,argv,ASK);
	 parmv[j].flag = parmv[j].flag | P_PRESENT;
       }
    }

/* All the mandatory parameters were read from the keyboard, we will now
   see if all the relations are satisfied. For each parameter we look if
   it has some requirements on the state of other parameters and we try to
   satisfy them. If it ask to a non-present parameter to be present, we simply
   ask the user for a value, if a present parameter should not be present, we
   are in big trouble and the program blows.

   The table is scanned until all relations fulfilled.
   The table may be scanned more than once because of prerequisite chains */

   satisfied = 0;
   for(; satisfied == 0 ;)
    { satisfied = 1;
      for(j = 0; j < parmc ; j++)

       { flag = parmv[j].flag;
	 reqa = 0;
	 reqb = 0;
	 reja = 0;
	 rejb = 0;

	 if(flag & P_PRESENT)
	  { if(flag & P_PRESENT_A_REQ)reqa = 1;
	    if(flag & P_PRESENT_A_EXCL)reja = 1;
	    if(flag & P_PRESENT_B_REQ)reqb = 1;
	    if(flag & P_PRESENT_B_EXCL)rejb = 1;
	  }

	 else
	  { if(flag & P_ABSENT_A_REQ)reqa = 1;
	    if(flag & P_ABSENT_A_EXCL)reja = 1;
	    if(flag & P_ABSENT_B_REQ)reqb = 1;
	    if(flag & P_ABSENT_B_EXCL)rejb = 1;
	  }

	 indirect = parmv[j].a;

	 if(reqa && ((parmv[indirect].flag & P_PRESENT) == 0))
	  { satisfied = 0;
	    parmv[indirect].flag |= P_PRESENT;
	    set_value(parmv + indirect,&i,argc,argv,ASK);
	  }

	 if(reja && (parmv[indirect].flag & P_PRESENT))
	  { fprintf(stderr,"%s: -%s is set but -%s excludes it\n",
		           argv[0],parmv[indirect].name,parmv[j].name);
	    exit(1);
	  }

	 indirect = parmv[j].b;

	 if(reqb && ((parmv[indirect].flag & P_PRESENT) == 0))
	  { satisfied = 0;
	    parmv[indirect].flag |= P_PRESENT;
	    set_value(parmv + indirect,&i,argc,argv,ASK);
	  }

	 if(rejb && (parmv[indirect].flag & P_PRESENT))
	  { fprintf(stderr,"%s: -%s is set but -%s excludes it\n",
			   argv[0],parmv[indirect].name,parmv[j].name);
	    exit(1);
	  }
       }
    }
}

/***********************************************************************

NAME 
	set_value


PURPOSE
	This function reads and checks values either from the argv table
	or from the keybord.

SYNOPSIS
	set_value(parm,ipnt,argc,argv,source)
	struct parameter *parm;
	int *ipnt,argc,source;
	char **argv;

DESCRIPTION
	The value received is checked for bounds imposed by the calling
	program in the table and the proper action is taken depending
	on the parameter type. If anything goes wrong, a message is issued
	and a new value is requested to the user. When the source is ASK
	the value is requested from the keyboard. When the source is NOASK
	the value is taken from the array argv and the pointer ipnt is
	incremented.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	2 july 1984

AUTHOR
	Michel DAGENAIS

***************************************************************************/


set_value(parm,ipnt,argc,argv,source)

struct parameter *parm;
int *ipnt;
int argc;
char **argv;
int source;

{ int
	length;		/* length of a character string excluding the null */

  struct p_file *p1;	/* pointer for file parameters */

  struct p_2file *p2;	/* pointer for parameter of type file2 */

  struct p_integer *p3;	/* pointer for integer parameter */

  struct p_float *p4;	/* pointer for floating point parameter */

  struct p_string *p5;	/* pointer for string parameter */

  struct p_character *p6; /* pointer for character argument */
  
  struct p_logical *p7;	/* pointer for logical parameter */

  char 
	*begin_ext,	/* address in buffer of the beginning of the ext */
	buffer[132];	/* temp storage for strings to read, 132 car max */


/* if it is read from the argv table, we increment the pointer in the table
   and check that at least one entry remains, otherwise a message is issued */

  if(parm->type < P_LOGICAL && source == NOASK)
   { (*ipnt)++;
     if(*ipnt >= argc)
      { fprintf(stderr,
	"%s: switch -%s must be followed by an argument\n",argv[0],parm->name);
	exit(1);
      }
   }

/* we branch to the program section corresponding to the parameter type */

  switch (parm->type)

/* it is a file to open */

   { case P_FILE :
      p1 = (struct p_file *)parm->def_pointer;
      for(; ;)
       { if(source == ASK)
          { fprintf(stdout,"%s: enter %s file name : ",argv[0],parm->descr);
            if(fscanf(stdin,"%s",buffer) == 0)
	     { fprintf(stderr,"\n%s: illegal character at input\n",argv[0]);
	       exit(1); 
	     }
	    putc('\n',stdout);
          }
         else copy_string(argv[(*ipnt)],buffer);
         source = ASK;
	 if(sfind_next_car('.',buffer) == 0) 
	  { if(p1->access[0] != 'r' || 
	      (*(p1->fp) = fopen(buffer,p1->access)) == NULL)
	     { append_string(".",buffer);
	       append_string(p1->default_ext,buffer);
               if((*(p1->fp) = fopen(buffer,p1->access)) == NULL)
                { fprintf(stderr,"%s: cannot open %s file %s\n",
			   argv[0],parm->descr,buffer);
	        }
	       else break;
	     }
	    else break;
	  }
         else if((*(p1->fp) = fopen(buffer,p1->access)) == NULL)
          { fprintf(stderr,"%s: cannot open %s file %s\n",
			   argv[0],parm->descr,buffer);
          }
         else break;
       }
      break;

/* we want to open two files with same name but different extensions */
  
     case P_2FILE :
      p2 = (struct p_2file *)parm->def_pointer;
      for(; ;)
       { if(source == ASK)
          { fprintf(stdout,"%s: enter %s file name : ",argv[0],parm->descr);
            if(fscanf(stdin,"%s",buffer) == 0)
	     { fprintf(stderr,"\n%s: illegal character at input\n",argv[0]);
	       exit(1);
	     }
            putc('\n',stdout);
          }
         else copy_string(argv[(*ipnt)],buffer);
         source = ASK;
	 append_string(".",buffer);
	 append_string(p2->default_ext1,buffer);
         if((*(p2->fp1) = fopen(buffer,p2->access1)) == NULL)
          { fprintf(stderr,
                    "%s: cannot open %s file %s\n",argv[0],parm->descr,buffer);
          }
	 else
          { begin_ext = sfind_next_car('.',buffer);
	    *(begin_ext + 1) = 0;
	    append_string(p2->default_ext2,buffer);
	    if((*(p2->fp2) = fopen(buffer,p2->access2)) == NULL)
             { fprintf(stderr,
                    "%s: cannot open %s file %s\n",argv[0],parm->descr,buffer);
             }
            else break;
	  }
       }
      break;

/* we want to input an integer value */

     case P_INTEGER :
      p3 = (struct p_integer *)parm->def_pointer;
      for(; ;)
       { if(source == ASK)
          { fprintf(stdout,
		    "%s: enter %s integer value : ",argv[0],parm->descr);
            if(fscanf(stdin,"%d",p3->intp) == 0)
	     { putc('\n',stdout);
	       if(fscanf(stdin,"%s",buffer) == 0)
		{ fprintf(stderr,"\n%s: illegal character at input\n",argv[0]);
		  exit(1);
		}
	       continue;
	     }
	    putc('\n',stdout);
          }
         else *(p3->intp) = atoi(argv[(*ipnt)]);
         source = ASK;
         if(*(p3->intp) < p3->min || *(p3->intp) > p3->max)
          { fprintf(stderr,
                    "%s: %d value entered for %s not between %d and %d\n",
                    argv[0],*(p3->intp),parm->descr,p3->min,p3->max);
          }
         else break;
       }
      break;

     case P_FLOAT :
      p4 = (struct p_float *)parm->def_pointer;
      for(; ;)
       { if(source == ASK)
          { fprintf(stdout,
		    "%s: enter %s float value : ",argv[0],parm->descr);
            if(fscanf(stdin,"%f",p4->floatp) == 0)
	     { putc('\n',stdout);
	       if(fscanf(stdin,"%s",buffer) == 0)
		{ fprintf(stderr,"\n%s: illegal character at input\n",argv[0]);
		  exit(1);
		}
	       continue;
	     }
	    putc('\n',stdout);
          }
         else *(p4->floatp) = atof(argv[(*ipnt)]);
         source = ASK;
         if(*(p4->floatp) < p4->min || *(p4->floatp) > p4->max)
          { fprintf(stderr,
                    "%s: %e value entered for %s not between %e and %e\n",
                    argv[0],*(p4->floatp),parm->descr,p4->min,p4->max);
          }
         else break;
       }
      break;

/* we want to input a string */

     case P_STRING :
      p5 = (struct p_string *)parm->def_pointer;
      for(; ;)
       { if(source == ASK)
          { fprintf(stdout,
  		    "%s: enter %s character string : ",argv[0],parm->descr);
            if(fscanf(stdin,"%s",buffer) == 0)
	     { fprintf(stderr,"\n%s: illegal character at input\n",argv[0]);
	       exit(1);
	     }
	    putc('\n',stdout);
          }
         else copy_string(argv[(*ipnt)],buffer);
         source = ASK;
         screen_backslash(buffer);
	 length = length_string(buffer);
         if(length < p5->min || length > p5->max)
          { fprintf(stderr,
                    "%s: %d chars entered for %s not between %d and %d\n",
                    argv[0],length,parm->descr,p5->min,p5->max);
          }
         else 
	  { *(p5->charp) = copy_and_alloc_string(buffer);
	    break;
	  }
       }
     break;


/* we want to input a character */

     case P_CHARACTER :
      p6 = (struct p_character *)parm->def_pointer;
      if(source == ASK)
       { fprintf(stdout,
  		 "%s: enter %s single character : ",argv[0],parm->descr);
         if(fscanf(stdin,"%s",buffer) == 0)
	  { fprintf(stderr,"\n%s: illegal character at input\n",argv[0]);
	    exit(1);
	  }
	 putc('\n',stdout);
       }
      else copy_string(argv[(*ipnt)],buffer);
      screen_backslash(buffer);
      *(p6->car) = buffer[0];
     break;

/* a logical value is to be set */

    case P_LOGICAL :
     p7 = (struct p_logical *)parm->def_pointer;
     if(*(p7->logp)) *(p7->logp) = FALSE;
     else *(p7->logp) = TRUE;
     if(source == ASK)
      { fprintf(stdout,
		"%s: %s is required and will be set\n",argv[0],parm->descr);
      }
     break;
   }
}

/*************************************************************************

NAME
	copy_string, copy_and_alloc_string, length_string

PURPOSE
	Ease the string manipulation by the calling functions.

SYNOPSIS
	copy_string(string_in,string_out)
	char *string_in,*string_out;

	char *copy_and_alloc_string(string)
	char *string;

	int length_string(string)
	char *string;

DESCRIPTION
	these subroutines copy all the characters from one string to the
	other, the string end is marked by a NULL character. The copy and
	allocate routine returns the pointer to the new string allocated.
	The length string function returns the number of characters in the 
	string.

COORDINATES
	McGill University Electrical Engineering MONTREAL CANADA
	2 july 1984

AUTHOR
	Michel DAGENAIS

*************************************************************************/

copy_string(string_in,string_out)
char *string_in,*string_out;

{
  for(; (*(string_out++) = *(string_in++)) != 0 ;);
}

/***********************************************************************/

char *copy_and_alloc_string(string)
char *string;

{
  char *pointer;
  pointer = calloc((unsigned)(length_string(string) + 1),sizeof(char));
  copy_string(string,pointer);
  return(pointer);
}

/************************************************************************/

int length_string(string)
char *string;

{
  int length;
  for(length = 0 ; *(string++) != 0 ; length++);
  return(length);
}

/*************************************************************************

NAME
	match_string, sfind_next_car, append_string

PURPOSE
	Ease the string manipulation by the calling functions.

SYNOPSIS
	int match_string(string_searched,line)
	char *string_searched,*line;

	char *sfind_next_car(car,string)
	char car,*string;

	append_string(string,line)
	char *string,*line;

DESCRIPTION
	Match looks if the string is entirely present in the line and
	if so it returns its length otherwise 0 is returned. Find car
	returns the address of the byte containing the car to find or
	0 if it is not found. Append simply put the string at the end
	of the line.

COORDINATES
	McGill University Electrical Engineering MONTREAL CANADA
	2 july 1984

AUTHOR
	Michel DAGENAIS

*************************************************************************/

int match_string(string_searched,line)

char *string_searched,*line;
{
  int length;
  for(length = 0 ; *string_searched != 0 && *line != 0 ; length++)
   { if(*(string_searched++) != *(line++)) return(0);
   }
  if(*string_searched == 0) return(length);
  return(0);
}

/*************************************************************************/

char *sfind_next_car(car,string)

char car,*string;
{
  for(; *string != 0 && *string == car ; string++);
  for(; *string != 0 && *string != car ; string++);
  if(*string == car) return(string);
  return(0);
}

/***********************************************************************/

append_string(string,line)

char *string,*line;
{
  for(; *line != 0 ; line++);
  for(; (*(line++) = *(string++)) != 0 ;);
}

/**********************************************************************

NAME
	screen_backslash

PURPOSE
	To help the user to enter some control characters in his strings
	the standard backslash notation will be used. This function replaces
	the occurence of \x by y where x is a character in the table and
	y is the corresponding control character.

SYNOPSIS
	screen_backslash(buffer)
	char *buffer;

DESCRIPTION
	The buffer is screened until the end ( null character ) and each time
	a \ character is found we replace the following character by a the 
	corresponding character in a table.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	16 august 1984

AUTHOR
	Michel DAGENAIS

***************************************************************************/

screen_backslash(buffer)

char *buffer;
{
  char *cursor;		/* pointer in the buffer */

  static char table[] =			/* table of characters to replace */
	 { 'n','r','\\','t'};

  static char replace[] =		/* table of replacing characters */ 
	{ '\n','\r','\\','\t' };

  static int table_size = sizeof(table) / sizeof(char); /* size of the table */

  int i;		/* index in the tables */

  cursor = buffer;

  for(; ;)
   { if(*buffer == '\\')
      { buffer++;
	for(i = 0 ; i < table_size ; i++) if(*buffer == table[i]) break;
        if(i == table_size) *cursor = *buffer;
	else *cursor = replace[i];
      }
     else *cursor = *buffer;
     if(*cursor == 0) return;
     cursor++;
     buffer++;
   }
}
