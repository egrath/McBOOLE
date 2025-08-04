/********************************************************************

NAME
	McBOOLE

PURPOSE
	This program does the minimization of a multiple output boolean
	function. It guarantees to give a solution which contains the
	lowest possible number of product terms. Also it gives cubes
	with irredundant output part in the sense that no output bit
	can be removed without modifying the function.

SYNOPSIS
	mcboole [...........]

DESCRIPTION
	Many switches in the standard UNIX format are avalaible.
	The most important ones are described below. When no
	switch are given ( strings not preceeded by a - ) it is taken 
	as the file names for input and output. If no files are specified
	the data is read on stdin and the output goes on stdout.

     -i input file to minimize (default extension .in if none given)

     -o output file with solution (default extension .out) 

     -io name of both input and output files ( with the extension .in and .out)

     -b maximum branching depth allowed. If the cycles have a greater depth,
	the user will be warned and a heuristic solution will be taken.

     -rit input terminator of cubes in the input file. It is : by default.

     -rot output terminator of cubes in the input file. It is ; by default

     -pit input terminator of cubes for the output. It is ' : ' by default.

     -pot output terminator of cubes for the output. It is ' ;\n' by default.

     -nint when set, intersecting cubes at input will not be accepted.

     -min when set, the program will minimize not only the number of product
	  terms but will also give the minimal number of literals at input.
	  This costs however some additional cpu time.

     -v   The program will put itself in verbose mode and give a lot of
	  interesting information.

     -vv  The program gets very verbosis.

	The program was developped at McGill university by Michel Dagenais
        He can now be reached at dagenais@vlsi.polymtl.ca .

COORDINATES
	McGill University Electrical Engineering VLSI lab MONTREAL CANADA
	17 august 1984

AUTHOR
	Michel DAGENAIS

**********************************************************************/

#include <stdio.h>
#include "cubes.h"
#include "param.h"

FILE *input_file, *output_file;

int 
	EPI_LIST,		/* tells if we should list the epi cubes */
	DISJOINT_REQUIRED = 0,  /* tells if cubes should be disjoint at input */
	VERBOSIS = 0,		/* puts the program in verbose mode */
	VERY_VERBOSIS = 0,	/* puts it more verbose */
	DONT_MIN_LITERAL = 1,	/* minimize only the number of product terms */
	max_branching_depth,	/* maximum branching depth reached */
	depth_limit = 10;	/* maximum branching depth allowed */

char
	read_interminator = ' ',	/* input terminator for cubes at input*/
	read_outterminator = '\n',	/* output terminator of cubes at input*/
	*print_interminator = " ",	/* input terminator for output file */
	*print_outterminator = "\n";	/* output terminator for output file */

struct p_file
	p1 = {"in","r",&input_file},
	p2 = {"out","w",&output_file};

struct p_2file
	p3 = {"in","r","out","w",&input_file,&output_file};

struct p_integer
	p4 = {0,16,&depth_limit};

struct p_logical
	p5 = {&DISJOINT_REQUIRED},
	p6 = {&VERBOSIS},
	p6a = {&VERY_VERBOSIS},
	p7 = {&DONT_MIN_LITERAL},
	p100 = {&EPI_LIST};

struct p_character
	p8 = {&read_interminator},
	p9 = {&read_outterminator};

struct p_string
	p10 = {1,10,&print_interminator},
	p11 = {1,10,&print_outterminator};

struct parameter parmv[]
   = { "rit","input terminator read",P_CHARACTER,0,0,P_OPTIONAL,(char *)&p8,
       "rot","output terminator read",P_CHARACTER,0,0,P_OPTIONAL,(char *)&p9,
       "pit","input terminator print",P_STRING,0,0,P_OPTIONAL,(char *)&p10,
       "pot","output terminator print",P_STRING,0,0,P_OPTIONAL,(char *)&p11,
       "nint","intersection not accepted",P_LOGICAL,0,0,P_OPTIONAL,(char *)&p5,
       "vv","very verbose mode",P_LOGICAL,0,0,P_OPTIONAL,(char *)&p6a,
       "v","verbose mode",P_LOGICAL,0,0,P_OPTIONAL,(char *)&p6,
       "min","literal minimization",P_LOGICAL,0,0,P_OPTIONAL,(char *)&p7,
       "io","input and output",P_2FILE,9,10,P_PRESENT_A_EXCL + P_PRESENT_B_EXCL,
				(char *)&p3,
	"i","input",P_FILE,0,0,P_OPTIONAL + P_DEFAULT,(char *)&p1,
	"o","output",P_FILE,0,0,P_OPTIONAL + P_DEFAULT,(char *)&p2,
	"b","branching depth",P_INTEGER,0,0,P_OPTIONAL,(char *)&p4,
	"n","non disjoint cubes",P_LOGICAL,0,0,P_OPTIONAL,(char *)&p5,
	"epi","list the epi cubes",P_LOGICAL,0,0,P_OPTIONAL,(char *)&p100};

int parmc = sizeof(parmv) / sizeof(struct parameter);
	
main(argc,argv)

int argc;
char **argv;
{
  input_file = stdin;
  output_file = stdout;

  struct node 
	*temp_node,	/* temp pointer in the list */
	*list;		/* list of nodes to minimize */

  int 
	nb_nodes,		/* number of nodes read */ 
	input_literal,		/* number of literal at input */
	output_literal;		/* number of literal at output */
  
/* We call the very nice function that handles the input of all the 
   parameters and provides the appropriate messages.		*/

  param(parmc,parmv,argc,argv);
  if(VERY_VERBOSIS) VERBOSIS = 1;

  (void)fread_nodes(input_file,&list);

/* We will compute the number of literal in the initial solution.  */

  temp_node = list;
  nb_nodes = 0;
  input_literal = 0;
  output_literal = 0;
  for(; temp_node != NULL ; temp_node = temp_node->next_node)
   { nb_nodes++;
     input_literal += input_cost(temp_node->cube);
     output_literal += output_cost(temp_node->cube);
   }
  sprintf(error_buffer,"%d nodes, var : in %d out %d literal : in %d out %d",
	     nb_nodes,input_number,output_number,input_literal,output_literal);
  if(VERBOSIS)send_user_dtime(error_buffer);
  send_file_dtime(error_buffer);

/* We will find the prime implicants */

  list = prime_implicants_by_recursive_partitioning(list);

/* We will select a set of prime implicants to cover the function. The
   function will also give some information on the number of prime implicants,
   the number of essential implicants and so on when the verbose mode is
   activated.							           */

  find_best_covering(list);

/* The final solution is in the vector Prime_nodes which is an external
   variable. The nodes retained are in the beginning of the vector and
   go up to the pointer Retained_nodes.					*/

  foutput_node_vector(output_file,prime_nodes,retained_nodes);

/* The program is finished, we print the total CPU time elapsed and the 
   maximum branching depth reached.					*/

  if(max_branching_depth < INFINITY)
   { sprintf(error_buffer,"end, max branching depth reached : %d",
	     max_branching_depth);
   }
  else
   { sprintf(error_buffer,
     "branching limit reached : %d, best solution not garanteed",depth_limit);
   }
  if(VERBOSIS)send_user_etime(error_buffer);
  send_file_etime(error_buffer);
}

