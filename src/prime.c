/*********************************************************************

NAME
	prime_implicants_by_recursive_partitioning

PURPOSE
	Allocate some working vectors and call the recursive prime
	implicants function to generate the prime implicants of
	a boolean function. In fact this function is only an entry
	for the recursive process performed by the recursive function
	that will be called.

SYNOPSIS
	struct node *prime_implicants_by_recursive_partitioning(list)
	struct node *list;

DESCRIPTION
	We allocate a vector that contains all the input variables. Each
	time we partition the function along one input variable, we place
	this variable at the end of the vector and decrement the number of
	input of the partitioned function such that this input variable
	is no longer available to the partitioned function. So this function
	initializes the recursive process by allocating a vector containing
	all the input variables before calling the recursive function to
	generate the prime implicants.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	14 juillet 1984

AUTHOR
	Michel DAGENAIS

***********************************************************************/

#include "cubes.h"
#include <stdlib.h>

struct node
	*common_list,		/* list when converting a tree in a list */
	*dont_care_list,	/* list to put the dont care cover */
	**point_to_merge,	/* pointer to node to merge */
	*node_to_merge;		/* node that we currently try to merge */

struct binary
	*common_binary;		/* branch to merge with */

int
	*nb0_at_input,		/* number of cubes with 0 for each input */
	*nb1_at_input,		/* number of cubes with 1 for each input */
	*nbx_at_input,		/* number of cubes with x for each input */
	*unused_input,		/* input var not taken by partitions */
	nbinput;		/* number of input var not taken for partition*/

struct node
*prime_implicants_by_recursive_partitioning(list)

struct node *list;
{
  struct branch branch;	/* branch sent to the recursive function */

  struct node 
	*temp_node,	/* node in the list */
	**prev_list;	/* pointer to node in the list */

/* We first screen the list to see if any node has an output part 
   composed exclusively of 0; in such a case, this node is removed
   from the list.						*/

  prev_list = &list;
  for(; (temp_node = *prev_list) != NULL ;)
   { if(empty_output(temp_node->cube))
      { *prev_list = temp_node->next_node;
        free_node(temp_node);
      }
     else prev_list = &(temp_node->next_node);
   }

/* we first allocate integer vectors to contain the list of input var,
   and for each passible code (0, 1 or x) a working vector used in
   the input var selection for partitioning.			*/

  unused_input = (int *)calloc((unsigned)input_number,sizeof(int));
  nb0_at_input = (int *)calloc((unsigned)input_number,sizeof(int));
  nb1_at_input = (int *)calloc((unsigned)input_number,sizeof(int));
  nbx_at_input = (int *)calloc((unsigned)input_number,sizeof(int));

  if(unused_input == NULL || nb0_at_input == NULL || nb1_at_input == NULL ||
     nbx_at_input == NULL) fatal_system_error("unable to alloc vector for inputs");

/* we initialize unused_input with the list of input var which is
   0 to (input_number - 1) when we start here with the unpartitioned
   function. nbinput is initialized to input_number the number of input
   variables available in the vector unused input before we start the 
   partitioning.							*/

  for(nbinput = 0 ; nbinput < input_number ; nbinput++)
   { unused_input[nbinput] = nbinput;
   }
  dont_care_list = NULL;

/* we now call the recursive function that will return the list of prime
   implicants of the function described by the list of disjoint cubes sent */

  branch.status = LEAF;
  branch._.leaf = list;
  recursive_prime_implicants(&branch,1);

/* the nodes in the dont care cover were kept in a different list, we will
   return them in the same list as the prime implicants.		*/

  list = merge_node_lists(dont_care_list,branch._.leaf); 

/* we now simply free the vectors allocated, the job is done */

  flush_binary();
  free((char *)unused_input);
  free((char *)nb0_at_input);
  free((char *)nb1_at_input);
  free((char *)nbx_at_input);

  return(list);
}

/**********************************************************************

NAME
	recursive_prime_implicants

PURPOSE
	Generate the list of prime implicants of a boolean function from
	a list of disjoint cubes. These prime implicants will be linked
	together in a covering graph.

SYNOPSIS
	recursive_prime_implicants(branch,return_a_list)
	struct branch *branch;
	int return_a_list;

DESCRIPTION
	This function generates the prime implicants using a recursive
	partitioning up to unate functions. The partitions are then merged
	together and the new implicants formed are checked for absorbtion.
	Each partition is performed on an input variable, all the cubes
	with a 0 go on one side, all the cubes with a 1 on the other and
	the cubes with a x go on both sides. We then make both sides prime
	and we finally merge both together along the partitioning variable.
	The parameter return_a_list tells that the 0 side of the tree should
	be returned in a list instead of a subtree.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	13 july 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

recursive_prime_implicants(branch,return_a_list)

struct branch *branch;
int return_a_list;
{
  int
	current_code,		/* code for the node at current_input */
	current_input;		/* input variable selected */

  struct node 
	*temp_node,		/* temp node to place in the list 1 or 0 */
	*start_list1,		/* list of nodes with 1 in current input */
	*start_list0,		/* list of nodes with a 0 in current input */
	*next_node,		/* next node in the list to partition */
	*list;			/* list of nodes received */

  struct binary 
	*temp_binary;		/* new node for this partition */

/* we select the input for which we will partition. A negative value tells
   that only one cube is left in the list and that no partitioning is needed
   any more; in this case the only cube in this partition is necessarily prime
   and the function terminates here.					*/

 
  current_input = select_input(&(branch->_.leaf));
  if(current_input < 0) return;
  list = branch->_.leaf;

/* we need to partition; the input is already selected, all the cubes with
   a 0 for this input will be put in start_list0, all the cubes with a 1
   in start_list1. The cubes with a x for this input will be splitted in
   two cube, one with a 0 to put in start_list 0 and one with a 1 to put
   in start_list1.							*/

  define_current_var(current_input);
  start_list0 = NULL;
  start_list1 = NULL;

  for(temp_node = list ; temp_node != NULL ; temp_node = next_node)
   { next_node = temp_node->next_node;
     current_code = extract_current_var(temp_node->cube);
     switch(current_code)
      { case _1_ : 

/* we have a 1 for the selected input place the cube in start_list1 */

	   temp_node->next_node = start_list1;
	   start_list1 = temp_node;
	   break;

        case _0_ :

/* we have a 0 for the selected input place the cube in start_list0 */

	   temp_node->next_node = start_list0;
	   start_list0 = temp_node;
	   break;

	case _X_ :

/* we have a x split the cube and place it in both lists */

	   temp_node->next_node = start_list0;
	   start_list0 = temp_node;
	   temp_node = copy_and_alloc_node(temp_node->cube);
	   temp_node->next_node = start_list1;
	   start_list1 = temp_node;

	/* place respectively 0 and 1 in the splitted cubes for current_input*/

	   set_current_var(start_list0->cube,mask10);
	   set_current_var(start_list1->cube,mask01);
	   break;
      }    
   }

/* for each partition we generate the prime implicants by calling the
   recursive function.							*/

  temp_binary = alloc_binary();
  temp_binary->var = current_input;
  temp_binary->is0.status = LEAF;
  temp_binary->is0._.leaf = start_list0;
  temp_binary->is1.status = LEAF;
  temp_binary->is1._.leaf = start_list1;
  if(return_a_list) recursive_prime_implicants(&(temp_binary->is0),1);
  else recursive_prime_implicants(&(temp_binary->is0),0);
  recursive_prime_implicants(&(temp_binary->is1),0);

  nbinput++;
  define_current_var(current_input);

/* we have two lists of cubes prime in their respective partition, when we will
   put the list together they will still be prime except maybe for the input
   var along which they were partition. For this reason, we will try the star 
   product between each cube of a partition with each cube of the other 
   partition. The new cubes formed may absorb the cubes used for their formation
   and the new cubes must also be checked for absorbtion with the other new
   cubes.								*/

  common_binary = temp_binary;
  scan_node_to_merge(&(temp_binary->is0));

/* if we have to return a list, we simply merge the subtrees in one list */

  if(return_a_list)
   { common_list = merge_node_lists(temp_binary->isx,temp_binary->is0._.leaf);
     put_in_common_list(&(temp_binary->is1));
     free_binary(temp_binary);
     branch->_.leaf = common_list;
     if(VERBOSIS)
      { sprintf(error_buffer,"leftmost subtree merged at level %d (%d)",
		input_number - nbinput,nb_alloc_nodes - 1);
	send_user_message(error_buffer);
      }
     return;
   }

/* both subtrees are empty, we can then simply return a list and release the
   binary structure that holds the subtrees.				*/

  if(temp_binary->is0.status == 0 && temp_binary->is1.status == 0)
   { branch->status = LEAF;
     branch->_.leaf = temp_binary->isx;
     free_binary(temp_binary);
   }
  else
   { branch->status = SUBTREE;
     branch->_.subtree = temp_binary;
   }
  if(VERY_VERBOSIS)
   { sprintf(error_buffer,"subtree merged at level %d (%d)",
	     input_number - nbinput,nb_alloc_nodes - 1);
     send_user_message(error_buffer);
   }
}

/**********************************************************************

NAME
	scan_node_to_merge

PURPOSE
	Find all the nodes in a branch and try to merge them with the rest
	of the nodes that are in the other side of the common branch.

SYNOPSIS
	scan_node_to_merge(branch)
	struct branch *branch;

DESCRIPTION
	When we start the merging at a binary node, we scan to merge all the
	nodes in the 0 branch and we try to merge them with the nodes in the
	1 branch. The 1 branch for this purpose can be accessed from the common
	binary. When the branch points to a leaf, we take all the nodes in the
	leaf; when it points to a subtree, we take all the nodes in the isx
	list and scan recursively the 0 and 1 branches.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	8 august 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

scan_node_to_merge(branch)

struct branch *branch;
{
  struct binary *temp_binary;		/* temp pointer to a subtree */
  int status;

  status = branch->status;
  if(status == 0) return;
  if(status & SUBTREE)
   { temp_binary = branch->_.subtree;
     scan_node_to_merge(&(temp_binary->is0));
     scan_node_to_merge(&(temp_binary->is1));
     point_to_merge = &(temp_binary->isx);
   }
  else point_to_merge = &(branch->_.leaf);

/* all the nodes in the list to merge will be star produc with all the other
   nodes in the 1 branch of the common_binary. When no merge covers the node
   to merge we take the next node in the list. If the node is covered, the
   point_to_merge is already updated to point to the next node	*/

  for(; (node_to_merge = *point_to_merge) != NULL ;)
   { if(merge_with_rest(&(common_binary->is1)) == 0) 
			point_to_merge = &(node_to_merge->next_node);
   }
  
  /* We look if this branch of the tree is not empty in which case we cut
     it to help restrict the search further on.				*/

  if(status & SUBTREE)
   { if(temp_binary->is0.status == 0 && temp_binary->is1.status == 0)
      { if(temp_binary->isx == NULL) branch->status = 0;
	else
	 { branch->status = LEAF;
	   branch->_.leaf = temp_binary->isx;
	 }
	free_binary(temp_binary);
      }
   }
  else if(branch->_.leaf == NULL) branch->status = 0;
}

/***********************************************************************

NAME
	merge_with_rest

PURPOSE
	We scan the specified branch to find some nodes that might merge
	with the node_to_merge. The candidates are merged and placed in the
	graph.

SYNOPSIS
	int merge_with_rest(branch)
	struct branch *branch;

DESCRIPTION
	If the branch points to a leaf, we try to merge the nodes in it with
	the current node to merge. If it points to a subtree, we try to merge
	the nodes in the isx list and recursively merge with the 0 and 1
	branches. When a merge is successfull, the new node is placed in the
	graph and we check that it is not absorbed by the nodes already in
	the common_binary in the isx list where the new nodes merged are put.
	When the node to merge is absorbed, we remove it from the list to
	merge, update the pointer to merge and return 1; when the node to merge
	is absorbed we know that no other merge will give a bigger cube and
	it is why we stop the merging for that node. Otherwise, we return 0.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	8 august 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

int merge_with_rest(branch)

struct branch *branch;
{
  struct node
	*temp_node,		/* temp pointer in the list to merge */
	**point_temp_node,	/* pointer to temp_node */
	*temp_nodex,		/* temp pointer in the list to absorb */
	**point_to_nodex,	/* pointer to temp_nodex */
	*new_node;		/* pointer to the result of the merge and link*/

  struct binary *temp_binary;		/* pointer to the subtree of branch */

  int 
	status,				/* temp storage for the branch status */
	code;			/* value of the var for the node to merge */

  status = branch->status;
  if(status == 0) return(0);
  if(status & SUBTREE)
   { temp_binary = branch->_.subtree;
     point_temp_node = &(temp_binary->isx);
   }
  else point_temp_node = &(branch->_.leaf);

/* we will try to merge the node with all the nodes in the list	*/

  for(; (temp_node = *point_temp_node) != NULL ;)
   { 
#ifdef DEBUG
     foutput_cube(stdout,node_to_merge->cube);
     foutput_cube(stdout,temp_node->cube);
     printf("Above two cubes to merge");
#endif

     if(star_product(node_to_merge->cube,temp_node->cube,spare_node->cube))
      { new_node = merge_and_link(point_to_merge,point_temp_node,spare_node);

#ifdef DEBUG
     printf(", merge successfull gives:\n");
     foutput_cube(stdout,new_node->cube);
#endif
	
/* a merge occured, we will see if the new node is absorbed by nodes already
   merged and put in the listx of the common binary.			*/

	point_to_nodex = &(common_binary->isx);
	for(; (temp_nodex = *point_to_nodex) != NULL ;)
	 { code = absorb(new_node->cube,temp_nodex->cube);

#ifdef DEBUG
           printf("Absorbtion code: %d \n",code);
#endif	   
           switch(code)
	    { case 0 :
		absorb_and_unlink(temp_nodex,&new_node);
		break;
	      case 1 :
		absorb_and_unlink(new_node,point_to_nodex);
		break;
	      case 2 :
		absorb_and_unlink(temp_nodex,&new_node);
		break;
	    }
	   if(new_node == NULL) break;
	
/* if the node in the listx was not absorbed, we update the point_to_nodex */ 

	   if(temp_nodex == *point_to_nodex) 
				point_to_nodex = &(temp_nodex->next_node);
	 }

/* the new node was not absorbed, it will be put in the listx.	*/

	if(new_node != NULL)
	 { new_node->next_node = common_binary->isx;
	   common_binary->isx = new_node;
	 }

/* the node to merge was absorbed, it is the best possible merge and we stop */ 

	if(*point_to_merge != node_to_merge) return(1); 
      }
     if(temp_node == *point_temp_node)point_temp_node = &(temp_node->next_node);
   }

/* We will now merge recursively with the subtrees of this branch. If the
   node to merge has a 0 for the variable of this binary it can only merge
   with the subtree 0 and the same if it has 1 with the subtree 1; this way
   important savings on useless attempt to star product, are achieved.	*/

  if(status & SUBTREE)
   { code = extract_var(node_to_merge->cube,temp_binary->var);

     switch (code)
      { case _0_ :
	  if(merge_with_rest(&(temp_binary->is0))) return(1);
	  break;
	case _1_ :
	  if(merge_with_rest(&(temp_binary->is1))) return(1);
	  break;
	case _X_ :
	  if(merge_with_rest(&(temp_binary->is0))) return(1);
	  if(merge_with_rest(&(temp_binary->is1))) return(1);
	  break;
      }

/* all the merging are done, we will now look if this branch is not empty
   in which case we will cut it from the tree.				*/

     if(temp_binary->is0.status == 0 && temp_binary->is1.status == 0)
      { if(temp_binary->isx == NULL) branch->status = 0;
	else
	 { branch->status = LEAF;
	   branch->_.leaf = temp_binary->isx;
	 }
	free_binary(temp_binary);
      }
   }
  else if(branch->_.leaf == NULL) branch->status = 0;
  return(0);
}

