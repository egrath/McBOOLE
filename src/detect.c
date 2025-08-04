#include "cubes.h"

/***************************************************************************

NAME
	count_code_in_cube

PURPOSE
	This function counts the number of occurences of a certain 2 bits
	code that represents one variable as packed in long int words

SYNOPSIS
	int count_code_in_cube(cube,bit_number,code)
	long int *cube,code;
	int bit_number;

DESCRIPTION
	we scan bit_number variables packed in long int words starting at cube
	for the pattern in code. The number of occurences is returned.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

int count_code_in_cube(cube,bit_number,code)

long int *cube,code;
int bit_number;
{
  long int mask;	/* mask to observe a single variable */
  int code_number;	/* number of occurences of the code in the cube */

  code_number = 0;
  for(; bit_number > 0 ;)
   { for(mask = 3; mask != 0 && bit_number > 0 ; mask = mask << 2)
      { if((mask & code) == (mask & *cube)) code_number++;
        bit_number--;
      }
     cube++;
   }
  return(code_number);
}

/************************************************************************

NAME
	input_cost, output_cost

PURPOSE
	Compute the number of contacts involved in an equivalent pla for
	input and output parts of a cube.

SYNOPSIS
	int input_cost(cube)
	long int *cube;

	int output_cost(cube)
	long int *cube;

DESCRIPTION
    -input_cost counts and returns the number of 0 and 1 in the input part of
	cube. In fact we substract the number of x from the number of input.

    -output_cost counts the number of 1 in the output part the cube.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	16 august 1984

AUTHOR
	Michel DAGENAIS

**************************************************************************/

int input_cost(cube)

long int *cube;
{
  return(input_number - count_code_in_cube(cube,input_number,mask11));
}

/***************************************************************************/

int output_cost(cube)

long int *cube;
{
  return(count_code_in_cube(cube + input_length,output_number,mask01));
}

/***************************************************************************

NAME
	detect_dont_care, remove_dont_care, detect_do_care, remove_do_care,
	change_dont_to_do_care

PURPOSE
	These functions are useful to examine the output part of a cube and
	detect the presence of dont care or do care bits. They can also
	be used to remove the do or dont cares. They are used to extract
	the dont care cover of a function before the minimization is
	performed.

SYNOPSIS
	int detect_dont_care(cube)
	long int *cube;

	remove_dont_care(cube)
	long int *cube;	

	int detect_do_care(cube)
	long int *cube;

	remove_do_care(cube)
	long int *cube;

	change_dont_to_do_care(cube)
	long int *cube;

DESCRIPTION
	In each case the cube to examine is cube.

    -detect_dont_care returns 1 if some dont care bits are present at output
	and 0 otherwise.

    -remove_dont_care changes the dont care bits at output by 0.

    -detect_do_care returns 1 if some 1 are present at output and 0 if only
	dont care and 0 are present at output.

    -remove_do_care changes the 1 at output by 0.

    _change_dont_to_do_care changes the dont care bits at output by 1.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

int detect_dont_care(cube)

long int *cube;
{
  long int 
	*end_of_output;	/* pointer to end of cube */

  end_of_output = cube + total_length;
  cube = cube + input_length;

  for(; cube < end_of_output ; cube++)
   { if(*cube & mask10) return(1);
   }
  return(0);			/* no dont care at output */
}

/****************************************************************************/

remove_dont_care(cube)

long int *cube;
{
  long int
	*end_of_cube;		/* pointer to end of cube */

  end_of_cube = cube + total_length;
  cube += input_length;

  for(; cube < end_of_cube ; cube++)
   { *cube |= (*cube >> 1) & mask01;		/* change 10 codes to 11 */
     *cube = ((*cube >> 1) ^ *cube) & mask01;   /* change 11 codes to 00 */ 
   }
}

/***********************************************************************/

change_dont_to_do_care(cube)

long int *cube;
{
  long int 
	*end_of_cube;		/* pointer to end of cube */

  end_of_cube = cube + total_length;
  cube += input_length;

  for(; cube < end_of_cube ; cube++)
   { *cube |= *cube >> 1 ; 
     *cube = *cube & mask01;
   }
}

/***************************************************************************/

int detect_do_care(cube)

long int *cube;
{
  long int
	*end_of_cube;		/* pointer to end of cube */

  end_of_cube = cube + total_length;
  cube = cube + input_length;

  for(; cube < end_of_cube ; cube++)
   { *cube |= (*cube >> 1) & mask01;		/* change 10 to 11 */ 
     if(((*cube >> 1) ^ *cube) & mask01) return(1);
   }
  return(0);
}

/***************************************************************************/

remove_do_care(cube)

long int *cube;
{
  long int
	result,			/* intermediate value */
	*end_of_cube;		/* pointer to end of cube */

  end_of_cube = cube + total_length;
  cube = cube + input_length;

  for(; cube < end_of_cube ; cube++)
   { result = (*cube & mask10);
     *cube = result | (result >> 1);
   }
}

