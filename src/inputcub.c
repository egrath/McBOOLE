#include "cubes.h"
#define CR '\n'
#define BL ' '

/*

COMMON VARIABLES
	The variables defined below are used in many modules and are defined
	external in the file cube.h
*/

int
	input_number,		/* number of input variables */
	output_number,		/* number of outpput variables */
	total_number,		/* number of variables */
	input_length,		/* number of long int words to store inputs */
        output_length,		/* number of long int words to store outputs */
	total_length;		/* number of long int words to store a cube */

char error_buffer[132];		/* buffer to hold the error messages */ 

unsigned
	node_size,		/* size in bytes of a node including the cube*/
	cube_list_size;		/* size of a cube_list including the cube */

struct node *spare_node;	/* a node is allocated for temp usage */

/*

TABLES
	Here are defined and initialized the tables useful for the input
	and output of cubes. We have the literal corresponding to each
	internal code for a boolean variable.

*/

char 
	input_literal[4] = { 'x','X','0','1' },	
				/* holds the characters accepted for the
				   input variables			  */

	output_literal[4] = { '0','1','d','D' };
				/* holds the characters accepted for output
				   variables				  */

long int
	input_code[4],		/* holds the code for the character at input */
	output_code[4];		/* holds the code for the output characters */

/****************************************************************************

NAME
	ffind_car

PURPOSE
	this functions skips characters in the input file until a EOF or the
	specified character is found.

SYNOPSIS
	char ffind_car(fp,lineno,car_searched)
	FILE *fp;
	int *lineno;
        char car_searched;

DESCRIPTION
	the characters are skipped from the file pointed by fp; EOF or
	the specified character is returned depending which is 
        encountered first. The line number is updated if needed.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

char ffind_car(fp,lineno,car_searched)

FILE *fp;
int *lineno;
char car_searched;
 {
   int	car;		/* byte to store the character read */

   for(; (car = getc(fp)) > EOF ;)
    { if(car == CR) (*lineno)++ ;
      if(car ==  car_searched) break;
    }			
   return(car);
 }

/***************************************************************************

NAME
	fread_bit_string

PURPOSE
	This function reads a string of bits and pack them in long words
	at the rate of 4 input bits per byte since up to 4 codes are
	recognized by this function.

SYNOPSIS
	fread_bit_string(fp,lineno,word,bit_number,terminator,literal,code)
	FILE *fp;
	long int *word,code[4];
	int bit_number,*lineno;
	char terminator,literal[4];

DESCRIPTION
	the characters are read from the file pointed by fp;
	When we encounter the terminator, we stop reading.
	The comments and blanks are skipped. The non-blank
	character are compared with the literals in the table;
	if the character is not in the table a message is issued.
	When we found the character in the table the corresponding code
	is placed in the long word. When a word is full, we increment the
	pointer. When the number of bits read is not bit_number, a 
	message is issued to the user. We keep track of the number of lines
	for diagnostic purpose. The function returns 1 when a cube was read
	properly, -1 when a cube was read but the terminator was missing and EOF
	was encountered. If anything goes wrong a message is issued
	and the function never returns. The function returns 0 when the EOF
	was encountered before any bit was found.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

int fread_bit_string(fp,lineno,word,bit_number,terminator,literal,code)

FILE *fp;
long int *word,code[4];
int bit_number,*lineno;
char terminator,literal[4];
{
  unsigned long int mask;     /* mask used to pack the variables in long int */

  int car;		/* byte to store the character returned by functions */

  int 
	nb_bit_read,	/* number of bits read */
	i;		/* loop counter	for code determination */

/* we fill the long words right to left as the variables values are read, 
   using the mask which is shifted left 2 bits each time to go to the next
   variable stored in the long word.					*/

  nb_bit_read = 0;
  for(; ;)
   { *word = mask00;
     for( mask = 3 ; mask != 0 ; mask = mask << 2)

/* We read characters at input until the EOF or the terminator is reached */

      { for(; ;)
	 { car = getc(fp);
	   if(car == CR) (*lineno)++;
	   if(car == terminator && nb_bit_read != 0) break;
	   if(car == '/') 
	    { car = ffind_car(fp,lineno,'/');
              if(car == '/') continue;
	    }
	   if(car <= EOF) break;
	   if(car <= BL) continue;
	   break;
	 }

/* The terminator is reached, we will check that the number of bits read was
   correct. The rest of the word is filled with the default code (code[0]). */

	if(car == terminator)
	 { if(nb_bit_read != bit_number)
	    { sprintf(error_buffer,
	      "wrong number of bits on line %d '%c' was reached after %d bits",
	      *lineno,terminator,nb_bit_read);
	      fatal_user_error(error_buffer);
	    }
           for(; mask > 3 ; mask = mask << 2) *word |= code[0] & mask; 
           return(1);
	 }

/* The end of file is reached, if a string of proper length was read the
   user is warned that the terminator is missing and we return -1.	*/

	if(car <= EOF)
	 { if(nb_bit_read == bit_number)
	    { for(; mask > 3 ; mask = mask << 2) *word |= code[0] & mask;
	      warning_user_error("missing terminator at end of file");
	      return(-1);
	    }
           if(nb_bit_read == 0) return(0);
	   sprintf(error_buffer,"EOF encountered after %d bits %d were expected"
			       ,nb_bit_read,bit_number);
	   fatal_user_error(error_buffer);
	 }

/* We will try to find the literal in the table */

        for(i = 0 ; i < 4 ; i++) if(car == literal[i]) break;
 
/* The literal was not found, it is an invalid character, a message is issued */

	if(i == 4)
	 { sprintf(error_buffer,"invalid character '%c' in line %d",car,*lineno);
	   fatal_user_error(error_buffer);
	 }

/* The specified number of bits were read and the terminator is not reached
   a message will be issued.					*/

	if(nb_bit_read >= bit_number)
	 { sprintf(error_buffer,"too many bits on line %d '%c' not reached",
		   *lineno,terminator);
	   fatal_user_error(error_buffer);
	 }

/* it is in the table so we put it in the long word			*/

        *word |= mask & code[i];
        nb_bit_read++;
      }
     word++;
   }
}

/*****************************************************************************

NAME
	fread_nodes

PURPOSE
	This function reads all the cubes from the input file and store
	them in the proper form after allocating space for them.

SYNOPSIS
	int fread_nodes(fp,list)
	FILE *fp;
	struct node **list;

DESCRIPTION
	When this function is called for the first time, the variable
	input_number is 0; then we read one cube to determine the number of
	input and output bits and we compute the size of the structures needed.
	When the number of inputs is known, we simply read the cubes one by 
	one from fp and allocate space for them. The list formed goes in list
	and the number of cubes in list is returned. Any error reading the
	cube will be signaled by this function or by the function 
	fread_bit_string called to read the cubes. 

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

int fread_nodes(fp,list)

FILE *fp;
struct node **list;
{
  int car;		/* character read */ 

  int 
	list_number,	/* number of cubes read and placed in the list */
	line_number,	/* number of lines read at input for diagnostic */
	answer;		/* return code of the function fread bit string */

  struct node **previous;	/* pointer to node.next_cube of the previous*/
				/* node to link the chain while reading */

/* We will read the first cube to count the number of input and output
   variables. The size of the structures will be computed.		*/

  if(input_number == 0)
   { line_number = 0;
     for(; ;)
      { car = getc(fp);
	if(car == read_interminator && input_number != 0) break; 
	if(car <= EOF) fatal_user_error("EOF encountered on first line"); 
        if(car <= BL) continue;
	if(car == '/') 
	 { car = ffind_car(fp,&line_number,'/');
	   if(car <= EOF) fatal_user_error("EOF encountered on first line");
	 }
	else input_number++;
      }
   
     for(; ;)
      { car = getc(fp);
	if(car == read_outterminator && output_number != 0) break;
	if(car <= EOF) break;
        if(car <= BL) continue;
	if(car == '/') 
	 { car = ffind_car(fp,&line_number,'/');
	   if(car <= EOF) break;
	 }
	else output_number++;
      }

/* We now have the number of input and output, so we can compute the different
   variables that depend on those values.				*/

     input_length = ((input_number - 1) / var_per_word) + 1;
     output_length = ((output_number - 1) / var_per_word) + 1;
     total_length = input_length + output_length;
     total_number = input_number + output_number;

/* The nodes already include 2 long int word which is the minimum space a cube
   is allowed to take. The actual length of the cube minus the 2 minimum
   is then added to the sizeof(node) to get the nodesize appropriate to our
   number of input and output.						*/

     node_size = sizeof(struct node) + (total_length - 2) * sizeof(long int);
     cube_list_size = 
	    sizeof(struct cube_list) + (total_length - 2) * sizeof(long int);

/* We now reset the file pointer to the beginning of the file to read all the
   nodes and place them in the list. Also we initialize all the constants needed
   to process the cubes. It is a convenient place to make this call since 
   before any processing can be done we have to read the cubes.		    */

     init_mask_and_codes();
     rewind(fp);
     spare_node = alloc_node();
   } 

/* We will now read the list of nodes from the file, the line number and the
   number of nodes in the list are initialized.				*/ 
 
  previous = list;
  line_number = 1;
  list_number = 0;
 
  for(; ;)

   { answer = fread_bit_string(fp,&line_number,spare_node->cube,input_number,
		read_interminator,input_literal,input_code);

/* No more cubes in the file, the EOF was encountered */

     if(answer == 0) break;
     if(answer == -1) fatal_user_error("last cube of file has no output part");

     answer = fread_bit_string(fp,&line_number,spare_node->cube + input_length,
	output_number,read_outterminator,output_literal,output_code);

/* The EOF was encountered but the cube was not finished */

     if(answer == 0)
      { fatal_user_error("The output of the last cube is incomplete");
      }

/* another node was read succesfully, place it next in the list.	*/

     *previous = spare_node;
     spare_node->next_node = alloc_node();
     previous = &(spare_node->next_node);
     spare_node = spare_node->next_node;
     list_number++;
 
 /* the cube read was the last one of the file */

     if(answer == -1) break;
   }

/* the last node we tried to read was not there, the eof was encountered,
   so we must remove it from the list				*/

  *previous = NULL;
  return(list_number);
}

