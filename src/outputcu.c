#include "cubes.h"

/****************************************************************************

NAME
	foutput_node_list, foutput_node_vector,
	foutput_graph_list, foutput_graph_vector

PURPOSE
	These function print list or vector of cubes to a specified file.
	Node functions print straight cube, as Graph function prints the 
	links of the node and its address.

SYNOPSIS
	foutput_node_list(fp,list)
	FILE *fp;
	struct node *list;

	foutput_node_vector(fp,start,end)
	FILE *fp;
	struct node **start, **end;

	foutput_graph_list(fp,list)
	FILE *fp;
	struct node *start_list;

	foutput_graph_vector(fp,start,end)
	FILE *fp;
	struct node **start, **end;

DESCRIPTION

    -foutput_node_list prints a list of nodes on the file fp calling the 
	function foutput_cube for each node.

    -foutput_node_vector prints nodes whose pointers are in a vector delimited
	by the addresses start and end. For each node the function foutput_cube
	is called.

    -foutput_graph_list calls the function foutput_cube_and_links for each
	node in the list.

    -foutput_graph_vector calls the function foutput_cube_and_links for
	all the nodes in the vector delimited by start and end.

DIAGNOSTICS
	If the cubes are very big, they will be truncated when displayed
	on the screen but they will be correctly written in the file
	since there are no limitations on the line length for the files.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

#ifdef DEBUG

foutput_node_list(fp,list)

FILE *fp;
struct node *list;
{
  for(; list != NULL ; list = list->next_node )
   { foutput_cube(fp,list->cube);
   }
}

#endif

/**************************************************************************/

foutput_node_vector(fp,start,end)

FILE *fp;
struct node **start, **end;
{
  for(; start < end ; start++)
   { foutput_cube(fp,(*start)->cube);
   }
}

/**************************************************************************/

#ifdef DEBUG

foutput_graph_list(fp,list)

FILE *fp;
struct node *list;
{
  fprintf(fp,"/*\n");
  for(; list != NULL ; list = list->next_node)
   { foutput_cube_and_links(fp,list);
   }
  fprintf(fp,"*/\n");
}

/****************************************************************************/

foutput_graph_vector(fp,start,end)

FILE *fp;
struct node **start, **end;
{
  fprintf(fp,"/*\n");
  for(; start < end ; start++)
   { foutput_cube_and_links(fp,*start);
   }
  fprintf(fp,"*/\n");
}

#endif

/****************************************************************************

NAME
	foutput_cube, foutput_cube_list, foutput_cube_and_links

PURPOSE
	Print on the specified file a cube. A function is also provided
	to print all the links of the cube when it is part of a covering
	graph.

SYNOPSIS
	foutput_cube(fp,cube)
	long int *cube;

	foutput_cube_list(fp,list)
	struct cube_list *list;

	foutput_cube_and_links(fp,node)
	struct node *node;

DESCRIPTION

    -foutput_cube receives a cube and prints it on the file fp. The cube
	is printed with its input part, the input terminator, the output part,
	the output terminator.

    -foutput_cube_list prints a list of cubes List to the file Fp. It simply
	calls the function foutput cube for each cube in the list.

    -foutput_cube_and_links prints the cube of the node with the function
	foutput cube. Also it prints on fp the address of the node and the
	addresses of all its ancestors and descendants in the covering graph.
	The status of the node and its uncovered part are also printed.

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	15 august 1984

AUTHOR
	Michel DAGENAIS

*************************************************************************/

foutput_cube(fp,cube)

FILE *fp;
long int *cube;
{
  foutput_bit_string(fp,cube,input_number,input_literal,
                     input_code);
  fprintf(fp,print_interminator);
  foutput_bit_string(fp,cube + input_length,output_number,
		     output_literal,output_code);
  fprintf(fp,print_outterminator);
}

/**************************************************************************/

foutput_cube_list(fp,list)

FILE *fp;
struct cube_list *list;
{
  for(; list != NULL ; list = list->next_cube )
   { foutput_cube(fp,list->cube);
   }
}

/****************************************************************************/

#ifdef DEBUG

foutput_cube_and_links(fp,node)

FILE *fp;
struct node *node;
{
  struct parent *temp_parent;

  fprintf(fp,"address : %lu, status : %d, count %lu, ",node,node->status,
							node->count);
  foutput_cube(fp,node->cube);
  foutput_cube_list(fp,node->uncovered); 
  fprintf(fp,"ancestors : ");
  temp_parent = node->ancestors;
  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { fprintf(fp,"%lu ",temp_parent->parent);
   }
  fprintf(fp,"\ndescendants : ");
  temp_parent = node->descendants;
  for(; temp_parent != NULL ; temp_parent = temp_parent->next_parent)
   { fprintf(fp,"%lu ",temp_parent->parent);
   }
  fprintf(fp,"\n");
}

#endif

/***************************************************************************

NAME
	foutput_bit_string

PURPOSE
	This function outputs a bit string packed into long int words.
	the 2 bits pattern are translated to the corresponding literal.

SYNOPSIS
	foutput_bit_string(fp,word,length,literal,code)
	FILE *fp;
	long int *word,code[4];
	int length;
	char literal[4];

DESCRIPTION
	The Word are scanned until Length variables are extracted. For
	each 2 bits representing a variable we output the Literal in the
	table corresponding to the Code in the table. These literals are
	output in a string on the file Fp.

DIAGNOSTICS
	If a code is not in the table, a message for invalid internal
	code found will be issued, such a situation should never happen
	unless the calling program is bugged since codes should be correct
	and checked during the input phase and preserved during all 
	operations performed on cubes.

COORDINATES
	McGill University Electrical Engineering VLSI Lab MONTREAL CANADA
	19 june 1984

AUTHOR
	Michel DAGENAIS

************************************************************************/

foutput_bit_string(fp,word,length,literal,code)

FILE *fp;
long int *word,*code;
int length;
char *literal;
 {
   long int 
	mask,		/* mask to read a particular variable in a long word */
	mask_word;	/* temp variable to store the cube masked for a var */

   int i;		/* loop counter for finding the literal for a code */

   for(; length > 0 ;)

/* We mask the variables in the long words starting from the right with a
   mask of 0000000011 (3). When a variable is printed we go to the next one
   by shifting the mask 2 bits left to the next variable to examine.
   We have length variable to examine that may occupy many words. When the
   mask goes to 0, we reached the end of a word, we then increment the word
   pointer and start again with a mask of 000000011.			*/

    { for( mask = 3 ; mask != NULL && length > 0 ; mask = mask << 2)
 
       { mask_word = *word & mask;

/* we scan the code table to find which code was isolated in mask word */

	 for(i = 0 ; i < 4 ; i++) if(mask_word == (mask & code[i])) break;
        
/* i contains the index of the code found. if the code was not found in the
   table a very severe error is signaled.				*/

	 if(i == 4) fatal_program_error("internal code error for a bit");

/* the code is recognized, we simply output the corresponding literal on the
   output file.							*/

	putc(literal[i],fp);
        length--;
       }
      word++;
    }
 }
