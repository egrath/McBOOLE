/**************************************************************************

NAME
	cubes.h

PURPOSE
	This file contains the declarations for all the structures needed
	to contain the cubes and the pointers for all the program. Also
	all the variables that will be accessible by the main program
	and shared with the subroutines for cubes processing are defined
	as external. For all the files containing part of the program,
	the functions will be declared. 

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	12 june 1984

AUTHOR
	Michel DAGENAIS

***************************************************************************/

#include <stdio.h>

#define LEAF 1
#define SUBTREE 2
#define BASIC 1
#define NOT_BASIC 254
#define DONT_CARE 128
#define _0_ 2
#define _1_ 1
#define _X_ 3
#define INFINITY 32000

/* 

STRUCTURES
	we first define the structures that will be used all around
	in the program
									*/
/*
	Each cube will be a node in a graph, the structure node contains
	a cube plus different pointers to other nodes adjacents in the 
	graph.
									*/

struct node
 { struct node *next_node;	/* point to next node in the list */
   struct parent *ancestors;	/* point to the list of ancestors */
   struct parent *descendants;   /* point to the list of descendant */
   struct cube_list *uncovered;	/* part of node uncovered by retained nodes */
   short int status;		/* a short word for status information */
   short int cost;		/* number of non x input in the cube */
   long int count;		/* pass count when the node was last visited */
   long int cube[2];		/* cube of length to determine on allocation */
 };

/*
	When we want to determine which part of a cube is covered, we sharp
	a certain number of other cubes and the result is a list of cubes
	describing the uncovered part of the original cube. The structure
	cube_list holds one such cube plus a pointer to the next cube in
	the list
									*/

struct cube_list
 { struct cube_list *next_cube;/* point to next cube in the list */
   long int cube[2];		/* cube of length to determine on allocation */
 };

/*
	For the merging of impicants a new tree is used. The leaves
	of this tree are the node structures holding the cubes. The
	tree is composed of branches that point to leaves or to other
	branches. Each branch is for a certain input variable, all the cubes
	in that branch are directed depending on their value for that
	input to the 0 or 1 subtree. 
									*/
union cube_node
 { struct node *leaf;		/* only one cube in this subtree so it is
				   a leaf				  */
   struct binary *subtree;	/* there are more than one cube in the subtree
				   we point to another binary node.	  */
 };

struct branch
 { union cube_node _;
   short int status;
 };

struct binary
 { struct branch is0;		/* status and pointer to the 0 subtree or cube*/
   struct branch is1;		/* status and pointer to the 1 subtree */
   struct node *isx;		/* pointer to the list of merged cubes */
   short int var;		/* number of the input variable considered */
 };

/*
	The cubes in the graph are linked. Each node has a list of ancestors
	and descendants. Each element in this list is a structure parent
	containing a pointer to a related node plus a pointer to the next
	element in the ancestor or descendant list.
									*/

struct parent
 { struct node *parent;		/* point to a parent node */
   struct parent *next_parent;	/* point to the next parent in the list */
 };

/*

COMMON VARIABLES
	the variables described below will be used everywhere in the program
	and are listed file by file.
									*/

/*	Alloc.c		*/

struct node *alloc_node();	/* allocates a node */

int
	free_node(),		/* release a node */
	flush_node(),		/* release all the nodes */
	free_list_of_nodes();	/* release a list of nodes */

struct cube_list *alloc_cube_list();  /* allocates a cube_list structure */

int
	free_cube_list(),	/* release a cube */
	flush_cube_list(),	/* release all the cubes */
	free_list_of_cubes();	/* release a list of cubes */

struct binary *alloc_binary();	/* allocates a binary structure */

int
	free_binary(),		/* release a binary structure */
	flush_binary();		/* release them all */

struct parent *alloc_parent();	/* allocates a parent structure */

int
	free_parent(),		/* release a parent structure */
	flush_parent(),		/* release all the parents */
	free_list_of_parents();	/* release a list of parents */

extern int
	nb_alloc_parent,
	nb_alloc_cube_list,
	nb_alloc_binary,
	nb_alloc_nodes;

/*	Messages.c	*/

int
	fatal_user_error(),	/* user made a terrible mistake */
	fatal_system_error(),	/* you should buy a new computer */
	fatal_program_error(),	/* an assertion of the program got false */
	warning_user_error(),	/* tells the user he did a mistake */
	send_user_message(),	/* prints a message on the console */
	send_file_message(),	/* puts a comment in the output file */
	send_user_dtime(),	/* prints the cpu time since last call */
	send_file_dtime(),	/* same but to the output file */
	send_user_etime(),	/* total cpu time sent to terminal */
	send_file_etime();	/* same but to the output file */


/*	Inputcube.c	   */

extern int
	input_length,		/* number of long int to store input */
	output_length,		/* number of long int to store output */
	total_length,		/* number of long int to store a cube */
	input_number,		/* number of input var */
	output_number,		/* number of output var */
	total_number;		/* number of input and output var */

extern unsigned
	node_size,		/* number of bytes to store a node structure */
	cube_list_size;		/* number of bytes to store a cube_list struct*/

extern char
	error_buffer[132],	/* buffer to hold messages */
	input_literal[4],	/* literal accepted for input part */
	output_literal[4];	/* literal accepted for output part */

extern long int
	input_code[4],		/* internal codes corresponding to input lit. */
	output_code[4];		/* internal code for output literals */

extern struct node *spare_node;	/* a node is kept allocated for temp usage */

char ffind_car();		/* finds a character in a string */

int
	fread_bit_string(),	/* reads a part of a cube */
	fread_nodes();		/* reads a list of cubes */


/*	Outputcube.c	   */

int
	foutput_node_list(),	/* prints a list of nodes */
	foutput_node_vector(),	/* prints nodes in a vector */
	foutput_graph_list(),	/* prints cubes and links for a list of nodes */
	foutput_graph_vector(),	/* prints cubes and links for vector of nodes */
	foutput_cube(),		/* prints a cube */
	foutput_cube_list(),	/* prints a list of cubes */
	foutput_cube_and_links(), /* prints a cube and the links of the node */
	foutput_bit_string();	/* prints a part of a cube */


/* 	init.c	     */

int init_mask_and_codes();	/* set constants which are machine dependant */

extern int
	var_per_word,		/* number of variables in a long int */
	log2_var_per_word;	/* log2 (var_per_word) */ 

extern long int
	mask_bit_index,		/* mask the bits for var. pos in a word */
	mask00,			/* 000000000000 */
	mask01,			/* 01010101010101... */
	mask10,			/* 10101010101010... */
	mask11;			/* 111111111111 */


/* 	Incubesop.c	  */

int
	star_product(),		/* merge to cubes to form the biggest cube */
	covers(),		/* see if a cube covers another one */
	absorb(),		/* tells if a cube absorbs another */
	covers_list(),		/* see if a list is covered by a cube */
	intersect(),		/* see if two cubes intersects */
	intersect_list(),	/* see if a cube intersects with a list */
	disjoint_sharp();	/* substracts a cube from a list */


/* 	outcubesop.c	   */

int
	set_output_word(),	/* change the output part of a cube */
	or_output(),		/* or the output part of two cubes */
	and_output();		/* and the output part of two cubes */


/*	Setvar.c	*/

extern int
	current_var,		/* current variable */
	current_bit_index,	/* number of shift to extract the current var */
	current_word_index;	/* number of long int to skip to reach cur var*/
	
extern long int
	current_mask11,		/* contains 11 for current var and 0 else */
	current_mask10;		/* contains 10 for current var and 0 else */

int
	define_current_var(),	/* change the current variable */
	extract_current_var(),	/* returns the value of current var of a cube */
	extract_var(),		/* returns value of a var for a cube */
	set_current_var();	/* change the current var of a cube */


/*	Detect.c	*/

int
	count_code_in_cube(),	/* returns the number of times a code occurs */
	input_cost(),		/* returns the number of non-x code at input */
	output_cost(),		/* returns the number of 1 at output */
	detect_dont_care(),	/* see if dont care present at output */
	remove_dont_care(),	/* change dont care to 0 at output */
	detect_do_care(),	/* see if 1 present at output */
	remove_do_care(),	/* changes 1 into 0 at output */
	change_dont_to_do_care(); /* changes dont care to 1 at output */


/*	Lists.c	       */

struct node 
	*merge_node_lists(),	/* links two node lists together */
	*copy_and_alloc_node();	/* duplicates a node */

struct cube_list 
	*copy_and_alloc_cube_list(), /* duplicates a cube_list structure */
	*duplicate_cube_list();	/* duplicates a list of cubes */

int
	copy_cube();		/* copy a cube */


/*	Buildgraph.c	   */

struct node
	*merge_and_link();	/* place a new node in the graph */

int
	pass_ancestors(),	/* change the address of ancestors */
	pass_descendants(),	/* change the address of descendants */
	remove_ancestors(),	/* remove the links from the graph */
	absorb_and_unlink();	/* remove a node from the graph */


/*	Select.c	*/

int
	max(),			/* returns the max value of 2 int */
	select_input(),		/* using heuristic select an input */
	put_in_dont_care_list(), /* put the dont care part of a node in a list*/
	put_in_common_list();	/* change a tree structure into a list */


/*	Prime.c	       */

extern struct node
	*common_list,		/* convenient place to build a list */
	*dont_care_list,	/* holds the dont care cover of the function */
	**point_to_merge,	/* common pointer to a node to merge */
	*node_to_merge;		/* node that we currently try to merge */

extern struct binary
	*common_binary;		/* branch at which we are merging now */

extern int
	*nb0_at_input,		/* vector to count the 0 for each var */
	*nb1_at_input,		/* vector to count the 1 for each var */
	*nbx_at_input,		/* vector to count the x for each var */
	*unused_input,		/* vector containing the input not partitioned*/
	nbinput;		/* number of inputs not selected for part. */

struct node
	*prime_implicants_by_recursive_partitioning();	/* generate PIs */

int
	recursive_prime_implicants(),	/* find PIs of a subtree */
	scan_node_to_merge(),		/* merge all nodes of is0 with is1 */
	merge_with_rest();		/* try merge a node with all in is1 */


/*	Solve.c		*/

#define COVERED 2 
#define RETAINED 4
#define DECIDED 8
#define AFFECTED 48
#define AFFECTED_RETAINED 16
#define UNAFFECTED_RETAINED 239
#define AFFECTED_UNRETAIN 32
#define UNAFFECTED_UNRETAIN 223
#define DECIDED_RETAINED 14
#define DECIDED_COVERED 10
#define DECIDED_INFERIOR 8
#define PRIME_ESSENTIAL 64

int
	recursive_find_covering(),	/* find the best set of cubes */
	essential_prime_implicants();	/* find the essential prime imp. */

extern struct node 
	**prime_nodes,		/* vector containing pointer to all cubes */
	**retained_nodes,	/* pointer to end of retained nodes */
	**unretain_nodes,	/* pointer to start of unretained covered node*/
	**end_prime,		/* pointer to end of prime vector */
	**start_stack,		/* pointer to start of stack */
	**end_stack,		/* pointer to end of stack */
	**current_in_stack;	/* after last node entered in stack */

extern struct cube_list
	*scanned_cube;	/* uncovered part of cube currently processed */

extern struct node
	*scanned_node,		/* node currently processed */
	*current_node;		/* node in the graph reached by the scan */

extern int
	undecided_count,	/* number of undecided nodes remaining */
	scan_count,		/* number of nodes scanned for a partition */
	prime_count,		/* number of cubes in prime nodes vector */
	branching_depth;	/* depth in recursion while branching */

extern unsigned long int
	ONE,			/* a constant 1 unsigned long int */
	odd_pass_counter,	/* pass counter + 1 to identify scanning dir.*/
	pass_counter;		/* counts the number of scanning pass */


/*	Place.c	       */

int
	retain_node(),		/* retain a node and scan nodes affected */
	unretain_inferior_node(), /* unretain an inf. node and scan affected */
	place_nodes_in_vector(), /* order the nodes in a vector by status */
	select_node(),		/* upon cycle choose a node to retain */
	increment_pass_count(),	/* increment the pass counter */
	init_pass_count(),	/* when pass counter overflows reinit all */
	push();			/* place an affected node on the stack */

struct node
	*pop();			/* take next node from the stack */


/*	Scan.c		*/

int
	scan_affected_retained_ancestors(),	/* recursive routines to scan */
	scan_sffected_retained_descendants(),	/* nodes in the graph         */
	scan_affected_inferior_ancestors(),
	scan_affected_inferior_descendants(),
	scan_inferior_descendants(),
	scan_inferior_ancestors(),
	scan_essential_descendants(),
	scan_essential_ancestors(),
	scan_partition(),
	scan_sparse_ancestors(),
	scan_sparse_descendants(); 


/*	Mcboole.c	*/

extern FILE
	*output_file;		/* file on which we print the result */

extern int
	EPI_LIST,
	DISJOINT_REQUIRED,	/* nodes at input should be disjoint */
	VERBOSIS,		/* give plenty of messages to user */
	VERY_VERBOSIS,		/* even more messages */
	DONT_MIN_LITERAL,	/* minimize only the number of product term */
	max_branching_depth,	/* deepest braching depth reached */
	depth_limit;		/* depth limit allowed */

extern char
	read_interminator,	/* character that ends input part for reading*/
	read_outterminator,	/* character that ends the cube when reading */
	*print_interminator,	/* string printed after input part */
	*print_outterminator;	/* string printed after output part */


/*

EXTERNAL FUNCTIONS CALLED
	all the system functions having a special type are defined here.

									*/

#ifdef UNIX
char
	*calloc(),		/* routine to allocate space */
	*sprintf();		/* routine to write in a string */
#endif
