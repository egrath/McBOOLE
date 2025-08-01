/************************************************************************

NAME
	param.h

DESCRIPTION
	This file contains the definitions of all the structures used to
	define the parameter table to be initialized by the main program
	and executed by the program param. New type of parameters can be
	added by putting new structure definitions in this file and by
	adding the new features accordingly in the param program.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	2 july 1984

AUTHOR
	Michel DAGENAIS

*********************************************************************/

#define P_NEEDED 1		/* mask indicating parameter not optional */
#define P_OPTIONAL 0		/* mask telling parameter is optional */
#define P_PRESENT 2		/* mask telling the parameter was received */
#define P_DEFAULT 4		/* tells to take it if no switch appears */
#define P_PRESENT_A_REQ 8	/* a required when parameter is present */
#define P_PRESENT_A_EXCL 16	/* a excluded when parameter present */
#define P_ABSENT_A_REQ 32	/* a required when parameter is absent */
#define P_ABSENT_A_EXCL 64	/* a excluded when parameter is absent */
#define P_PRESENT_B_REQ 128	/* b required when parameter is present */
#define P_PRESENT_B_EXCL 256	/* b excluded when parameter is present */
#define P_ABSENT_B_REQ 512	/* b required when parameter is absent */
#define P_ABSENT_B_EXCL 1024	/* b excluded when parameter is absent */



/*	the structure parameter contains information about this parameter,
	its type, related parameters, and contains also a pointer
	to a structure adapted to the parameter type containing whatever
	information is relevant for this particular type of parameter */

struct parameter
{ char *name;		/* name of the option as entered by the user */
  char *descr;		/* string for parameter description */
  int type;		/* number of the parameter type */
  int a;		/* index of a related parameter in the table */
  int b;		/* index of a related parameter in the table */
  unsigned int flag;	/* word containing information for this parameter */
  char *def_pointer;	/* pointer to structure defining the parameter */
};

/*	the structures below are defined one for each of the different type
	of parameters recognized by the param program			*/

/*	opening of a file	*/

#define P_FILE 1

struct p_file
{ char *default_ext;	/* default extension if the user does not give one */
  char *access;		/* access wanted for that file like "r" or "w"...*/
  FILE **fp;		/* pointer to the file pointer to set */
};

/*	opening of two files with the same name but with different access and
	extension to the name						*/

#define P_2FILE 2

struct p_2file
{ char *default_ext1;	/* default extension if the user does not give one */
  char *access1;	/* access wanted for that file like "r" or "w"...*/
  char *default_ext2;	/* default extension if the user does not give one */
  char *access2;	/* access wanted for that file like "r" or "w"...*/
  FILE **fp1;		/* pointer to the file pointer to set */
  FILE **fp2;		/* pointer to the file pointer to set */
};

/*	receive an integer value	*/

#define P_INTEGER 3

struct p_integer
{ int min;		/* minimum value accepted */
  int max;		/* maximum value accepted */
  int *intp;		/* integer to read */
};

/*	receive a floating point value	    */

#define P_FLOAT 4

struct p_float
{ float min;		/* minimum value accepted */
  float max;		/* maximum value accepted */
  float *floatp;	/* float to read */
};

/*	receive a character string	*/

#define P_STRING 5

struct p_string
{ int min;		/* minimum number of char accepted */
  int max;		/* maximum number of char accepted */
  char **charp;		/* character pointer where to send string */
};

/* 	receive a character	*/

#define P_CHARACTER 6

struct p_character
{ char *car;
};

/*	logical value to set, the parameters asking for no values should
	all be placed after this one for proper processing by the param
	program								*/

#define P_LOGICAL 7 

struct p_logical
{ int *logp;		/* logic variable to set to 1 */
};

