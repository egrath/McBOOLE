#include "cubes.h"

/*

COMMON VARIABLES
	The variables below are common to all the modules. They are defined
	as external variables in cubes.h 
*/

int
	var_per_word = sizeof(long int) / sizeof(char) * 4,
				/* number of variables packed in a long word */
	log2_var_per_word;	/* the number of var in a long int is assumed */
				/* to be a power of 2 to simplify the */
				/* computation on the word index in a cube */

long int
	mask_bit_index,		/* mask to extract bit index from var number */
	mask00,			/* word containing 00000000...	*/
	mask01,			/* word containing 01010101...  */
	mask10,			/* word containing 10101010...  */
	mask11;			/* word containing 11111111...  */

/*****************************************************************************

NAME
	init_mask_and_codes

PURPOSE
	This function simply initializes the mask and codes used in the 
	program; this initialization cannot be done before runtime because
	no assumptions are made on the type of machine used and correspondingly
	on the number of bits in a long words. However we assume that it is a
	multiple of the number of bits in a character and that the number of
	bits in a character is 8 which should be quite reasonable for most
	machines.

SYNOPSIS
	init_mask_and_codes

DESCRIPTION
	the masks are initialized with a simple loop in which all words
	are shifted 2 bits left each time; at the end the long words contain
	the proper patterns;

DIAGNOSTIC
	The program makes the assumption that the number of bits in a long int
	is a power of 2. It helps for some bit operations. This assumption
	seems reasonable for most machines, however if your machine is more
	fancy, the program will detect the error. In such a case, you will
	have to modify this functiion and the functions extract_var and
	define_current_var but it is very simple, they are only 10 lines long
	or less.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

init_mask_and_codes()

{
  long int mask;

  mask00 = NULL;
  mask11 = NULL;
  mask10 = NULL;
  mask01 = NULL;

  for(mask = 3 ; mask != NULL ; mask = mask << 2)
   { mask11 = (mask11 << 2) | 3;
     mask01 = (mask01 << 2) | 1;
     mask10 = (mask10 << 2) | 2;
   }

  input_code[0] = mask11;
  input_code[1] = mask11;
  input_code[2] = mask10;
  input_code[3] = mask01;

  output_code[0] = mask00;
  output_code[1] = mask01;
  output_code[2] = mask11;
  output_code[3] = mask11;

/* we initialize here log2 var per word and mask bit index. For proper
   operation of the functions set current var and extract var, the machine
   should have a number of bits per long word which is a power of 2. If
   not a message will be issued and the user can change it very easily
   at a little cpu cost; the operation required to compute the word index
   will be a division instead of a simple shift.			*/

  log2_var_per_word = NULL;
  mask_bit_index = NULL;
  for(mask = 1 ; mask != NULL ; mask = mask << 1)
   { if(mask == var_per_word) break;
     log2_var_per_word++;
     mask_bit_index = (mask_bit_index << 1) | 1;
   }

  if(mask == NULL) fatal_system_error(
   "The program should be modified for this machine see init_mask_and_codes");

#ifdef DEBUG
  printf("log2vpw %d vpw %d mask_bit_index %lx\n",log2_var_per_word,
         var_per_word,mask_bit_index);
#endif

}
