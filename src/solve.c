#include "cubes.h"
#include <stdlib.h>

/***********************************************************************

NAME
	find_best_covering

PURPOSE
	This function receives a list of cubes, describing a function,
	linked together in a graph. It produces a vector consisting
	of the best set of prime implicants that can cover the function.

SYNOPSIS
	find_best_covering(list)
	struct node *list;

DESCRIPTION
	This function puts all the prime implicants in a vector, finds
	the essential prime implicants and then finds the best
	set of cubes to cover the function. The nodes are ordered in
	the vector with the retained first.

COORDINATES
	McGill University Electrical Engineering MONTREAL CANADA
	17 july 1984

AUTHOR
	Michel DAGENAIS

*****************************************************************************/

struct node 
	**prime_nodes,		/* vector containing pointer to all cubes */
	**retained_nodes,	/* pointer to end of retained nodes */
	**unretain_nodes,	/* pointer to start of unretained covered node*/
	**end_prime,		/* pointer to end of prime vector */
	**start_stack,		/* pointer to start of stack */
	**end_stack,		/* pointer to end of stack */
	**current_in_stack;	/* after last node entered in stack */

struct cube_list
	*scanned_cube;	/* uncovered part of cube currently processed */

struct node
	*scanned_node,		/* node currently processed */
	*current_node;		/* node in the graph reached by the scan */

int 
	scan_count,		/* number of nodes scanned when we count them */
	prime_count,		/* number of cubes in prime nodes vector */
	branching_depth;	/* depth in recursion while branching */

unsigned long int
	ONE = 1,		/* a constant 1 unsigned long int */
	odd_pass_counter,	/* pass counter + 1 to identify scanning dir.*/
	pass_counter;		/* counts the number of scanning pass */


find_best_covering(list)

struct node *list;
{
  int
	prime_care,		/* number of non-dont care prime implicants */
	input_literal,		/* number of literals at input */
	output_literal;		/* number of literal at output */

  struct node **cursor;		/* pointer in the vector of nodes */
  
/* we will now compute the cost of the cubes in the list. The cost is defined
   as the number of non x inputs. Also a cube list is allocated for each node
   containing the uncovered part of the node; at the beginning, the whole node
   is uncovered and we simply copy it.					*/

  prime_count = 0;
  prime_care = 0;
  scanned_node = list;
  for(; scanned_node != NULL ; scanned_node = scanned_node->next_node)
   { scanned_node->cost = input_cost(scanned_node->cube);
     scanned_node->uncovered = copy_and_alloc_cube_list(scanned_node->cube);
     prime_count++;
     if((scanned_node->status & DONT_CARE) == 0) prime_care++;
   }

/* We will put in a vector pointers to all the nodes; When we return from the
   find cover the retained nodes are in the beginning and the unretained are at
   the end. When a cycle is encountered, the retained are first, then we have
   the uncovered nodes in the current partition, then the remaining uncovered
   nodes, then the unretained covered nodes. We keep the uncovered unretained
   nodes in the active part of the vector because their status may change and
   must be remembered when we perform branching.			*/

  prime_nodes = (struct node **)
			calloc((unsigned)prime_count,sizeof(struct node *));
  if(prime_nodes == NULL)fatal_system_error("unable to allocate prime_nodes");
  cursor = prime_nodes;
  scanned_node = list;
  for(; scanned_node != NULL ; scanned_node = scanned_node->next_node)
   { *cursor = scanned_node;
     cursor++;
   }
  retained_nodes = prime_nodes;
  unretain_nodes = prime_nodes + prime_count;
  end_prime = unretain_nodes;

/* we allocate the stack that will contain at any moment all the nodes 
   that might get decided. It will never contain more than all the nodes
   and so should never overflow.			*/

  start_stack = (struct node **)
			calloc((unsigned)prime_count,sizeof(struct node *));
  if(start_stack == NULL)fatal_system_error("unable to allocate stack");
  end_stack = start_stack + prime_count;
  current_in_stack = start_stack;

  sprintf(error_buffer,"The function has %d prime implicants",prime_care);
  if(VERBOSIS)send_user_dtime(error_buffer);
  send_file_dtime(error_buffer);

/* we first extract from the list the essential prime implicants */

  pass_counter = 0;

#ifdef CHECK
  check_graph();
#endif

  scan_count = 0;
  essential_prime_implicants();

  sprintf(error_buffer,"The function has %d essential PI",scan_count);
  if(VERBOSIS)send_user_dtime(error_buffer);
  send_file_dtime(error_buffer);

/* we are ready now to call the recursive function to find the best solution
   branching if needed. We have first to define that we are now at branching
   depth 0 here. Also we initialize the pass counter to 0 since all the flags
   of the nodes are at 0.						*/

  branching_depth = 0;
  max_branching_depth = 0;
  recursive_find_covering();


/* the solution is reached, the retained nodes in the final solution are in the
   prime nodes vector from prime nodes to retained nodes. The nodes from 
   unretain nodes to end prime are the unretained nodes. We will now put
   the output part of each of the retained cubes irredundant in the sense
   that no 1 at output can be removed without changing the the function. */

  make_output_sparse();
  sprintf(error_buffer,"The solution contains %d nodes",(int)(retained_nodes -
							    prime_nodes));
  if(VERBOSIS)send_user_dtime(error_buffer);
  send_file_dtime(error_buffer);

  input_literal = 0;
  output_literal = 0;
  cursor = prime_nodes;
  for(; cursor < retained_nodes ; cursor++)
   { scanned_node = *cursor;
     input_literal += input_cost(scanned_node->cube);
     output_literal += output_cost(scanned_node->cube);
   }	
  sprintf(error_buffer,"Final solution, literal : %d input %d output",
			  input_literal,output_literal);
  if(VERBOSIS)send_user_dtime(error_buffer);
  send_file_dtime(error_buffer);
}

/************************************************************************

NAME
	essential_prime_implicants

PURPOSE
	This function finds which of the prime implicants are essential
	using the graph algorithm.

SYNOPSIS
	essential_prime_implicants()

DESCRIPTION
	The vector of prime nodes is scannned. We look at the basic nodes
	to determine which are prime essential implicants. The prime
	essential implicants are then retained.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	16 july 1984

AUTHOR
	Michel DAGENAIS

***************************************************************************/

essential_prime_implicants()

{
  struct node **cursor;		/* pointer in the vector of nodes */ 
 
  struct parent *temp_parent;	/* temp parent in a parent list */

  cursor = retained_nodes;
  for(; cursor < unretain_nodes ; cursor++)
   { scanned_node = *cursor;

     if(scanned_node->status & DONT_CARE)
      { scanned_cube = NULL;
	retain_node();
	unretain_nodes--;
	*cursor = *unretain_nodes;
	*unretain_nodes = scanned_node;
	cursor--;
	continue;
      }

/* the implicant is not basic and is necessarily covered so we continue */

     if((scanned_node->status & BASIC) == 0) continue;

     scanned_node->status &= NOT_BASIC;
     scanned_cube = copy_and_alloc_cube_list(scanned_node->cube);
     temp_parent = scanned_node->ancestors;

/* we remove from cube the part covered by its ancestors */

     for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
      { (void)disjoint_sharp(&scanned_cube,temp_parent->parent->cube);
      }

     temp_parent = scanned_node->descendants;

/* we see if the remaining part is covered by its descendants */

     for(;temp_parent != NULL ;temp_parent = temp_parent->next_parent)
	 { if(disjoint_sharp(&scanned_cube,temp_parent->parent->cube))break;
	 }

/* The node is essential it will be retained because it is necessarily part
   of the optimal solution.						*/

     if(scanned_cube != NULL) 
      { scan_count++;

#ifdef CHECK 
	retain_check_node();
#else
	retain_node();
#endif
	scanned_node->status |= PRIME_ESSENTIAL;
	if(EPI_LIST) foutput_cube(stdout,scanned_node->cube);
      }
   }
}

/**********************************************************************

NAME
	recursive_find_covering

PURPOSE
	This function decides which of the undecided nodes should be
	retained and which should be unretained. If a cycle occurs,
	the remaining undecided nodes are partitioned and we will branch
	for each partition separately.

SYNOPSIS
	recursive_find_covering()

DESCRIPTION
	When a node is retained or unretained inferior, certain nodes will
	be affected because they might become covered or essential. These
	affected nodes are put on a stack. In this program the nodes in the
	stack are checked. When no more nodes are in the stack, it means
	that the remaining nodes were not affected by the nodes retained
	or unretained and that they form a cycle altogether. At that point
	we partition the cycles and branch for each of them.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	18 july 1984

AUTHOR
	Michel DAGENAIS

********************************************************************/

struct save_status
	{ struct node *node;		/* pointer to the node */
	  struct cube_list *uncovered;	/* copy of its uncovered part */
	  short int status;		/* copy of its status */
	};

static int 
	retain_cost,		/* cost of solution with branch node retained */
	unretain_cost,		/* cost of solution with node unretained */
	retain_count,		/* number of cubes in solution node retained */
	unretain_count,		/* number of cubes in solution node unretained*/
	temp_status;		/* temp storage for the status of a node */

struct save_status 
	*end_partition;		/* pointer to end of partition */

recursive_find_covering()

{
  struct save_status
	*cursor_partition,	/* pointer in the partition vector */
	*save_partition;	/* vector to store the status of the nodes */

  struct node
	**cursor,			/* pointer in the vector of nodes */
	**partition_nodes,		/* end of the current partition */
	**save_retained_nodes,		/* end of retained nodes */
	**save_unretain_nodes,		/* start of unretain covered nodes */
	*branching_node;		/* node choosen to branch on */

  int 
	partition_count;	/* number of nodes in save partition */

/* We scan all the affected nodes in the stack until it is empty, at this
   point either we have the final solution or we have a cycle */

  for(scanned_node = pop() ; ; scanned_node = pop())
   { if(scanned_node == NULL)

/* there is no more affected nodes in the stack, we either have a cycle
   or the final solution; We will first order the nodes in the vector
   to figure out how many nodes of each category we have.		*/

      { save_retained_nodes = retained_nodes;
	save_unretain_nodes = unretain_nodes;
	place_nodes_in_vector();
	if(VERBOSIS)
	 { retain_count = retained_nodes - save_retained_nodes;
	   unretain_count = save_unretain_nodes - unretain_nodes;
	   unretain_cost = unretain_nodes - retained_nodes;
	   retain_cost = unretain_cost - scan_count;
           unretain_cost = save_unretain_nodes - save_retained_nodes;
	   sprintf(error_buffer,
	   "%d %d retained, %d covered, %d inferior, %d undecided",
	   unretain_cost ,retain_count, unretain_count, retain_cost,scan_count);
	   send_user_message(error_buffer);
	 }

/* if only retained nodes and unretained nodes are present we have the answer */

        if(scan_count == 0) return;

/* We have a cycle, if the branching depth reached the limit, we will simply
   pick a node and continue in the loop.			*/

#ifdef CHECK 
        check_cycle();
#endif
	if(branching_depth >= depth_limit)
	 { select_node();
	   scanned_cube = NULL;
	   retain_node();
	   if(max_branching_depth < INFINITY) max_branching_depth = INFINITY;
	   continue;
	 }

/* We will break the loop to go and perform the branching */

	break;
      }

/* if the node was affected by a node being retained, it might get covered
   or inferior, we will check it here.				*/

     if(scanned_node->status & AFFECTED_RETAINED)
      { if(scanned_node->uncovered == NULL)
	 { scanned_node->status = DECIDED_COVERED;
	   continue;
	 }
	current_node = scanned_node;
	increment_pass_count();
	if(scan_inferior_ancestors()) continue;
	current_node = scanned_node;
	if(scan_inferior_descendants()) continue;
	scanned_node->status &= UNAFFECTED_RETAINED;
      }

/* If the node was affected by a node being unretained, it might now be
   essential, we will check it here.					*/

     if(scanned_node->status & AFFECTED_UNRETAIN)
      { current_node = scanned_node;
	increment_pass_count();
	scanned_cube = duplicate_cube_list(scanned_node->uncovered);
	if(scan_essential_ancestors())
	 { scanned_node->status &= UNAFFECTED_UNRETAIN;
	   continue;
	 }
	current_node = scanned_node;
	if(scan_essential_descendants())
	 { scanned_node->status &= UNAFFECTED_UNRETAIN;
	   continue;
	 }

/* The node is now essential we will retain it */

#ifdef CHECK
	retain_check_node();
#else
        retain_node();
#endif
      }
   }

/* the loop was brooken because we have a cycle and we want to branch.
   We will branch and solve the remaining undecided nodes partition
   by partition.						 */

  branching_depth++;
  if(branching_depth > max_branching_depth)
				max_branching_depth = branching_depth;

  for(; scan_count != 0 ;)
   { current_node = *retained_nodes;
     scan_count = 1;
     increment_pass_count();
     scan_partition();

     if(VERBOSIS)
      { sprintf(error_buffer,"partition of %d nodes at branching depth %d",
		          scan_count,branching_depth);
        send_user_message(error_buffer);
      }

/* All the nodes related to the first undecided nodes, and with each others
   all have the same pass count placed by the scan partition function. All
   these nodes will be put in the partition and their current status will
   be stored for branching.						*/

     partition_count = scan_count;
     save_partition = (struct save_status *)calloc((unsigned)partition_count,
						sizeof(struct save_status));
     if(save_partition == NULL)fatal_system_error("unable to alloc partition");
     cursor_partition = save_partition;
     cursor = retained_nodes;
     partition_nodes = retained_nodes;

     for(; cursor < unretain_nodes ;)
      { scanned_node = *cursor;
	if(scanned_node->count == pass_counter)
	 { cursor_partition->node = scanned_node;
	   cursor_partition->status = scanned_node->status;
	   cursor_partition->uncovered = 
				duplicate_cube_list(scanned_node->uncovered);
	   cursor_partition++;

	   if(partition_nodes == cursor)
	    { partition_nodes++;
	      cursor++;
	    }
	   else
	    { *cursor = *partition_nodes;
	      *partition_nodes = scanned_node;
	      partition_nodes++;
	    }
	 }
	else cursor++;
      }

/* Before calling the recursive function to solve the partition saved, we
   need to save some of the variables that will be affected during the
   recursive call. We set the unretain node pointer to the end of the
   partition so the recursive program called will only try to solve that
   part.							*/

     save_retained_nodes = retained_nodes;
     save_unretain_nodes = unretain_nodes;
     unretain_nodes = partition_nodes;

/* we will select one of the nodes in the partition for the branching */

     select_node();
     branching_node = scanned_node;
     if(VERBOSIS)send_user_message("a node is unretain for branching");
     unretain_inferior_node();

/* the node selected was unretained and will see what solution is produced */

     recursive_find_covering();

/* We will now store the solution and restore the old status to be able to
   try the alternate solution. For all the nodes in save partition, we exchange
   the stored uncovered part and the status with the current status of the
   node.								*/

     cursor_partition = save_partition;
     end_partition = save_partition + partition_count;

     for(; cursor_partition < end_partition ; cursor_partition++)
      { scanned_node = cursor_partition->node;
	scanned_cube = cursor_partition->uncovered;
	temp_status = cursor_partition->status;
	cursor_partition->status = scanned_node->status;
	cursor_partition->uncovered = scanned_node->uncovered;
	scanned_node->uncovered = scanned_cube;
	scanned_node->status = temp_status;
      }

/* We now have to reset the pointers in the prime node vector to their 
   original status and to retain the branching node to get the alternate
   solution.								*/

     retained_nodes = save_retained_nodes;
     unretain_nodes = partition_nodes;
     scanned_node = branching_node;
     scanned_cube = NULL;
     if(VERBOSIS)send_user_message("a node is retained for branching"); 
     retain_node();
     recursive_find_covering();

/* Having both solutions we can now compute their cost and compare them  */

     cursor_partition = save_partition;
     end_partition = save_partition + partition_count;
     retain_cost = 0;
     retain_count = 0;
     unretain_cost = 0;
     unretain_count = 0;

     for(; cursor_partition < end_partition ; cursor_partition++)
      { scanned_node = cursor_partition->node;
	if(scanned_node->status & RETAINED)
	 { retain_count++;
	   retain_cost += scanned_node->cost;
	 }
	if(cursor_partition->status & RETAINED)
	 { unretain_count++;
	   unretain_cost += scanned_node->cost;
	 }
      }

     if(retain_count == unretain_count)
      { if(retain_cost < unretain_cost) unretain_count = INFINITY;
	else if(retain_cost > unretain_cost) retain_count = INFINITY;
      }

/* If the solution with the branching node unretained is the best, we
   will have to restore the first solution.			*/

     cursor_partition = save_partition;

     if(retain_count > unretain_count)
      { if(VERBOSIS)send_user_message("the heuristic gave a worse solution");

	for(; cursor_partition < end_partition ; cursor_partition++)
         { scanned_node = cursor_partition->node;
   	   if(scanned_node->uncovered != NULL)
	    { free_list_of_cubes(&(scanned_node->uncovered));
	    }
	   scanned_node->uncovered = cursor_partition->uncovered;
	   scanned_node->status = cursor_partition->status;
         }
      }

/* If the solution with the branching node retained was better or equal,
   we simply have to free the space occupied by the save partition vector */

     else
      { if(VERBOSIS)
	 { if(retain_count < unretain_count)
	   send_user_message("the heuristic gave a better solution");
	   else send_user_message("both solutions were of equal cost");
         }

	for(; cursor_partition < end_partition ; cursor_partition++)
	 { if(cursor_partition->uncovered != NULL)
	    { free_list_of_cubes(&(cursor_partition->uncovered));
	    }
	 }
      }

/* Now we can release the save partition since all the uncovered parts 
   pointed by it were released. Also we restore the pointers at their 
   value before the branching. Finally we place the nodes decided in
   their proper place in the vector.					*/

     free((char *)save_partition);
     retained_nodes = save_retained_nodes;
     unretain_nodes = save_unretain_nodes;

     place_nodes_in_vector();
   }

/* the solution is finally obtained at this level, we will reset the branching
   level and return to the calling program.				*/
 
  if(VERBOSIS)
   { sprintf(error_buffer,"Branching finished at level %d",branching_depth);
     send_user_message(error_buffer);
   }
  branching_depth--;
}

/***********************************************************************

NAME
	make_output_sparse

PURPOSE
	When the number of cubes has been minimized we go trough this routine
	to remove the redundant connections in the output part. This way the
	number of transistors in a pla is lowered, the speed increase due to 
	reduced capacitive loading and the foldability of the matrix is
	improved.

SYNOPSIS
	make_output_sparse()

DESCRIPTION
	For each cube retained, we sharp all the other intersecting retained
	cubes. At the end we look if some bits at output disappeared from the
	part covered only by this cube. The output bits that disappeared are
	then removed from the cube.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	14 august 1984

AUTHOR
	Michel DAGENAIS

**************************************************************************/

make_output_sparse()

{
  struct node **cursor;		/* pointer in the vector of nodes */

  struct cube_list *temp_cube;  /* pointer in the list of cubes uncovered */ 

  cursor = prime_nodes;
  for(; cursor < retained_nodes ;)
   { scanned_node = *cursor;
     scanned_cube = copy_and_alloc_cube_list(scanned_node->cube);
     current_node = scanned_node;
     increment_pass_count();
     scan_sparse_ancestors();
     current_node = scanned_node;
     scan_sparse_descendants();
 
 /* scanned cube contains the part of the function covered only by this cube,
    we will or all the cubes in this list to get the minimal number of output
    bits that we can keep without changing the boolean function to minimize.  */

     set_output_word(scanned_node->cube,mask00);
     temp_cube = scanned_cube;
     if(temp_cube == NULL)
      {	if(max_branching_depth != INFINITY) 
			fatal_program_error("the solution is redundant"); 
	retained_nodes--;
	*cursor = *retained_nodes;
	*retained_nodes = scanned_node;
      }
     else
      { cursor++;
	for(; temp_cube != NULL ; temp_cube = temp_cube->next_cube)
         { or_output(temp_cube->cube,scanned_node->cube);
         }
        free_list_of_cubes(&scanned_cube);
      }
   }
}

#ifdef CHECK
#include "check.c"
#endif

