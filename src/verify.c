/********************************************************************

NAME
	This program calls all the functions in cube.c and test each 
	of them touroughly before any algorithm is to be implemented 
	using this library.

PURPOSE
	debug completely cubes.c

**********************************************************************/

#include "cubes.h"
#include "param.h"

FILE *input_file1, *input_file2, *output_file;

struct p_file
	p1 = {"in","r",&input_file1},
	p2 = {"in","r",&input_file2};

char 
	read_interminator = ' ',
	read_outterminator = '\n',
	*print_interminator = " ",
	*print_outterminator = "\n";

struct p_character
	p3 = {&read_interminator},
	p4 = {&read_outterminator};

struct parameter parmv[]
     = {"i1","input 1",P_FILE,0,0,P_NEEDED + P_DEFAULT,(char *)&p1,
	"i2","input 2",P_FILE,0,0,P_NEEDED + P_DEFAULT,(char *)&p2,
	"rit","input terminator",P_CHARACTER,0,0,P_OPTIONAL,(char *)&p3,
	"rot","output terminator",P_CHARACTER,0,0,P_OPTIONAL,(char *)&p4};

int parmc = sizeof(parmv) / sizeof(struct parameter);

int
	input_number,
	output_number,
	total_number;

char error_buffer[132];

struct node *spare_node;
	
main(argc,argv)

int argc;
char **argv;
{
  struct node *list1, *list2, *temp1, *temp2;

  struct cube_list *temp_cube, *temp_cube2;

  int i, error1, error2;
  
  param(parmc,parmv,argc,argv);

  i = fread_nodes(input_file1,&list1);

  fprintf(stdout,"%d nodes read from the first file\n",i);

  i = fread_nodes(input_file2,&list2);

  fprintf(stdout,"%d nodes read from the second file\n",i);
  send_user_dtime(stdout,"both files are read");

  temp1 = list1;
  for(; temp1 != 0 ; temp1 = temp1->next_node)
   { temp_cube = copy_and_alloc_cube_list(temp1->cube);
     temp2 = list2;
     for(; temp2 != 0 ; temp2 = temp2->next_node)
      { if(disjoint_sharp(&temp_cube,temp2->cube) < 0) break;
      }
     if(temp2 == 0)
      { temp_cube2 = temp_cube;
	for(; temp_cube2 != 0 ; temp_cube2 = temp_cube2->next_cube)
	 { if(detect_do_care(temp_cube2->cube)) break;
	 }
	if(temp_cube2 == 0) continue;
	fprintf(stdout,"One cube of the first file is not covered");
	fprintf(stdout,"\n\nuncovered cube : ");
	foutput_cube(stdout,temp1->cube);
	fprintf(stdout,"\nuncovered part of it :\n");
	foutput_cube_list(stdout,temp_cube);
	fprintf(stdout,"\n");
	error1 = 1;
	free_list_of_cubes(&temp_cube);
      }
   }

  if(error1 == 0)
   { fprintf(stdout,"the first file is covered by the second\n");
   }

  temp2 = list2;
  for(; temp2 != 0 ; temp2 = temp2->next_node)
   { temp_cube = copy_and_alloc_cube_list(temp2->cube);
     temp1 = list1;
     for(; temp1 != 0 ; temp1 = temp1->next_node)
      { if(disjoint_sharp(&temp_cube,temp1->cube) < 0) break;
      }
     if(temp1 == 0)
      { temp_cube2 = temp_cube;
	for(; temp_cube2 != 0 ; temp_cube2 = temp_cube2->next_cube)
	 { if(detect_do_care(temp_cube2->cube)) break;
	 }
	if(temp_cube2 == 0) continue;
        fprintf(stdout,"One cube of the second file is not covered");
	fprintf(stdout,"\n\nuncovered cube : ");
	foutput_cube(stdout,temp2->cube);
	fprintf(stdout,"\nuncovered part of it :\n");
	foutput_cube_list(stdout,temp_cube);
	fprintf(stdout,"\n");
	error2 = 1;
	free_list_of_cubes(&temp_cube);
      }
   }

  if(error2 == 0)
   { fprintf(stdout,"the second file is covered by the first\n");
   }
	 
  if(error1 == 0 && error2 == 0)
   { fprintf(stdout,"Both files describe strictly equivalent functions\n");
   }
  send_user_dtime(stdout,"the covering was verified");
}

