/***********************************************************************

NAME
	alloc_node, free_node, flush_node, free_list_of_nodes
 	alloc_cube_list, free_cube_list, flush_cube_list, free_list_of_cubes
	alloc_binary, free_binary, flush_binary
	alloc_parent, free_parent, flush_parent, free_list_of_parents

PURPOSE
	because the system allocation routines are not efficient when
	they are called very often for small blocks, special routines
	will be provided for each structure type to be allocated for
	cube manipulation.

SYNOPSIS
	
	struct node *alloc_node()

	free_node(node)
	struct node *node;
       
	flush_node()

	free_list_of_nodes(list)
	struct node **list;

	struct cube_list *alloc_cube_list()

	free_cube_list(cube)
	struct cube_list *cube;

	flush_cube_list()

	free_list_of_cubes(list)
	struct cube_list **list;

	struct binary *alloc_binary()

	free_binary(binary)
	struct binary *binary;

	flush_binary()

	struct parent *alloc_parent()

	free_parent(parent)
	struct parent *parent;

	flush_parent()

	free_list_of_parents(list)
	struct parent **list;

DESCRIPTION
	Big blocks of memory are allocated from which several nodes can be
	made. Each time alloc node is called a piece of this block is passed;
	when the whole block is used a new one is allocated. When some node 
	freed, they are kept in a list to be reallocated next time alloc node
	is called. The blocks are not freed by the free node function since
	they can be freed in any order and we never know when a block contains
	only blocks that were released and are presently in the free list.
	However when all the nodes are not useful anymore the function
	flush nodes can be called to release all the blocks. All the comments
	that apply  for the structure node will apply most of the time as well
	for the structures parent,binary and cube_list. 

DIAGNOSTIC
	Each time we allocate space the code returned by the system is
	verified and an error is signaled when no more memory is
	available.

COORDINATES
	McGill University Electrical Engineering MONTREAL CANADA
	2 july 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

#include "cubes.h"
#include <stdlib.h>

/* The size of the big blocks is defined here in terms of how many elements
   of the specified structure they contain. Those values can be changed to
   best values depending on your system and the average size of the exaamples
   you expect to run; in any case it must be much bigger then 1.	*/

#define MANY_NODE 100
#define MANY_CUBE 100
#define MANY_BINARY 200
#define MANY_PARENT 400

int
	nb_alloc_cube_list = 0,
	nb_alloc_parent = 0,
	nb_alloc_binary =  0,
	nb_alloc_nodes = 0;

struct block 
 { struct block *next_block;	/* pointer to next block allocated */
   char space[1];		/* the real size > 1 will be computed later */
 };

static struct block 
	*temp_block,		/* temp block pointer */
	*node_block_list = NULL,	/* list of blocks allocated */
	*cube_block_list = NULL,	/* list of blocks allocated for cube_list */
	*binary_block_list = NULL,/* list of blocks allocated for binary nodes*/
	*parent_block_list = NULL;	/* list of blocks allocated for parent */

static struct node 
	*node_free_list = NULL,	/* list of nodes released */
	*next_node,		/* temp pointer to next node in a list */
	*temp_node;		/* temp node pointer */

static struct cube_list
	*cube_free_list = NULL,	/* list of cube_list released */
	*next_cube,		/* temp pointer to next cube in a list */
	*temp_cube;		/* temp cube_list pointer */

static struct binary
	*binary_free_list = NULL,	/* list of binary nodes released */
	*temp_binary;		/* temp binary node pointer */

static struct parent
	*parent_free_list = NULL,	/* list of parents released */
	*next_parent,		/* temp pointer to next parent in a list */
	*temp_parent;		/* temp parent pointer */

static char 
	*start_node_free = NULL,	/* address of free space in block */
	*end_node_free = NULL,		/* address of block end */
	*start_cube_free = NULL,	/* address of free space in block for cubes */
	*end_cube_free = NULL,	/* address of block end */
	*start_binary_free = NULL,/* address of free space in block for binary */
	*end_binary_free = NULL,	/* address of block end */
	*start_parent_free = NULL,	/* address of free space in block for parents */
	*end_parent_free = NULL;	/* address of block end */

struct node *alloc_node()
{

/*  check if some released nodes are present in the list node free list,
    We first try to reallocate those by simply removing them from the list
    and sending their address to the calling program.			*/

   nb_alloc_nodes++;
   if(node_free_list != NULL)
    { temp_node = node_free_list;
      node_free_list = node_free_list->next_node;
      temp_node->next_node = NULL;
      temp_node->ancestors = NULL;
      temp_node->descendants = NULL;
      temp_node->status = NULL;
      return(temp_node);
    }

/*  Allocate fresh new space from a huge block allocated earlier using
    the calloc routine. We keep track of the space left in the block
    after we allocate each node						*/

   if(start_node_free < end_node_free)
    { temp_node = (struct node *)start_node_free;
      start_node_free = start_node_free + node_size;
      return(temp_node);
    }

/*  No more space left in the block we will have to allocate a new one
    initialize the pointers in it and send one node out of it for now.
									*/

   temp_block = node_block_list;
   node_block_list = (struct block *)
		calloc(1,sizeof(struct block) + MANY_NODE * node_size);
   if(node_block_list == NULL)fatal_system_error("unable to alloc nodes");
   node_block_list->next_block = temp_block;
   temp_node = (struct node *)node_block_list->space;
   start_node_free = node_block_list->space + node_size;
   end_node_free = node_block_list->space + MANY_NODE * node_size;
   return(temp_node);
}

/************************************************************************/

free_node(node)

struct node *node;
{

/*  All the nodes freed by the user are kept in a list until he wants
    to allocate other nodes in which case the old nodes are sent back.
    we do not send back this space to the system to avoid inefficient
    repetion of alloc and free operations on nodes			*/

   nb_alloc_nodes--;
   node->next_node = node_free_list;
   node_free_list = node;
}

/************************************************************************/

#ifdef DEBUG

flush_node()

{

/*  All the blocks allocated to contain nodes are freed one by one */

   nb_alloc_nodes = 0;
   for(; node_block_list != NULL ; node_block_list = temp_block)
    { temp_block = node_block_list->next_block;
      free((char *)node_block_list);
    }
   node_free_list = NULL;
   start_node_free = NULL;
   end_node_free = NULL;
}

#endif

/*************************************************************************/

free_list_of_nodes(list)

struct node **list;
{

/* We go to the end of the list to free, append the free list and put this
   new list back as the free list. The pointer to the list freed is
   reset to 0.								*/

  temp_node = *list;
  if(temp_node == NULL) return;
  for( ; ; temp_node = next_node)
   { next_node = temp_node->next_node;
     nb_alloc_nodes--;
     if(next_node == NULL) break;
   }
  temp_node->next_node = node_free_list;
  node_free_list = *list;
  *list = NULL;
}

/************************************************************************/
   
struct cube_list *alloc_cube_list()
{

/*  The allocation routines for the other kind of structures are similar
    to the ones for the nodes so for explanations refer to the previous
    section.								*/

   nb_alloc_cube_list++;
   if(cube_free_list != NULL)
    { temp_cube = cube_free_list;
      cube_free_list = cube_free_list->next_cube;
      temp_cube->next_cube = NULL;
      return(temp_cube);
    }

   if(start_cube_free < end_cube_free)
    { temp_cube = (struct cube_list *)start_cube_free;
      start_cube_free = start_cube_free + cube_list_size;
      return(temp_cube);
    }

   temp_block = cube_block_list;
   cube_block_list = (struct block *)
		calloc(1,sizeof(struct block) + MANY_CUBE * cube_list_size);
   if(cube_block_list == NULL)fatal_system_error("unable to alloc cubes");
   cube_block_list->next_block = temp_block;
   temp_cube = (struct cube_list *)cube_block_list->space;
   start_cube_free = cube_block_list->space + cube_list_size;
   end_cube_free = cube_block_list->space + MANY_CUBE * cube_list_size;
   return(temp_cube);
}

/************************************************************************/

free_cube_list(cube)

struct cube_list *cube;
{
   nb_alloc_cube_list--;
   cube->next_cube = cube_free_list;
   cube_free_list = cube;
}

/************************************************************************/

#ifdef DEBUG

flush_cube_list()

{
   nb_alloc_cube_list = NULL;
   for(; cube_block_list != NULL ; cube_block_list = temp_block)
    { temp_block = cube_block_list->next_block;
      free((char *)cube_block_list);
    }
   cube_free_list = NULL;
   start_cube_free = NULL;
   end_cube_free = NULL;
}

#endif

/*************************************************************************/

free_list_of_cubes(list)

struct cube_list **list;
{
  temp_cube = *list;
  if(temp_cube == NULL) return;
  for( ; ; temp_cube = next_cube)
   { next_cube = temp_cube->next_cube;
     nb_alloc_cube_list--; 
     if(next_cube == NULL) break;
   }
  temp_cube->next_cube = cube_free_list;
  cube_free_list = *list;
  *list = NULL;
}

/*************************************************************************/
   
struct binary *alloc_binary()
{

/*  The allocation routines for the other kind of structures are similar
    to the ones for the nodes so for explanations refer to the previous
    section.								*/

   nb_alloc_binary++;
   if(binary_free_list != NULL)
    { temp_binary = binary_free_list;
      binary_free_list = binary_free_list->is0._.subtree;
      temp_binary->is0.status = NULL;
      temp_binary->is1.status = NULL;
      temp_binary->isx = NULL;
      return(temp_binary);
    }

   if(start_binary_free < end_binary_free)
    { temp_binary = (struct binary *)start_binary_free;
      start_binary_free = start_binary_free + sizeof(struct binary);
      return(temp_binary);
    }

   temp_block = binary_block_list;
   binary_block_list = (struct block *)
    calloc(1,sizeof(struct block) + MANY_BINARY * sizeof(struct binary));
   if(binary_block_list == NULL)fatal_user_error("unable to alloc binarys");
   binary_block_list->next_block = temp_block;
   temp_binary = (struct binary *)binary_block_list->space;
   start_binary_free = binary_block_list->space + sizeof(struct binary);
   end_binary_free = binary_block_list->space 
				+ MANY_BINARY * sizeof(struct binary);
   return(temp_binary);
}

/****************************************************************************/

free_binary(binary)

struct binary *binary;
{
   nb_alloc_binary--;
   binary->is0._.subtree = binary_free_list;
   binary_free_list = binary;
}

/************************************************************************/

flush_binary()

{
   nb_alloc_binary = 0;
   for(; binary_block_list != NULL ; binary_block_list = temp_block)
    { temp_block = binary_block_list->next_block;
      free((char *)binary_block_list);
    }
   binary_free_list = NULL;
   start_binary_free = NULL;
   end_binary_free = NULL;
}

/*************************************************************************/
   
struct parent *alloc_parent()
{

/*  The allocation routines for the other kind of structures are similar
    to the ones for the nodes so for explanations refer to the previous
    section.								*/

   nb_alloc_parent++;
   if(parent_free_list != NULL)
    { temp_parent = parent_free_list;
      parent_free_list = parent_free_list->next_parent;
      temp_parent->next_parent = NULL;
      return(temp_parent);
    }

   if(start_parent_free < end_parent_free)
    { temp_parent = (struct parent *)start_parent_free;
      start_parent_free = start_parent_free + sizeof(struct parent);
      return(temp_parent);
    }

   temp_block = parent_block_list;
   parent_block_list = (struct block *)calloc(1,sizeof(struct block) 
		                 + MANY_PARENT * sizeof(struct parent));
   if(parent_block_list == NULL)fatal_system_error("unable to alloc parents");
   parent_block_list->next_block = temp_block;
   temp_parent = (struct parent *)parent_block_list->space;
   start_parent_free = parent_block_list->space + sizeof(struct parent);
   end_parent_free = parent_block_list->space 
					+ MANY_PARENT * sizeof(struct parent);
   return(temp_parent);
}

/******************************************************************************/

free_parent(parent)

struct parent *parent;
{
   nb_alloc_parent--;
   parent->next_parent = parent_free_list;
   parent_free_list = parent;
}

/************************************************************************/

#ifdef DEBUG

flush_parent()

{
   nb_alloc_parent = 0;
   for(; parent_block_list != NULL ; parent_block_list = temp_block)
    { temp_block = parent_block_list->next_block;
      free((char *)parent_block_list);
    }
   parent_free_list = NULL;
   start_parent_free = NULL;
   end_parent_free = NULL;
}

#endif

/*************************************************************************/

free_list_of_parents(list)

struct parent **list;
{
  temp_parent = *list;
  if(temp_parent == NULL) return;
  for( ; ; temp_parent = next_parent)
   { next_parent = temp_parent->next_parent;
     nb_alloc_parent--;
     if(next_parent == NULL) break;
   }
  temp_parent->next_parent = parent_free_list;
  parent_free_list = *list;
  *list = NULL;
}

