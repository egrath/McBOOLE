
#include "cubes.h"

/****************************************************************************

NAME
	retain_check_node, unretain_inferior_check_node

DESCRIPTION
	Once we arrived to a decision concerning the scanned_node, we can
	have it executed it by the proper function in the list above.

SYNOPSIS
	retain_check_node()

	unretain_inferior_check_node()

DESCRIPTION
	The scanned node is removed from the undecided list and put in the
	proper list. Also its status is set properly. These functions are 
	similar to the normal retain and unretain functions except that 
	they are much slower because they do an extensive verification
	of the validity of the decision taken. This function should only
	be used to verify the rest of the program during the extensive
	validation pass of the program.

COORDINATES
	McGill University Electrical Engineering MONTREAL CANADA
	17 july 1984

AUTHOR
	Michel DAGENAIS

***********************************************************************/

retain_check_node()

{ 
  struct node **temp_in_vector;

  free_list_of_cubes(&scanned_cube);
 
  temp_in_vector = prime_nodes;
  scanned_cube = copy_and_alloc_cube_list(scanned_node->cube);
  for(; temp_in_vector < unretain_nodes; temp_in_vector++)
   { if(((*temp_in_vector)->status & DECIDED) && 
	(((*temp_in_vector)->status & RETAINED) == 0)) continue;
     if((*temp_in_vector) == scanned_node) continue;
     if(disjoint_sharp(&scanned_cube,(*temp_in_vector)->cube))
      {
#if DEBUG
		  foutput_cube_and_links(stderr,scanned_node);
		  foutput_graph_vector(stderr,retained_nodes,unretain_nodes);
#endif
		  fatal_program_error("no way you will retain this node");
      }
   }

  send_user_message("a node is retained ok");
  free_list_of_cubes(&scanned_cube);
  free_list_of_cubes(&(scanned_node->uncovered));
  scanned_node->status = DECIDED_RETAINED;
  current_node = scanned_node;
  increment_pass_count();
  scan_affected_retained_ancestors();
  current_node = scanned_node;
  scan_affected_retained_descendants();
}

/***************************************************************************/

unretain_inferior_check_node()

{
  struct node **temp, *temp_node;
  struct cube_list *temp_cube;

  temp_cube = copy_and_alloc_cube_list(scanned_node->cube);
  temp = prime_nodes;
  for(; temp < unretain_nodes ; temp++)
   { temp_node = *temp;
     if((temp_node->status & RETAINED) == 0) continue;
     if(disjoint_sharp(&temp_cube,temp_node->cube))
      {
#if DEBUG
		  foutput_cube_and_links(stderr,scanned_node);
#endif
		  fatal_program_error("an inferior cube is in fact covered");
      }
   }

  temp = retained_nodes;
  for(; temp < unretain_nodes ; temp++)
   { temp_node = *temp;
     if(temp_node->status & DECIDED) continue;
     if(temp_node == scanned_node) continue;
     if(covers_list(temp_node->cube,temp_cube))
      { free_list_of_cubes(&temp_cube);
	send_user_message("a node is unretained inferior ok");
	break;
      }
   }

  if(temp_cube != NULL)
   {
#if DEBUG
	   foutput_cube_and_links(stderr,scanned_node);
#endif
	   fatal_program_error("a node is unretained inferior but should not");
   }

  scanned_node->status = DECIDED_INFERIOR;
  current_node = scanned_node;
  increment_pass_count();
  scan_affected_unretain_ancestors();
  current_node = scanned_node;
  scan_affected_unretain_descendants();
}

/**********************************************************************

NAME
	check_cycle

PURPOSE
	When we are in the software validation process this function can
	check that we really have a cycle.

SYNOPSIS
	check_cycle()

DESCRIPTION
	Each node in the cycle is checked to see if it is covered or 
	essential in which case it is not a real cycle.

COORDINATES
	McGill Electrical Engineering VLSI lab MONTREAL CANADA
	17 august 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

check_cycle()

{
  struct node **temp1, **temp2;
  struct cube_list *temp_cube;

  temp1 = retained_nodes;
  for(; temp1 < unretain_nodes ; temp1++)
   { if(((*temp1)->status & DECIDED) == 0)
      { temp2 = prime_nodes;
	temp_cube = copy_and_alloc_cube_list((*temp1)->cube);
	for(; temp2 < unretain_nodes ; temp2++)
	 { if(((*temp2)->status & RETAINED) == 0) continue; 
	   if((*temp1) == (*temp2)) continue;
	   if(disjoint_sharp(&temp_cube,(*temp2)->cube))
	    {
#if DEBUG
			foutput_cube_and_links(stderr,(*temp1));
#endif
			fatal_program_error("a node in the cycle is in fact covered");
	    }
	 }
	temp2 = retained_nodes;
	for(; temp2 < unretain_nodes ; temp2++)
	 { if((*temp2)->status & DECIDED) continue;
           if((*temp2) == (*temp1)) continue;
	   if(disjoint_sharp(&temp_cube,(*temp2)->cube)) break;
	 }
	if(temp_cube != NULL)
	 {
#if DEBUG
		 foutput_cube_and_links(stderr,(*temp1));
#endif
		 fatal_program_error("a node in the cycle is in fact essential");
	 }
      }
   }
  send_user_message("you indeed seem to have a real cycle");
}

/******************************************************************

NAME
	check_graph

PURPOSE
	check that the graph generated by the function which does generate
	the prime implicants and the graph, is consistent.

SYNOPSIS
	check_graph()

DESCRIPTION
	We first that for each ancestor of a node, there is a link pointing
	to the node as a descendant. After we check if all the ancestors can
	cover the node, if not the node must be basic. Also we check that
	all the ancestors and descendants of the node do intersect with it.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	9 august 1984

AUTHOR
	Michel DAGENAIS

**************************************************************************/

check_graph()

{
  int nb_nodes, nb_basic, nb_scanned;
 
  struct node **temp1, **temp2, *temp_node, *parent_node;

  struct parent *temp_parent, *parent_parent;

  struct cube_list *temp_cube;

/* We first check the consistancy of the graph */

  temp1 = prime_nodes;
  for(; temp1 < end_prime ; temp1++)
   { temp_node = *temp1;
     temp_parent = temp_node->ancestors;
     for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
      { parent_node = temp_parent->parent;
	parent_parent = parent_node->descendants;
	for(; parent_parent !=  NULL ; parent_parent = parent_parent->next_parent)
	 { if(parent_parent->parent == temp_node) break;
	 }
	if(parent_parent == NULL)
	 {
#if DEBUG
		 foutput_cube_and_links(stderr,temp_node);
#endif
		 fatal_program_error("inconsistancy in the graph");
	 }
      }
     temp_parent = temp_node->descendants; 
     for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
      { parent_node = temp_parent->parent;
	parent_parent = parent_node->ancestors;
	for(; parent_parent !=  NULL ; parent_parent = parent_parent->next_parent)
	 { if(parent_parent->parent == temp_node) break;
	 }
	if(parent_parent == NULL)
	 {
#if DEBUG
		 foutput_cube_and_links(stderr,temp_node);
#endif
		 fatal_program_error("inconsistancy in the graph");
	 }
      }
   }
  
  send_user_message("the graph is consistent");

/* We count the nodes and the basic nodes; Also we check certain properties
   of the nodes that might not be there but should.			*/

  nb_nodes = 0;
  nb_basic = 0;
  temp1 = prime_nodes;
  for(; temp1 < end_prime ; temp1++)
   { temp_node = *temp1;
     nb_nodes++;
     if(temp_node->status & DONT_CARE) continue;
     temp_cube = copy_and_alloc_cube_list(temp_node->cube);
     temp_parent = temp_node->ancestors;
     for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
      { if(disjoint_sharp(&temp_cube,temp_parent->parent->cube))break;
      }
     if(temp_cube != NULL)
      { free_list_of_cubes(&temp_cube);
	if((temp_node->status & BASIC) == 0)
	 {
#if DEBUG
		 foutput_cube_and_links(stderr,temp_node);
#endif
		 fatal_program_error("this cube is in fact basic");
	 }
      }
   }
  
  temp1 = prime_nodes;
  for(; temp1 < end_prime ; temp_node = *(temp1++))
   { if((*temp1)->status & BASIC)
      { nb_basic++;
      }
   }
  send_user_message("all nodes uncovered by ancestors are basic");

/* We will now check that all the nodes intersecting are linked together
   in the graph.							*/

  temp1 = prime_nodes;
  for(; temp1 < end_prime ; temp1++)
   { temp_node = *temp1;
     temp_parent = temp_node->ancestors;
     for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
      { if(intersect(temp_node->cube,temp_parent->parent->cube) == 0)
	 {
#if DEBUG
		 foutput_cube_and_links(stderr,temp_node);
		 foutput_cube_and_links(stderr,temp_parent->parent);
#endif
		 fatal_program_error("the two nodes above are related but disjoint");
	 }
      }
     temp_parent = temp_node->descendants;
     for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
      { if(intersect(temp_node->cube,temp_parent->parent->cube) == 0)
	 {
#if DEBUG
		 foutput_cube_and_links(stderr,temp_node);
		 foutput_cube_and_links(stderr,temp_parent->parent);
#endif
		 fatal_program_error("the two nodes above are related but disjoint");
	 }
      }
   }
  
  temp1 = prime_nodes;
  for(; temp1 < end_prime ; temp1++)
   { temp_node = *temp1;
     current_node = temp_node;
     scanned_node = temp_node;
     scan_count = 1;
     increment_pass_count();
     scan_intersecting_ancestors();
     current_node = scanned_node;
     scan_intersecting_descendants();
     parent_node = temp_node;
     nb_scanned = 0;
     for(; parent_node != NULL ; parent_node = parent_node->next_node)
      { if(intersect(parent_node->cube,temp_node->cube))
	 { if((parent_node->count | ONE) != odd_pass_counter)
	    {
#if DEBUG
		  foutput_cube_and_links(stderr,temp_node);
	      foutput_cube_and_links(stderr,parent_node);
#endif
	      fatal_program_error("two nodes intersect but are not related");
	    }
	   nb_scanned++;
	 }
      }
     if(nb_scanned != scan_count)
      {
#if DEBUG
		  foutput_cube_and_links(stderr,temp_node);
#endif
		  fatal_program_error("some nodes are in the graph but not in the list");
      }
   }
  sprintf(error_buffer,"your graph is perfect and has %d nodes and %d basic\n",
	nb_nodes,nb_basic);
  send_user_message(error_buffer);
  init_pass_count();
}

/************************************************************************

NAME
	scan_intersecting_ancestors, scan_intersecting_descendants

SYNOPSIS
	scan_intersecting_ancestors(), scan_intersecting_descendants()

DESCRIPTION
	These function scan all the intersecting parents of the current node,
	set their pass counter and count them.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA

AUTHOR
	Michel DAGENAIS

*****************************************************************************/

scan_intersecting_ancestors()
{
  struct parent *temp_parent;

  temp_parent = current_node->ancestors;
  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;

     if(current_node->count == pass_counter) continue;
     if(current_node->count == odd_pass_counter)
      { current_node->count = pass_counter;
	scan_intersecting_ancestors();
	continue;
      }

     if(intersect(scanned_node->cube,current_node->cube) == 0)continue; 
     current_node->count = pass_counter;
     scan_count++;
     scan_intersecting_ancestors();
     current_node = temp_parent->parent;
     scan_intersecting_descendants();
   }
}

/***************************************************************************/

scan_intersecting_descendants()
{
  struct parent *temp_parent;

  temp_parent = current_node->descendants;
  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { current_node = temp_parent->parent;
     if((current_node->count | ONE) == odd_pass_counter) continue;
     if(intersect(current_node->cube,scanned_node->cube) == 0)continue;
     scan_count++;
     current_node->count = odd_pass_counter;
     scan_intersecting_descendants();
   }
}
     
