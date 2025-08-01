
#include "cubes.h"

/**********************************************************************

NAME
	scan_affected_retained_ancestors, scan_affected_retained_descendants
	scan_affected_unretain_ancestors, scan_affected_unretain_descendants

PURPOSE
	This recursive function scans all the path in the graph that may
	contain some nodes that may be affected by the decision taken on
	a node. If a node is retained, all the undecided nodes that intersect
	with this node might get completely covered or simply inferior; their
	uncovered part is updated in the process to take into account the fact
	that a new node was retained and covers part of it. If a node is
	unretained, some node might become essential because they would be
	the only remaining undecided node to cover a yet uncovered part of
	the function.

SYNOPSIS
	scan_affected_retained_ancestors()

	scan_affected_retained_descendants()

	scan_affected_unretain_ancestors()

	scan_affected_unretain_descendants()

DESCRIPTION
    -scan_affected_retained_ancestors will look at all the ancestors of the
	current_node. For each of them, if their uncovered part intersects with
	the scanned_node, it is updated to take into account the fact that
	the scanned node is being retained. The undecided nodes affected are
	put on the stack with the status affected_retained. Also when we have
	intersection, all the ancestors and descendants of the node affected
	will be recursively scanned.

    -scan_affected_retained_descendants does the samething but on the
	descendants of the current node. Also the intersecting descendants
	only have their descendants recursively scannned.

    -scan_affected_unretain_ancestors will look to all the ancestors of
	the current node. For each of them, if their uncovered part intersects
	with the scanned node they have their ancestors and descendants
	recursively checked. Also the nodes undecided affected are put on the
	affected stack to be processed in case we can now get a decision on 
	them.

    -scan_affected_unretain_descendants does the same but on the descendants
	of the current node. Also the intersecting descendants only have
	their intersecting descendants recursively scanned.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	17 july 1984

AUTHOR
	Michel DAGENAIS

*************************************************************************/

scan_affected_retained_descendants()

{
  struct parent *temp_parent;	/* parent to inspect */

  temp_parent = current_node->descendants;

  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

/* we must check that we do not come back to either the calling node 
   or a node already scanned to avoid passing many times on some node on the 
   graph. 							*/

     if((current_node->count | ONE) == odd_pass_counter)continue;

/* if the node is covered there is nothing to do on this side */

     if(current_node->status & COVERED)
      { current_node->count = pass_counter;
	continue;
      }

/* if the uncovered part of the node intersects with the node retained
   we will sharp it.						*/

     if(intersect_list(scanned_node->cube,current_node->uncovered))
      { current_node->count = odd_pass_counter;
	if(disjoint_sharp(&(current_node->uncovered),scanned_node->cube))
	 { current_node->status |= COVERED;
	 }
	if(current_node->status & DECIDED)scan_affected_retained_descendants();
	else

/* the node is undecided so if it is not already in the affected stack, we
   will put it in it.						*/

	 { if((current_node->status & AFFECTED) == 0) push(current_node);
	   current_node->status |= AFFECTED_RETAINED;
	   scan_affected_retained_descendants();
	 }
      }
     else current_node->count = pass_counter;
   }
}

/****************************************************************************/

scan_affected_retained_ancestors()

{
  struct parent *temp_parent;	/* parent to inspect */

  temp_parent = current_node->ancestors;

  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

/* we must check that we do not come back to either the calling node 
   or a node already scanned to avoid passing many times on some node on the 
   graph. 							*/

     if(current_node->count == pass_counter)continue;

/* If the node was already scanned for descendants, we will also at this point
   scan its ancestors.						*/

     if(current_node->count == odd_pass_counter)
      { current_node->count = pass_counter;
	scan_affected_retained_ancestors();
        continue;
      }

/* if the node is covered there is nothing to do on this side */

     current_node->count = pass_counter;

     if(current_node->status & COVERED) continue;

/* if the uncovered part of the node intersects with the node retained
   we will sharp it.						*/

     if(intersect_list(scanned_node->cube,current_node->uncovered))
      { if(disjoint_sharp(&(current_node->uncovered),scanned_node->cube))
	 { current_node->status |= COVERED;
	 }
	if(current_node->status & DECIDED)
	 { scan_affected_retained_ancestors();
	   current_node = temp_parent->parent;
	   scan_affected_retained_descendants();
 	 }
	else

/* the node is undecided so if it is not already in the affected stack, we
   will put it in it.						*/

	 { if((current_node->status & AFFECTED) == 0) push(current_node);
	   current_node->status |= AFFECTED_RETAINED;
	   scan_affected_retained_ancestors();
	   current_node = temp_parent->parent;
	   scan_affected_retained_descendants();
	 }
      }
   }
}

/***************************************************************************/

scan_affected_unretain_descendants()

{
  struct parent *temp_parent;	/* parent to inspect */

  temp_parent = current_node->descendants;

  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

/* we must check that we do not come back to either the calling node 
   or a node already scanned to avoid passing many times on some node on the 
   graph. 							*/

     if((current_node->count | ONE) == odd_pass_counter)continue;

/* if the node is covered there is nothing to do on this side */

     if(current_node->status & COVERED)
      { current_node->count = pass_counter;
	continue;
      }

/* if the uncovered part of the node intersects with the node unretained
   this part may become essential if this node is the only one not
   unretained to cover it.				*/

     if(intersect_list(scanned_node->cube,current_node->uncovered))
      { current_node->count = odd_pass_counter;
	if(current_node->status & DECIDED)scan_affected_unretain_descendants();
	else

/* the node is undecided so if it is not already in the affected stack, we
   will put it in it.						*/

	 { if((current_node->status & AFFECTED) == 0) push(current_node);
	   current_node->status |= AFFECTED_UNRETAIN;
	   scan_affected_unretain_descendants();
	 }
      }
     else current_node->count = pass_counter;
   }
}

/***************************************************************************/

scan_affected_unretain_ancestors()

{
  struct parent *temp_parent;	/* parent to inspect */

  temp_parent = current_node->ancestors;

  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

/* we must check that we do not come back to either the calling node 
   or a node already scanned to avoid passing many times on some node on the 
   graph. 							*/

     if(current_node->count == pass_counter)continue;

/* if the node was already scanned for descendants we will now at this
   point scan it for ancestors.					*/

     if(current_node->count == odd_pass_counter)
      { current_node->count = pass_counter;
        scan_affected_unretain_ancestors();
	continue;
      }

     current_node->count = pass_counter;

/* if the node is covered there is nothing to do on this side */

     if(current_node->status & COVERED)continue;

/* if the uncovered part of the node intersects with the node unretained
   this part may become essential if this node is the only one not
   unretained to cover it.				*/

     if(intersect_list(scanned_node->cube,current_node->uncovered))
      { if(current_node->status & DECIDED)
	 { scan_affected_unretain_ancestors();
	   current_node = temp_parent->parent;
	   scan_affected_unretain_descendants();
	 }
	else

/* the node is undecided so if it is not already in the affected stack, we
   will put it in it.						*/

	 { if((current_node->status & AFFECTED) == 0) push(current_node);
	   current_node->status |= AFFECTED_UNRETAIN;
	   scan_affected_unretain_ancestors();
	   current_node = temp_parent->parent;
	   scan_affected_unretain_descendants();
	 }
      }
   }
}

/*****************************************************************************

NAME
	scan_inferior_descendants, scan_inferior_ancestors

PURPOSE
	Determine if the uncovered part of a cube by retained cubes can
	be covered by a single unretained cube of lower or equal cost,
	in which case, the scanned cube is inferior.

SYNOPSIS
	int scan_inferior_descendants()

	int scan_inferior_ancestors()

DESCRIPTION

    -scan_inferior_descendants, for each descendants of the current node,
	we look if they are undecided and can cover the uncovered part of
	the scanned node. If the cube is of lower or equal cost then the
	scanned node, we unretain inferior the scanned node. If the node
	is of greater cost and covers, if we dont ask for minimum number 
	of literals, we will unretain inferior the scanned node just as well.
	If the descendants examined is undecided inferior or if it is
	undecided but of greater cost, we will scan its descendants 
	recursively.

    -scan_inferior_ancestors does the same but on the ancestors. Also the
	recursive scanning is on both the ancestors and descendants.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	17 july 1984

AUTHOR
	Michel DAGENAIS

*************************************************************************/

int scan_inferior_descendants()

{
  struct parent *temp_parent;	/* parent to inspect */

  temp_parent = current_node->descendants;

  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

/* we must check that we do not come back to either the calling node 
   or the scanned node to avoid passing many times on some node on the 
   graph. 							*/

     if((current_node->count | ONE) == odd_pass_counter)continue;

/* If the node is covered by retained cubes, we can ignore it since it was
   sharped from the scanned cube in the scan covering pass	*/

     if(current_node->status & COVERED) 
      { current_node->count = pass_counter;
        continue;
      }

     if(intersect_list(current_node->cube,scanned_node->uncovered))
      { current_node->count = odd_pass_counter;
        if(current_node->status & DECIDED)

/* the node is unretained but covers the uncovered part of the scanned_node
   so maybe one of its parents is undecided and also covers it and we must
   scan them.								*/

	 { if(scan_inferior_descendants()) return(1);
	 }

/* if the node is undecided and covers the scanned cube and has a lower cost
   then the scanned cube is unretained inferior.			*/

	else if(covers_list(current_node->cube,scanned_node->uncovered))
	 { if(current_node->cost <= scanned_node->cost || DONT_MIN_LITERAL)
	    {
#ifdef CHECK
	      unretain_inferior_check_node();
#else
	      unretain_inferior_node();
#endif
              return(1);
	    }
	   else if(scan_inferior_descendants()) return(1);
	 }
	else
	 { if(scan_inferior_descendants()) return(1);
	 }
      }
     else current_node->count = pass_counter;
   }
  return(0);
}

/**************************************************************************/

int scan_inferior_ancestors()

{
  struct parent *temp_parent;	/* parent to inspect */

  temp_parent = current_node->ancestors;

  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

/* we must check that we do not come back to either the calling node 
   or the scanned node to avoid passing many times on some node on the 
   graph. 							*/

     if(current_node->count == pass_counter)continue;

     if(current_node->count == odd_pass_counter)
      { current_node->count = pass_counter;
	if(scan_inferior_ancestors()) return(1);
	continue;
      }

     current_node->count = pass_counter;

/* If the node is covered by retained cubes, we can ignore it since it was
   sharped from the scanned cube in the scan covering pass	*/

     if(current_node->status & COVERED) continue;

     if(intersect_list(current_node->cube,scanned_node->uncovered))
      { if(current_node->status & DECIDED)

/* the node is unretained but covers the uncovered part of the scanned_node
   so maybe one of its parents is undecided and also covers it and we must
   scan them.								*/

	 { if(scan_inferior_ancestors()) return(1);
	   current_node = temp_parent->parent;
	   if(scan_inferior_descendants()) return(1);
	 }

/* if the node is undecided and covers the scanned cube and has a lower cost
   then the scanned cube is unretained inferior.			*/

	else if(covers_list(current_node->cube,scanned_node->uncovered))
	 { if(current_node->cost <= scanned_node->cost || DONT_MIN_LITERAL)
	    {
#ifdef CHECK
	      unretain_inferior_check_node();
#else
	      unretain_inferior_node();
#endif
	      return(1);
	    }
	   else
	    { if(scan_inferior_ancestors()) return(1);
	      current_node = temp_parent->parent;
	      if(scan_inferior_descendants()) return(1);
	    }
	 }
	else
	 { if(scan_inferior_ancestors()) return(1);
	   current_node = temp_parent->parent;
	   if(scan_inferior_descendants()) return(1);
	 }
      }
   }
  return(0);
}

/***********************************************************************

NAME
	scan_essential_descendants, scan_essential_ancestors

PURPOSE
	Determine if the uncovered part of the scanned node can be 
	covered by an undecided node, if not the scanned node must
	be retained.

SYNOPSIS
	int scan_essential_descendants()

	int scan_essential_ancestors()

DESCRIPTION
	The parent nodes are scanned to see if the uncovered part can be
	covered by the undecided cubes. Scanned_cube contains the part
	of scanned_node not covered by any retained or undecided node.
	The parents of the current node which are undecided are sharped
	from the scanned_cube; The parents unretained inferior intersecting
	with the scanned cube are scanned recursively in case one of their
	parent would intersect with scanned cube and be undecided.

    -scan_essential_descendants examines the descendants of the current node
	and scans recursively the intersecting inferior nodes.

    -scan_essential_ancestors does the same on the ancestors of the current
	node. Also the ancestors and descendants are scanned recursively.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	17 july 1984

AUTHOR
	Michel DAGENAIS

*************************************************************************/

int scan_essential_descendants()

{
  struct parent *temp_parent;	/* parent to inspect */

  temp_parent = current_node->descendants;

  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

/* we must check that we do not come back to either the calling node 
   or the scanned node to avoid passing many times on some node on the 
   graph. 							*/

     if((current_node->count | ONE) == odd_pass_counter)continue;

/* If the node is covered by retained cubes, we can ignore it since it was
   sharped from the scanned cube in the scan covering pass	*/

     if(current_node->status & COVERED) 
      { current_node->count = pass_counter;
	continue;
      }

     if(intersect_list(current_node->cube,scanned_cube))
      { if(current_node->status & DECIDED)

/* the node is unretained but covers a part of the scanned_node
   so maybe one of its parents is undecided and also covers it and we must
   scan them.								*/

	 { current_node->count = odd_pass_counter;
	   if(scan_essential_descendants())return(1);
	 }

/* if the node is undecided and covers a part of the scanned cube we sharp it
   and see if any uncovered part remains.				*/

	else
	 { current_node->count = pass_counter;
 	   if(disjoint_sharp(&scanned_cube,current_node->cube)) return(1);
	 }
      }
     else current_node->count = pass_counter;
   }
  return(0);
}

/**************************************************************************/

int scan_essential_ancestors()

{
  struct parent *temp_parent;	/* parent to inspect */

  temp_parent = current_node->ancestors;

  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

/* we must check that we do not come back to either the calling node 
   or the scanned node to avoid passing many times on some node on the 
   graph. 							*/

     if(current_node->count == pass_counter)continue;

     if(current_node->count == odd_pass_counter)
      { current_node->count = pass_counter;
	if(scan_essential_ancestors()) return(1);
	continue;
      }

     current_node->count = pass_counter;

/* If the node is covered by retained cubes, we can ignore it since it was
   sharped from the scanned cube in the scan covering pass	*/

     if(current_node->status & COVERED) continue;

     if(intersect_list(current_node->cube,scanned_cube))
      { if(current_node->status & DECIDED)

/* the node is unretained but covers a part of the scanned_node
   so maybe one of its parents is undecided and also covers it and we must
   scan them.								*/

	 { if(scan_essential_ancestors())return(1);
	   current_node = temp_parent->parent;
	   if(scan_essential_descendants()) return(1);
	 }

/* if the node is undecided and covers a part of the scanned cube we sharp it
   and see if any uncovered part remains.				*/

	else
	 { if(disjoint_sharp(&scanned_cube,current_node->cube)) return(1);
	 }
      }
   }
  return(0);
}

/*************************************************************************

NAME
	scan_partition

PURPOSE
	When we have a cycle we want to put in a partition only the nodes that
	can interact with each other. Two nodes can interact with each other 
	when their respective uncovered part do intersect. So in a partition
	we put together all the nodes undecided or inferior such that each node
	is related directly or indirectly to the starting node.

SYNOPSIS
	scan_partition()

DESCRIPTION
	All the nodes which can affect the current node have their pass count
	set to the pass counter and they are recursively scanned. Also, each
	node put this way in the partition is counted in scan_count. 

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	17 july 1984

AUTHOR
	Michel DAGENAIS

****************************************************************************/

scan_partition()

{
  struct parent *temp_parent;	/* parent to inspect */

  struct node *save_current_node;	/* current node saved during scanning */

/* we will now scan its ancestors to find uncovered nodes to scan */

  save_current_node = current_node;
  temp_parent = current_node->ancestors;

  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

     if(current_node->count == pass_counter)continue;

/* If the node is covered by retained cubes, we can ignore it since it 
   does cut the interactions with the other cubes related to it. */

     if(current_node->status & COVERED) 
      { current_node->count = pass_counter;
        continue;
      }

/* it is a undecided or inferior node it will be scanned for partition */

     if(intersect_list(current_node->cube,save_current_node->uncovered))
      { current_node->count = pass_counter;
	scan_count++;
	scan_partition();
      }
   }
	
/* we will now scan its descendants to find uncovered nodes to scan */

  temp_parent = save_current_node->descendants;

  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

     if(current_node->count == pass_counter)continue;

/* If the node is covered by retained cubes, we can ignore it since it 
   does cut the interactions with the other cubes related to it. */

     if(current_node->status & COVERED) 
      { current_node->count = pass_counter;
        continue;
      }

/* it is a undecided or inferior node it will be scanned for partition */

     if(intersect_list(current_node->cube,save_current_node->uncovered))
      { current_node->count = pass_counter;
	scan_count++;
        scan_partition();
      }
   }
}

/**************************************************************************

NAME
	scan_sparse_ancestors, scan_sparse_descendants

PURPOSE
	Find all the other retained cubes intersecting with a retained cube,
	to sharp them and determine which part is uniquely covered by each
	retained cube.

SYNOPSIS
	scan_sparse_ancestors()

	scan_sparse_descendants()

DESCRIPTION

    -scan_sparse_ancestors, the ancestors of the current node are examined,
	the retained nodes are sharped from the scanned cube. The other
	nodes intersecting with the scanned cube have their ancestors and
	descendants recursively scanned.

    -scan_sparse_descendants does the same but for the descendants of the
	current node. Also the recursive scan is done only on the descendants.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	14 august 1984

AUTHOR
	Michel DAGENAIS

*************************************************************************/

scan_sparse_descendants()

{
  struct parent *temp_parent;	/* parent to inspect */

  temp_parent = current_node->descendants;

  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

/* we must check that we do not come back to either the calling node 
   or a node already scanned to avoid passing many times on some node on the 
   graph. 							*/

     if((current_node->count | ONE) == odd_pass_counter)continue;

/* The node is retained, we will sharp it from the scanned cube */ 

     if(current_node->status & RETAINED)
      { (void)disjoint_sharp(&scanned_cube,current_node->cube);
	current_node->count = pass_counter;
	continue;
      }

     if(intersect_list(current_node->cube,scanned_cube))
      { current_node->count = odd_pass_counter;
	scan_sparse_descendants();
      }
     else current_node->count = pass_counter;
   }
}

/****************************************************************************/

scan_sparse_ancestors()

{
  struct parent *temp_parent;	/* parent to inspect */

  temp_parent = current_node->ancestors;

  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

/* we must check that we do not come back to either the calling node 
   or a node already scanned to avoid passing many times on some node on the 
   graph. 							*/

     if(current_node->count == pass_counter)continue;

/* If the node was already scanned for descendants, we will also at this point
   scan its ancestors.						*/

     if(current_node->count == odd_pass_counter)
      { current_node->count = pass_counter;
	scan_sparse_ancestors();
        continue;
      }

     current_node->count = pass_counter;

     if(current_node->status & RETAINED) 
      { (void)disjoint_sharp(&scanned_cube,current_node->cube);
	continue;
      }

     if(intersect_list(scanned_node->cube,current_node->uncovered))
      { scan_sparse_ancestors();
	current_node = temp_parent->parent;
	scan_sparse_descendants();
      }
   }
}

