#include "cubes.h"

/****************************************************************************

NAME
	retain_node, unretain_inferior_node

DESCRIPTION
	Once we arrived to a decision concerning the scanned_node, we can
	have it executed it by the proper function in the list above.

SYNOPSIS
	retain_node()

	unretain_inferior_node()

DESCRIPTION

    -retain_node, the scanned_node is essential to the cover of our boolean
	function and we decided to retain it. This function will set its status
	to decided retained, free the scanned_cube allocated for this decision
	and finally we will scan all the intersecting nodes in the graph that
	can be affected. The affected nodes will be placed on the stack to be
	processed later.

    -unretain_node, the scanned node can be covered by another node so it is
	inferior and we decided to unretain it. Its status will be set to
	decided inferior and we will scan all the nodes affected by this  
	to put them on the processing stack.

COORDINATES
	McGill University Electrical Engineering MONTREAL CANADA
	17 july 1984

AUTHOR
	Michel DAGENAIS

***********************************************************************/

retain_node()

{ 
  free_list_of_cubes(&scanned_cube);
  free_list_of_cubes(&(scanned_node->uncovered));
  scanned_node->status = DECIDED_RETAINED;
  current_node = scanned_node;
  increment_pass_count();
  scan_affected_retained_ancestors();
  current_node = scanned_node;
  scan_affected_retained_descendants();
}

/************************************************************************/

unretain_inferior_node()

{
  scanned_node->status = DECIDED_INFERIOR;
  current_node = scanned_node;
  increment_pass_count();
  scan_affected_unretain_ancestors();
  current_node = scanned_node;
  scan_affected_unretain_descendants();
}

/**************************************************************************

NAME
	place_nodes_in_vector

PURPOSE
	This function puts the nodes in the vector in the following
	order : retained, undecided and unretained inferior, unretained.
	They arrive randomly placed.

SYNOPSIS
	place_nodes_in_vector()

DESCRIPTION
	We have taken many decisions to retain or unretain nodes, and we
	simply set their status word accordingly. Now we want to order them
	in the vector to remove from the active area the nodes which are
	decided and covered by the nodes retained up to date. The nodes retained
	are placed at the beginning of the vector, the nodes unretained and
	covered by the retained nodes are placed at the end and the nodes
	inferior or undecided are left in the middle. The nodes which are not
	covered (undecided and inferior unretained) are left in the middle
	because any decision may affect them so when we decide to branch,
	they must all be remembered.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	25 july 1984

AUTHOR
	Michel DAGENAIS

***********************************************************************/

place_nodes_in_vector()

{
  int status;		/* temp space to put the status of the node processed */

  struct node
	**cursor,	/* pointer in the vector to nodes to place */
	*temp_node;	/* temp pointer to a node to place */	

  cursor = retained_nodes;

  for(; cursor < unretain_nodes ;)
   { status = (*cursor)->status;
     if(status & COVERED)
  
  /* The node is retained, it will be placed at the beginning of the vector
     the the pointer to retained nodes is incremented.			*/
 
      { if(status & RETAINED)
  	 { if(cursor == retained_nodes) cursor++;

	   else
	    { temp_node = *cursor;
	      *cursor = *retained_nodes;
	      *retained_nodes = temp_node;
	    }
           retained_nodes++;
	 }

/* The node is unretain covered it will be placed at the end of the vector.  */

	else
	 { unretain_nodes--;
	   temp_node = *cursor;
	   *cursor = *unretain_nodes;
	   *unretain_nodes = temp_node;
	 }
      }

/* The node is not covered, we leave it in the middle of the vector.	*/

     else cursor++;
   }

/* the retained and unretained covered nodes are now placed. Now we will
   count the undecided nodes. The count is placed in the external variable
   scan_count.								*/

  cursor = retained_nodes;
  scan_count = 0;

  for(; cursor < unretain_nodes ; cursor++)
   { if((*cursor)->status & DECIDED) continue;
     scan_count++;
   }
}

/*********************************************************************

NAME
	select_node

PURPOSE
	This function is called to select an undecided node using some
	heuristics. We want this node to break the cycle encountered
	efficiently. If we do not branch we simply hope that the node
	selected will give a good solution when retained.

SYNOPSIS
	select_node()

DESCRIPTION
	The function puts the node selected in scanned node. The heuristic
	used is the following; we take the node which if retained or
	unretained will affect the biggest number of direct undecided
	parents. This way we know that if we retain  or unretain it, most 
	probably, many nodes will be affected and get decided. We want
	this way to break in pieces the cycles, to reduce the branching depth
	necessary to solve completely the covering.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	27 july 1984

AUTHOR
	Michel DAGENAIS

****************************************************************************/

#define AFFECTED_SCORE 1
#define COVERED_SCORE 4

select_node()

{
  struct parent *temp_parent;	/* pointer in the parent list of the node */

  struct node
	*temp_node,		/* pointer to the node examined */
	**cursor;		/* pointer in the vector of nodes */

  int
	value,			/* score of the temp_node */
	best_value;		/* score of scanned node, the best node */

  best_value = -1;
  cursor = retained_nodes;

  for(; cursor < unretain_nodes ; cursor++)
   { temp_node = *cursor;
     if(temp_node->status & DECIDED) continue;

/* the node is undecided, we will see how many of its parents have their
   uncovered part intersecting with it and so would be affected by any
   decision taken on it.					*/

     value = 0;
     temp_parent = temp_node->ancestors;
     for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
      { if(temp_parent->parent->status & DECIDED) continue;
	if(intersect_list(temp_node->cube,temp_parent->parent->uncovered))
         { if(covers_list(temp_node->cube,temp_parent->parent->uncovered))
	    { value += COVERED_SCORE;
	    }
	   else value += AFFECTED_SCORE;
	 }
      }

     temp_parent = temp_node->descendants;
     for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
      { if(temp_parent->parent->status & DECIDED) continue; 
	if(intersect_list(temp_node->cube,temp_parent->parent->uncovered))
         { if(covers_list(temp_node->cube,temp_parent->parent->uncovered))
	    { value += COVERED_SCORE;
	    }
	   else value += AFFECTED_SCORE;
	 }
      }

     if(value > best_value)
      { best_value = value;
	scanned_node = temp_node;
      }
   }

/* There should always be a node selected, but we might just check because
   the CPU cost is very small and it is very healty for software to check
   itself with assertions.						*/

  if(best_value == -1) fatal_program_error("partition with no undecided node"); 
}

/*********************************************************************

NAME
	increment_pass_count, init_pass_count

PURPOSE
	These function take care of all the operations needed by the 
	pass count, from the initialization of the count in the nodes
	to the increment of the pass counter.

SYNOPSIS
	increment_pass_count()

	init_pass_count()

DESCRIPTION
	When we initialize, all the nodes in the working lists get their count
	reset to 0, the pass_counter is then put to 2. At any time when we
	increment the pass_count, we want to insure that the count of all the
	nodes is inferior to the new pass_count so the count can never reach
	any of the values left in the nodes. There is a problem however, when
	the pass_count does overflow and comes back to 0; at that point, we
	simply reinitialize all the nodes and put the pass count to 2. Also
	the first node scanned, current_node, will have its count set to the
	pass counter.

    -increment_pass_count increments of 2 the pass counter and set to pass
	counter + 1 the odd_pass_counter. If the pass counter does overflow,
	we call the function init pass count.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	20 july 1984

AUTHOR
	Michel DAGENAIS

*************************************************************************/

increment_pass_count()

{
  pass_counter += 2;

/* we have an overflow of the pass counter */

  if(pass_counter == 0)
   { init_pass_count();
     pass_counter += 2;
   }

  odd_pass_counter = pass_counter | ONE;
  current_node->count = pass_counter;
  return;
}

/**************************************************************************/

init_pass_count()

{
  struct node **cursor;		/* temp pointer in the vector to reset */

  pass_counter = 0;
  cursor = prime_nodes;
  for(; cursor <  end_prime ; cursor++)
   { (*cursor)->count = pass_counter;
   }
}

/***********************************************************************

NAME
	push, pop

PURPOSE
	push and pop node pointers from the stack of affected nodes.
	It is a standard stack implementation; When the stack is empty
	a null value is returned, when the stack is full a fatal error
	is signaled.

SYNOPSIS
	push(node)
	struct node *node;

	struct node *pop()

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL QUEBEC
	25 july 1984

AUTHOR
	Michel DAGENAIS

*****************************************************************************/

push(node)

struct node *node;
{
  if(current_in_stack == end_stack) 
			fatal_program_error("affected nodes stack is full");
  *current_in_stack = node;
  current_in_stack++;
}

/*************************************************************************/

struct node *pop()

{
  if(current_in_stack == start_stack) return(NULL);
  current_in_stack--;
  return(*current_in_stack);
}

