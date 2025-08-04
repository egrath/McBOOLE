#include "cubes.h"

/***************************************************************************

NAME
	set_output_word, or_output, or_output_list, and_output, and_output_list,
	invert_output, bigger_output, empty_output

PURPOSE
	When at input many cubes are intersecting, we have at some point
	to combine their output part using or and and functions.

SYNOPSIS
	set_output_word(cube,code)
	long int *cube, code;

	or_output(cube,or_cube)
	long int *cube, *or_cube;

	or_output_list(list,or_cube)
	long int *or_cube;
	struct cube_list *list;

	and_output(cube,and_cube)
	long int *cube, *and_cube;

	and_output_list(list,and_cube)
	long int *and_cube;
	struct cube_list *list;

	invert_output(cube)
	long int *cube;

	bigger_output(cube1,cube2)
	long int *cube1, *cube2;

	int empty_output(cube)
	long int *cube;

DESCRIPTION

    -set_output_word puts everywhere in the output part of cube the Code.

    -or_output makes the or of the output of Cube and Or_cube and places the
	result back in Or_output.

    -or_output_list or all the output of the cubes in list into the or_cube.

    -and_output makes the and of the output of Cube and And_cube and places the
	result back in And_output.

    -and_output_list and all the output of the cubes in list into and_cube.

    -invert_output just inverts the output of a cube.

    -bigger_output checks if cube1 has some output bits not set in cube2.
	if so it returns 1, else 0.

    -empty_output returns 1 when the output part is all 0s. 

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	1 august 1984

AUTHOR
	Michel DAGENAIS

****************************************************************************/

set_output_word(cube,code)

long int *cube, code;
{
  long int 
	*end_of_cube;		/* pointer to end of cube */

  end_of_cube = cube + total_length;
  cube = cube + input_length;

  for(; cube < end_of_cube ; cube++)
   { *cube = code;
   }
}

/*************************************************************************/

or_output(cube,or_cube)

long int *cube, *or_cube;
{
  long int
	*end_of_cube;		/* pointer to end of cube */

  end_of_cube = cube + total_length;
  cube = cube + input_length;
  or_cube = or_cube + input_length;

  for(; cube < end_of_cube ;)
   { *or_cube = *cube | *or_cube;
     cube++;
     or_cube++;
   }
}

/************************************************************************/

#ifdef DEBUG

or_output_list(list,or_cube)

long int *or_cube;
struct cube_list *list;
{
  set_output_word(or_cube,mask00);
  for(; list != NULL ; list = list->next_cube) or_output(list->cube,or_cube);
}

#endif

/**********************************************************************/

and_output(cube,and_cube)

long int *cube, *and_cube;
{
  long int
	*end_of_cube;		/* pointer to end of cube */

  end_of_cube = cube + total_length;
  cube = cube + input_length;
  and_cube = and_cube + input_length;

  for(; cube < end_of_cube ;)
   { *and_cube = *cube & *and_cube;
     cube++;
     and_cube++;
   }
}

/************************************************************************/

#ifdef DEBUG

and_output_list(list,and_cube)

long int *and_cube;
struct cube_list *list;
{
  set_output_word(and_cube,mask11);
  for(; list != NULL ; list = list->next_cube) and_output(list->cube,and_cube);
}

/*****************************************************************************/

invert_output(cube)

long int *cube;
{
  long int
	*end_of_cube;		/* pointer to end of the cube */

  end_of_cube = cube + total_length;
  cube += input_length;
  for(; cube < end_of_cube ; cube++) *cube = ~(*cube);
}

/******************************************************************************/

int bigger_output(cube1,cube2)

long int *cube1, *cube2;
{
  long int 
	*end_of_cube;		/* pointer to end of cube */

  end_of_cube = cube1 + total_length;   
  cube1 += input_length;
  cube2 += input_length;
  for(; cube1 < end_of_cube ;)
   { if((*cube1 & *cube2) != *cube1) return(1);
     cube1++;
     cube2++;
   }
  return(0);
}

#endif

/*************************************************************************/

int empty_output(cube)

long int *cube;
{
  long int
	*end_of_cube;		/* pointer to end of cube */

  end_of_cube = cube + total_length;
  cube += input_length;
  for(; cube < end_of_cube ; cube++)
   { if(*cube != 0) return(0);
   }
  return(1);
}

