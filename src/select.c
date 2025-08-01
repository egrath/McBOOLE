#include "cubes.h"

/************************************************************************

NAME
	max

SYNOPSIS
	int max(i1,i2)
	int i1,i2;

************************************************************************/

int max(i1,i2)

int i1,i2;
{
  return(i1 > i2 ? i1 : i2);
}

/*************************************************************************

NAME
	select_input

PURPOSE
	The prime implicants are generated using a partioning algorithm
	and this function selects on which input this partition should
	be done, using heuristics.

SYNOPSIS
	int select_input(point_list)
	struct node **point_list;

DESCRIPTION
	for each input not already selected in this subtree count the
	number of 0 1 and x in the cubes. Then the heuristic for the
	selection will use this information. We want to minimize the 
	number of cubes duplicated in both subtrees created by the
	partition, so we will select the input with the least number
	of x. When we are left with only one cube in the partition
	we return -1 instead of an input number. If all the remaining
	cubes are intersecting, an error is generated if the user asked
	for disjoint input cubes by having disjoint_required at 1.
	Otherwise, if all the remaining cubes have the same input,
	we simly combine all their output to form only one cube;
	at output the 1 are the strongest and the d are the weakest.
	If an input is selected, its number is returned.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	12 july 1984

AUTHOR
	Michel DAGENAIS

***************************************************************************/

int select_input(point_list)

struct node **point_list;
{
   struct cube_list
	*or_cube;	/* cube used when many cubes are intersecting */
   
   struct node 
	*list,		/* pointer to the list received */
	*temp_node;	/* temp pointer to nodes in the list */

   int
	i,		/* loop index for inputs */
	node_number,	/* number of nodes in the list */
	best_input,	/* input selected */
        current_balance, /*number of 0 or 1 whichever is bigger for cur. input*/
        best_balance,	/* number of 0 or 1 whichever is bigger for best input*/
	cost,		/* cost of an input */
	best_cost;	/* cost of the input selected */

/* before anything else we will check if the list contains only one cube
   in which case no partitioning is needed.				*/

   list = *point_list;
   if(list->next_node == NULL) 
    { if(detect_dont_care(list->cube)) put_in_dont_care_list(list);
      list->status = BASIC;
      return(-1);
    }

/* we need partioning so the number of x, 0 and 1 for all the remaining inputs
   will be computed							*/

   for(i = 0 ; i < nbinput ; i++)
    { define_current_var(unused_input[i]);
      nb1_at_input[i] = 0;
      nb0_at_input[i] = 0;
      nbx_at_input[i] = 0;

      temp_node = list;
      for(; temp_node != NULL ; temp_node = temp_node->next_node)
       { switch(extract_current_var(temp_node->cube))

	  { case _1_ :
	      (nb1_at_input[i])++;
	      break;

	    case _0_ :
	      (nb0_at_input[i])++;
	      break;

	    case _X_ :
	      (nbx_at_input[i])++;
	      break;
	  }
       }
    }

   /* Now for all the remaining input we have the number of x, 0 and 1
      we can now decide which input to select. The heuristic is to pick the 
      input with the least number of x and, for this number of x, pick the 
      input with the most balanced number of 0 and 1.		*/

   best_cost = INFINITY;
   best_balance = INFINITY;
   node_number = nb0_at_input[0] + nbx_at_input[0] + nb1_at_input[0];

   for(i = 0 ; i < nbinput ; i++)
    { cost = nbx_at_input[i];
      current_balance = max(nb0_at_input[i],nb1_at_input[i]);
      if(cost != node_number && current_balance != node_number)
       { if(cost < best_cost || 
		(cost == best_cost && current_balance < best_balance))
          { best_balance = current_balance;
            best_cost = cost;
	    best_input = i;
	  }
       }
    }  

/* If no input was selected, all the nodes are the same for the input part.
   If the user specified that disjoint cubes are to be accepted only, a
   message will be issued. Otherwise, we will combine the output of all the
   cubes; the 1 at output are the strongest, the 0 second and the 0 last   */

   if(best_cost == INFINITY)
    { if(DISJOINT_REQUIRED)
       { foutput_cube(stderr,list->cube);
	 fatal_user_error("the part above is present in many cubes");
       }
      or_cube = copy_and_alloc_cube_list(list->cube);
      temp_node = list->next_node;
      for(; temp_node != NULL ; temp_node = temp_node->next_node)
       { or_output(temp_node->cube,or_cube->cube);
       }

/* all the cubes had the same input part so we computed a new cube out of 
   them. The old list is freed and the new cube is placed in the list.
   Since we are left with only one cube, no partitioning is needed and
   we return -1.						*/

      free_list_of_nodes(point_list);
      *point_list = copy_and_alloc_node(or_cube->cube);
      free_cube_list(or_cube);
      list = *point_list;
      if(detect_dont_care(list->cube)) put_in_dont_care_list(list);
      list->status = BASIC;
      return(-1);
    }

/* the best input is selected, we will return it; also we place it at the
   end of the unused_input vector because for the next partition the number
   of unused input will be decreased by one and this input will not be 
   accessible any more as an unused input				*/

   nbinput--;
   i = unused_input[best_input];
   unused_input[best_input] = unused_input[nbinput];
   unused_input[nbinput] = i;
   return(i);
}

/*************************************************************************

NAME
	put_in_dont_care_list

PURPOSE
	When a node has some dont care bits at output, the dont care part
	is put in a special list and a link makes this new node an ancestor
	of the original node with the dont care.

SYNOPSIS
	put_in_dont_care_list(node)
	struct node *node;

DESCRIPTION
	A node with dont care has its dont care part put in the dont care list
	and the dont care bits of the original node are then converted to 1.
	The dont care node is put as an ancestor of the original node.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA

AUTHOR
	Michel DAGENAIS

***************************************************************************/

put_in_dont_care_list(node)

struct node *node;
{
  struct parent *parent;
  
  struct node *temp_node;

  temp_node = copy_and_alloc_node(node->cube);
  remove_do_care(temp_node->cube);
  change_dont_to_do_care(node->cube);
  temp_node->next_node = dont_care_list;
  dont_care_list = temp_node;
  temp_node->status = DONT_CARE;
  parent = alloc_parent();
  parent->next_parent = temp_node->descendants;
  temp_node->descendants = parent;
  parent->parent = node;
  parent = alloc_parent();
  parent->next_parent = node->ancestors;
  node->ancestors = parent;
  parent->parent = temp_node;
}

/************************************************************************

NAME
	put_in_common_list

PURPOSE
	The branch received is scanned for the nodes it contains; all the
	nodes in the branch are put in the common list. This function is
	useful to convert a tree structure to store the nodes into a list
	of nodes.

SYNOPSIS
	put_in_common_list(branch)
	struct branch *branch;

DESCRIPTION
	If the branch points to a list of nodes, they are put in the common
	list. If it points to a new binary node, the isx list is put in the
	common list and the 0 and 1 branches are recursively put in the
	common list.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	8 august 1984

AUTHOR
	Michel DAGENAIS

***************************************************************************/

put_in_common_list(branch)

struct branch *branch;
{
  if(branch->status == 0)return;
  if(branch->status & SUBTREE)
   { put_in_common_list(&(branch->_.subtree->is0));
     put_in_common_list(&(branch->_.subtree->is1));
     common_list = merge_node_lists(branch->_.subtree->isx,common_list);
     free_binary(branch->_.subtree);
     return;
   }
  common_list = merge_node_lists(branch->_.leaf,common_list);
}

