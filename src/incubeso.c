#include "cubes.h"

/****************************************************************************

NAME
	star_product

PURPOSE
	This function generates the biggest cube that can be created
	from two adjacent cubes. The variable for which adjacency is
	checked is predefined as the current_var.

SYNOPSIS
	int star_product(cube1,cube2,result_cube)
	long int *cube1,*cube2,*result_cube;

DESCRIPTION
	The two cubes are used to form the biggest possible cube out of them.
	If they are not adjacent for the current_var or if their output 
        are disjoint, no cube can be formed and the value 0 is returned.
	Otherwise 1 is returned.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

int star_product(cube1,cube2,result_cube)

long int *cube1,*cube2,*result_cube;
{
  long int
	result,		/* word containing the intersection */
	*end_input,	/* pointer to the word after the last word of input */
	*end_output,	/* pointer to the word after the last word of cube */
	*merged_word,	/* pointer to the word in which is the current var */
	intersect_mask; /* mask that contains 10 for intersecting var and 00 */
			/* for non-intersecting var */

  int common_output;

  merged_word = cube1 + current_word_index;
  end_input = cube1 + input_length;
  end_output = cube1 + total_length;

#ifdef DEBUG
  printf("cube1 %lx merged_word %lx current_mask11 %lx current_mask10 %lx\n",
         cube1,merged_word,current_mask11,current_mask10);
#endif

  for(; cube1 < end_input ; cube1++)

/*	we put the intersection in result_word		*/

   { result = *cube1 & *cube2;

/*	we check if the intersection is null on one variable	*/

     intersect_mask = ((result << 1) | result) & mask10;
     if(intersect_mask != mask10)

/*	the null intersection should be only on the current_var which means
	that the two cubes are adjacent along that variable.
	If the null part is for another variable, 0 is returned and no merging
	is possible along that variable. If the merging can be performed, that
	part becomes an x.					*/

      { if((intersect_mask | current_mask10) != mask10)return(0);
	if(cube1 != merged_word) return(0);
        result |= current_mask11;
      }
     *result_cube = result;
     cube2++;
     result_cube++;
   }

/* the input of the new cube is formed, we must now check if its output is
   empty, and if not compute it */

  common_output = 0;
  for(; cube1 < end_output ; cube1++)
   { *result_cube = *cube1 & *cube2;
     if(*result_cube != 0L) common_output = 1;
     cube2++;
     result_cube++;
   }
  if(common_output) return(1);
  else return(0);
}

/****************************************************************************

NAME
	covers, covers_input, absorb, covers_list

PURPOSE
	This function tells you if a cube covers completely another one.

SYNOPSIS
	int covers(cube1,cube2)
	long int *cube1,*cube2;

        int covers_input(cube1,cube2)
	long int *cube1, *cube2;

	int absorb(cube1,cube2)
	long int *cube1,*cube2;

	int covers_list(cube,list)
	long int *cube;
	struct cube_list *list;

DESCRIPTION

    -covers returns 1 if cube1 covers cube2 and 0 otherwise.

    -covers_input returns 1 if the input of cube1 covers the input of cube2. 

    -absorb returns -1 if nobody covers nobody, 0 if cube1 and cube2 are equal,
	1 if cube1 covers cube2 and 2 if cube2 covers cube1.

    -covers_list returns 1 if cube covers all the cube in the list and 0 
	otherwise.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

int covers(cube1,cube2)

long int *cube1,*cube2;
{
  long int *end_of_cube; /* pointer to the word after the last of cube*/

  end_of_cube = cube1 + total_length;

  for(; cube1 < end_of_cube ; cube1++)
   { if((*cube1 & *cube2) != *cube2 ) return(0);
     cube2++;
   }
  return(1);
 }

/**********************************************************************/

#ifdef DEBUG

int covers_input(cube1,cube2)

long int *cube1,*cube2;
{
  long int *end_of_input; /* pointer to the word after the last of cube*/

  end_of_input = cube1 + input_length;

  for(; cube1 < end_of_input ; cube1++)
   { if((*cube1 & *cube2) != *cube2 ) return(0);
     cube2++;
   }
  return(1);
 }

#endif

/***********************************************************************/

int absorb(cube1,cube2)

long int *cube1,*cube2;
{
  long int 
	*end_of_cube,	/* pointer to the word after the last of cube */
	and_cubes;	/* intersection of the two cubes */

  int 
	cube1_covers,	/* logic variable tells if 1 covers 2 up to now */
	cube2_covers;	/* logic variable tells if 2 covers 1 up to now */

  end_of_cube = cube1 + total_length;

  cube1_covers = 1;
  cube2_covers = 1;

  for(; cube1 < end_of_cube ; cube1++)
   { and_cubes = *cube1 & *cube2;
     if(and_cubes != *cube1)
      { if(cube1_covers == 0) return(-1);
        else cube2_covers = 0;
      }
     if(and_cubes != *cube2)
      { if(cube2_covers == 0) return(-1);
        else cube1_covers = 0;
      }
     cube2++;
   }
  if(cube1_covers)
   { if(cube2_covers) return(0);	/* both cubes are equal */
     else return(1);	/* cube 1 covers cube 2 */
   }
  else return(2);	/* cube 2 covers cube 1 */
}

/********************************************************************/

int covers_list(cube,list)

long int *cube;
struct cube_list *list;
{
  for(; list != NULL ; list = list->next_cube)
   { if(covers(cube,list->cube) == 0) return(0);
   }
  return(1);
}

/****************************************************************************

NAME
	intersect, intersect_list

PURPOSE
	These functions check if two cubes do intersect.

SYNOPSIS
	int intersect(cube1,cube2)
	long int *cube1,*cube2;

	int intersect_list(cube,list)
	struct cube_list *list;
	long int *cube;

DESCRIPTION

    -intersect returns 1 if cube1 and cube2 do intersect and 0 otherwise.

    -intersect_list returns 1 if cube1 intersects with any cube in list and 0
	otherwise.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

int intersect(cube1,cube2)

long int *cube1,*cube2;
{
  long int 
	*end_of_input,		/* pointer to word after input of cube */
	*end_of_output,		/* pointer to word after output of cube */
	result;			/* intersection of the cubes */

  end_of_input = cube1 + input_length;
  end_of_output = cube1 + total_length;

  for(; cube1 < end_of_input ; cube1++)
   { result = *cube1 & *cube2;

/*	we want to detect if the intersection is null for any variable */

     if((((result << 1) | result ) & mask10) != mask10) return(0);
     cube2++;
   }
  for(; cube1 < end_of_output ; cube1++)
   { if((*cube1 & *cube2) != 0L) return(1);
     cube2++;
   }
  return(0);
}

/*************************************************************************/

int intersect_list(cube,list)

long int *cube;
struct cube_list *list;
{
  for(; list != NULL ; list = list->next_cube)
   { if(intersect(cube,list->cube)) return(1);
   }
  return(0);
}

/****************************************************************************

NAME
	disjoint_sharp

PURPOSE
	This function takes a list of disjoint cubes and sharp from it
	another cube. The list is updated to contain the result.

SYNOPSIS
	int disjoint_sharp(list,cube)
	long int *cube;
	struct cube_list **list;

DESCRIPTION
	the function takes each cube in the list, see if the other cube
	intersect with it. If so a new list is computed such that it is
	disjoint from the cube but the union of the cube and the new
	list has the same coverage as with the old list and the cube. It is the 
	disjoint sharp operation as defined by Hong and Opstako for
	their MINI program. The implementation is different however.
	The function returns 1 when the list is empty after the sharp.
	Otherwise the function returns 0.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

int disjoint_sharp(list,cube)

long int *cube;
struct cube_list **list;
{
  struct cube_list 
	*present_cube_list,	/*pointer to the cube_list element considered*/
	**previous_cube,	/*pointer to the next_cube element of previous*/
				/* cube, useful when we want to delete an */
				/* element in the list */
	*temp_pointer;		/* temporary pointer when a cube is splitted */

  long int 
	*end_input,		/* pointer to end of input of a cube */
	*end_output,		/* pointer to end of output of a cube */
	*present_cube,		/* pointer to the cube considered */
	*sharping_cube,		/* pointer to the cube we sharp from the list*/
	result,			/* contains the intersection of two cubes */
	mask_var;		/* mask one variable in a long int */

  previous_cube = list;

  end_input = cube + input_length;
  end_output = cube + total_length;

  for(; (present_cube_list = *previous_cube) != NULL ;)
   { present_cube = present_cube_list->cube;
   
/* If the two cubes do not intersect we simply go to the next cube in the
   list since there is nothing to sharp here.				*/

     if(intersect(cube,present_cube) == 0)
      { previous_cube = &(present_cube_list->next_cube);
	continue;
      }

     for(sharping_cube = cube ; sharping_cube < end_input ; sharping_cube++)

/*	we compute the intersection of the two cubes and see for which 
	variables it is smaller than the cube from which we want to remove the
	intersection 							*/

      { result = (*sharping_cube & *present_cube) ^ *present_cube;
        if(result != NULL)

/*	the intersection does not cover all the cube we will have to split it
	for all the variables for which we have X in the cube in the list and
	not X for the sharping cube					*/

         { for(mask_var = 3 ; mask_var != NULL ; mask_var = mask_var << 2)

            { if(result & mask_var)

/*	We split the cube in two by allocating a new cube and giving the value
	1 to a cube and 0 to the other for the input variable of the cube 
	which was x. One cube will be disjoint from the sharping cube and
	skipped in the list while the other will intersect and is smaller
	such that by repetitive splitting, we will end with a cube completely
	covered by the sharping cube; for this last cube the output common 
	with the sharping cube will be reset to 0 as a result of the sharp
	operation.							*/

               { temp_pointer = present_cube_list->next_cube;
                 present_cube_list->next_cube =
		 copy_and_alloc_cube_list(present_cube_list->cube);

	         *present_cube = ~(mask_var & *sharping_cube) & *present_cube;

		 present_cube = (present_cube - present_cube_list->cube)
					+ (present_cube_list->next_cube)->cube;

		 previous_cube = &(present_cube_list->next_cube);
		 present_cube_list = present_cube_list->next_cube;
		 present_cube_list->next_cube = temp_pointer;
                 *present_cube = ((~mask_var) | *sharping_cube) & *present_cube;
	       }
	    }
	 }
	present_cube++;
      }

/*	If all the output bits are reset to 0 as result of the sharp
	operation, the last cube becomes empty and is removed from the list.
	After we go to process the next cube in the list		*/

     result = NULL;
     for(; sharping_cube < end_output;)
      { *present_cube = *present_cube & (~ *sharping_cube);
        if(*present_cube) result = 1;
        sharping_cube++;
        present_cube++;
      }
     if(result == NULL)
      { temp_pointer = present_cube_list->next_cube;
        free_cube_list(present_cube_list);
        present_cube_list = temp_pointer;
	*previous_cube = temp_pointer;
      }
     else
      { previous_cube = &(present_cube_list->next_cube);
      }
   }
  if(*list == NULL) return(1);
  return(0);
}

