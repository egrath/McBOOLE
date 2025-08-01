#include "cubes.h"

/************************************************************************

NAME
	merge_and_link

PURPOSE
	Once the star product on two cubes gives a result, one or both
	of the original cubes may be covered and have to be deleted.
	Also the new cube must be placed properly in the graph and the
	cubes deleted removed from it.

SYNOPSIS
	struct node *merge_and_link(node0,node1,nodex) 
	struct node **node0,**node1,*nodex;

DESCRIPTION
	We look at node 1 and 0 to see if they are covered by nodex.
	A node deleted must have all its descendants and ancestors
	passed to the new node. The new node will have the two nodes
	as ancestors, or their own ancestors and descendants if they
	are covered. The address of the new node is returned.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	13 july 1984

AUTHOR
	Michel DAGENAIS

***************************************************************************/

struct node 
*merge_and_link(node0,node1,nodex)

struct node **node0,**node1,*nodex;
{
   struct node 
	*temp_node0,		/* temp pointer to a node */
	*temp_node1;		/* temp pointer to a node */

   struct parent *temp_parent;	/* temp pointer to a parent just allocated */

   int cover_status;		/* int tells which of the two cubes are covered
				   by the new cube nodex */

   cover_status = covers(nodex->cube,(*node0)->cube);
   cover_status = cover_status << 1;
   cover_status |= covers(nodex->cube,(*node1)->cube);
   temp_node0 = *node0;
   temp_node1 = *node1; 

   switch(cover_status)
    { case 0 :

/* none of the cubes are covered, cubex has two ancestors and a new node
   will be allocated. Also the new cube is a descendant of the two others   */

	 nodex = copy_and_alloc_node(nodex->cube);
	 temp_parent = alloc_parent();
	 nodex->ancestors = temp_parent;
	 temp_parent->parent = temp_node0;
	 temp_parent->next_parent = alloc_parent();
	 (temp_parent->next_parent)->parent = temp_node1;
	 temp_parent = alloc_parent();
	 temp_parent->next_parent = temp_node0->descendants;
	 temp_node0->descendants = temp_parent;
	 temp_parent->parent = nodex;
	 temp_parent = alloc_parent();
	 temp_parent->next_parent = temp_node1->descendants;
	 temp_node1->descendants = temp_parent;
	 temp_parent->parent = nodex;
	 return(nodex);

      case 3 :

/* the two cubes are covered by nodex, nodex will be placed in the graph
   instead of node0 and all the ancestors and descendants of the both
   cubes are passed to nodex.					*/

	 temp_node0->status |= temp_node1->status;
	 copy_cube(nodex->cube,temp_node0->cube);
	 pass_ancestors(temp_node1,temp_node0);
	 pass_descendants(temp_node1,temp_node0);
	 *node0 = temp_node0->next_node;
	 *node1 = temp_node1->next_node;
	 free_node(temp_node1);
	 return(temp_node0);

      case 2 :

/* the cube0 is covered by cubex. Cubex will take its place in the graph
   and cube1 will be added as one of its ancestor. Also cubex will be a 
   descendant of cube1.						*/

	 copy_cube(nodex->cube,temp_node0->cube);
	 temp_parent = alloc_parent();
	 temp_parent->next_parent = temp_node0->ancestors;
	 temp_node0->ancestors = temp_parent;
	 temp_parent->parent = temp_node1;
	 temp_parent = alloc_parent();
	 temp_parent->next_parent = temp_node1->descendants;
	 temp_node1->descendants = temp_parent;
	 temp_parent->parent = temp_node0;
	 *node0 = temp_node0->next_node;
	 return(temp_node0);

      case 1 :

/* the cube1 is covered by cubex. Cubex will take its place in the graph
   and cube0 will be added as one of its ancestor. Also cubex will be
   a descendant of cube0.					*/

	 copy_cube(nodex->cube,temp_node1->cube);
	 temp_parent = alloc_parent();
	 temp_parent->next_parent = temp_node1->ancestors;
	 temp_node1->ancestors = temp_parent;
	 temp_parent->parent = temp_node0;
	 temp_parent = alloc_parent();
	 temp_parent->next_parent = temp_node0->descendants;
	 temp_node0->descendants = temp_parent;
	 temp_parent->parent = temp_node1;
	 *node1 = temp_node1->next_node;
	 return(temp_node1);
    }

/* the program cannot reach here since all cases are defined and end with
   a return.								*/

   fatal_program_error("internal coding error in merge and link");
   return(nodex);
}

/************************************************************************

NAME
	pass_ancestors, pass_descendants

PURPOSE
	when a node is removed from the graph, its descendants and ancestors
	are passed to another node and this function does it.

SYNOPSIS
	pass_ancestors(node_from,node_to)
	struct node *node_from,*node_to;

	pass_descendants(node_from,node_to)
	struct node *node_from,_node_to;

DESCRIPTION
	When a set of parents is passed from a node to another, these parents
	should be inserted in the list of the node_to but also these ancestors
	should be notified that the address of their descendant is changed.
	for each ancestor or descendant passed, the corresponding list of
	descendant or ancestor of the parent node should be scanned for the
	address of the node_from to be replaced by the address of the node_to.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	13 july 1984

AUTHOR
	Michel DAGENAIS

*************************************************************************/

pass_descendants(node_from,node_to)

struct node *node_from,*node_to;
{
   struct parent 
	*temp_parent,		/* temp pointer to the next parent in list */
	*parent_to,		/* temp pointer to descendant list of node_to*/
	*parent_from;		/* temp pointer to descendant list of nodefrom*/

   parent_to = node_to->descendants;
   parent_from = node_from->descendants;

   for(; parent_from != NULL ; parent_from = temp_parent)

/* for each descendant in the list, we look in the ancestor list until
   we find the link to the node_from , after passing it to node_to.	*/

    { temp_parent = parent_from->next_parent;
      parent_from->next_parent = parent_to;
      parent_to = parent_from;
      parent_from = (parent_from->parent)->ancestors;

      for(; parent_from != NULL ; parent_from = parent_from->next_parent)
       { if(parent_from->parent == node_from)break;
       }

/* The link to node from was not found, the graph is inconsistent and the 
   program is probably bugged but at least it knows it			*/

      if(parent_from == NULL)
	fatal_program_error("inconsistency in graph description");

/* The link to node from was found, it is changed to point now to node_to */

      parent_from->parent = node_to;
    }
   node_to->descendants = parent_to;
}

/****************************************************************************/

pass_ancestors(node_from,node_to)

struct node *node_from,*node_to;
{
   struct parent 
	*temp_parent,		/* temp pointer to the next parent in list */
	*parent_to,		/* temp pointer to ancestor list of node_to*/
	*parent_from;		/* temp pointer to ancestor list of nodefrom*/

   parent_to = node_to->ancestors;
   parent_from = node_from->ancestors;

   for(; parent_from != NULL ; parent_from = temp_parent)

/* each link to ancestors is passed from node_from to node_to */

    { temp_parent = parent_from->next_parent;
      parent_from->next_parent = parent_to;
      parent_to = parent_from;
      parent_from = (parent_from->parent)->descendants;

/* for each ancestor we look in their list of descendants to find the link
   to node_from							*/

      for(; parent_from != NULL ; parent_from = parent_from->next_parent)
       { if(parent_from->parent == node_from)break;
       }

/* the link was not found the graph is inconsistent		*/

      if(parent_from == NULL)
	fatal_user_error("inconsistency in graph description");

/* the link was found we change it to point to node_to	*/

      parent_from->parent = node_to;
    }
   node_to->ancestors = parent_to;
}

/****************************************************************************

NAME
	remove_ancestors

PURPOSE
	When a cube is absorbed, its ancestors must be a subset of the
	ancestors of the cube that absorbs it. So these ancestors are simply
	removed from the graph and freed, before the node itself is freed.

SYNOPSIS
	remove_ancestors(node)
	struct node *node;

DESCRIPTION
	Each ancestor in the list has its parent structure pointing to
	node removed in its descendant list. We scan the ancestor list
	of the node, remove and free the link in its ancestor and then free
	the link to its ancestor.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	14 july 1984

AUTHOR
	Michel DAGENAIS

***************************************************************************/

remove_ancestors(node)

struct node *node;
{
  struct parent
	**prev_parent,		/* address of pointer of parent to remove */ 
	*temp_parent,		/* parent to remove */
	*parent_ancestor;	/* pointer to parent in a list */

  parent_ancestor = node->ancestors;
  for(; parent_ancestor != NULL ; parent_ancestor = parent_ancestor->next_parent)
   { prev_parent = &((parent_ancestor->parent)->descendants);

/* for each ancestor, we look in its descendant list for a link to node */

     for(; (temp_parent = *prev_parent) != NULL ;) 
      { if(temp_parent->parent == node)break;
	prev_parent = &(temp_parent->next_parent);
      }

/* the link was not found, the graph is inconsistent 	*/

     if(temp_parent == NULL)
        fatal_program_error("inconsistency in the graph");

/* the link is found, it is removed from the list and freed	*/

     *prev_parent = temp_parent->next_parent;
     free_parent(temp_parent);
   }

/* all the links of ancestors to the node were removed and freed, we can now
   free the list of ancestors.						*/

  free_list_of_parents(&(node->ancestors));
} 

/****************************************************************************

NAME
	absorb_and_unlink

PURPOSE
	When a cube is absorbed this function is called to remove it from the 
	graph.

SYNOPSIS
	int absorb_and_unlink(good_node,bad_node)
	struct node *good_node,**bad_node;

DESCRIPTION
	The bad node is covered by the bad node and should be removed from the
	graph. Its links with ancestors will be removed since the good node has
	its own ancestors that anyway already include the important ancestors
	of the bad node. The descendants of the bad node are passed to the
	good node because we cannot leave those without ancestors.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	13 july 1984

AUTHOR
	Michel DAGENAIS

****************************************************************************/

absorb_and_unlink(good_node,bad_node)

struct node *good_node,**bad_node;
{
  struct node *temp_node;		/* temp pointer to bad_node */

  temp_node = *bad_node;
  pass_descendants(temp_node,good_node);
  remove_ancestors(temp_node);
  good_node->status |= temp_node->status;
  *bad_node = temp_node->next_node;
  free_node(temp_node);
}

