#include "cubes.h"

/****************************************************************************

NAME
	merge_node_lists, merge_cube_lists

PURPOSE
	Take two node lists and return all the nodes in a single list

SYNOPSIS
	struct node *merge_node_lists(list1,list2)
	struct node *list1, *list2;

	struct cube_list *merge_cube_lists(list1,list2)
	struct cube_list *list1, *list2; 

DESCRIPTION
	We first go to the end of the first list and then put as the next
	node the second list. This way the two lists are now linked and
	the beginning of the new list is the beginning of the first list
	The address of the new list is returned.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	8 august 1984

AUTHOR
	Michel DAGENAIS

*************************************************************************/

struct node 
*merge_node_lists(list1,list2)

struct node *list1, *list2;
{
  struct node *temp_node;	/* temp pointer in the first list */

  if(list1 == NULL) return(list2);
  temp_node = list1;
  for(; temp_node->next_node != NULL ; temp_node = temp_node->next_node);
  temp_node->next_node = list2;
  return(list1);
}

/****************************************************************************/

#ifdef DEBUG

struct cube_list  
*merge_cube_lists(list1,list2)

struct cube_list *list1, *list2;
{
  struct cube_list *temp_cube;	/* temp pointer in the first list */

  if(list1 == NULL) return(list2);
  temp_cube = list1;
  for(; temp_cube->next_cube != NULL ; temp_cube = temp_cube->next_cube);
  temp_cube->next_cube = list2;
  return(list1);
}

#endif

/***************************************************************************

NAME
	copy_and_alloc_node, copy_and_alloc_cube_list, copy_cube,
	duplicate_cube_list

PURPOSE
	These functions copy the corresponding structure, 
	and allocate the proper space.

SYNOPSIS
	struct node *copy_and_alloc_node(cube)
	long int *cube;

	struct cube_list *copy_and_alloc_cube_list(cube)
	long int *cube;

	copy_cube(cube_in,cube_out)
	long int *cube_in,*cube_out;

	struct cube_list *duplicate_cube_list(cube)
	struct cube_list *cube;

DESCRIPTION

    -copy_and_alloc_node allocates a new node and copy cube in the cube of
	the new node allocated. The address of the new node is returned.

    -copy_and_alloc_cube_list allocates a new cube and copy cube in the cube
	of the new structure cube_list allocated. The address of the new
	cube allocated is returned.

    -copy_cube simply copy the cube_in to the cube_out which should be
	previously allocated.

    -duplicate_cube_list allocates as many cube as there is in the cube list
	Cube and for each new cube allocated the content of the corresponding
	cube in the old cube list is copied. The address of the new list is
	returned. The difference between this function and the copy_and_alloc_
	cube_list is that this function copy and allocates many cubes to 
	duplicate a list of cubes as the other function simply copy and allocate
	only one cube ( of type cube_list).

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

struct node *copy_and_alloc_node(cube)

long int *cube;
{
  struct node *pointer;		/* pointer to new space allocated */

  pointer = alloc_node();
  copy_cube(cube,pointer->cube);
  return(pointer);
}

/***********************************************************************/

struct cube_list *copy_and_alloc_cube_list(cube)

long int *cube;
{
  struct cube_list *pointer;	/* pointer to new space allocated */

  pointer = alloc_cube_list();
  copy_cube(cube,pointer->cube);
  return(pointer);
}

/*************************************************************************/

struct cube_list *duplicate_cube_list(cube)

struct cube_list *cube;
{
  struct cube_list 
	*new_cube,		/* new cube allocated */
	*temp_cube;		/* temp pointer in the list of cubes */

  new_cube = NULL;
  for(; cube != NULL ; cube = cube->next_cube)
   { temp_cube = new_cube;
     new_cube = alloc_cube_list();
     copy_cube(cube->cube,new_cube->cube);
     new_cube->next_cube = temp_cube;
   }
  return(new_cube);
}

/*********************************************************************/

copy_cube(cube_in,cube_out)

long int *cube_in,*cube_out;
{
 long int *end_of_cube;

 end_of_cube = cube_in + total_length;
 for(; cube_in < end_of_cube ;)
  { *cube_out = *cube_in;
    cube_in++;
    cube_out++;
  }
}

