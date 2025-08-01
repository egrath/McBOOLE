#include "cubes.h"

/* 
   COMMON VARIABLES
	The variables below have to be accessed at many places in the
	program and will be declared as external in cubes.h
									*/

int
	current_var,		/* current variable on which op are performed */
	current_bit_index,	/* position of the current var in the word */
	current_word_index;	/* position of the current var in the cube */

long int
	current_mask11, 	/* mask to extract the value of current var.*/
	current_mask10;		/* mask to see if star_product possible */

/***************************************************************************

NAME
	define_current_var, extract_current_var,
	extract_var, set_current_var, set_var

PURPOSE
	These function are useful for operations on a single bit. We can
	define a current variable, extract its value, change its value
	using very fast functions since all the masks for the current var
	are computed only when define current var is called. We can also
	extract another var using the extract_var function. The current var
	is used by star_product; the star_product function will only merge
	cubes along the current var which should be previously defined.

SYNOPSIS
	define_current_var(var)
	int var;

	int extract_current_var(cube)
	long int *cube;

	int extract_var(cube,var)
	long int *cube;
	int var;

	set_current_var(cube,code)
	long int *cube,code;

	set_var(cube,code,var)
	long int *cube,code;
	int var;

DESCRIPTION
	
    -define_current_var makes Var the current variable and compute all the masks
	needed to access it rapidly.

    -extract_current_var returns the code 0 to 3 corresponding to the code
	packed in the current_var.

    -extract_var returns the code 0 to 3 in the variable Var.

    -set_current_var places the Code at the current var in the Cube.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

define_current_var(var)

int var;
{
/* If your machine does not have a number of bits per long int which is a
   power of 2, you can replace 'var >> log2_var_per_word' by
   'var / var_per_word' and '(var & mask_bit_index) << 1' by
   '(var % var_per_word) * 2'. The same change should also be made in 
   extract_var.								*/ 
   
  current_var = var;
  current_word_index = var >> log2_var_per_word;
  current_bit_index = (var & mask_bit_index) << 1;
  current_mask11 = 3L << current_bit_index;
  current_mask10 = 2L << current_bit_index;
}

/**********************************************************************/

int extract_current_var(cube)

long int *cube;
{
  cube = cube + current_word_index;
  return( (*cube >> current_bit_index) & 3);
}

/*************************************************************************/

int extract_var(cube,var)

long int *cube;
int var;
{
  cube = cube + (var >> log2_var_per_word);
  return((*cube >> ((mask_bit_index & var) << 1)) & 3);
}

/**************************************************************************/

set_current_var(cube,code)

long int *cube,code;
{
   cube = cube + current_word_index;
   *cube = *cube & (~current_mask11);
   *cube = *cube | (code & current_mask11);
}

/***************************************************************************/

#ifdef DEBUG

set_var(cube,code,var)

long int *cube, code;
int var;
{
  long int mask;	/* mask to modify the specified var */

  cube = cube + (var >> log2_var_per_word);
  mask = 3L << ((var & mask_bit_index) << 1); 
  *cube = *cube & (~mask);
  *cube = *cube | (code & mask);
}

#endif

