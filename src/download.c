#include <stdio.h>
#include <time.h>

delay(int nr_sec)
{
int wait_til_time,current_time;

time(&current_time);
wait_til_time = current_time + nr_sec;
do {time(&current_time);} while (current_time < wait_til_time);
}

main(int argc,char *argv[])
{
FILE *infile;
int ch;

if (argc == 1) 
   {printf("To download, type 'DWN filename <CR>' (.S extn assumed)\n"); }
else
{ 
  strncat(argv[1],".S",2);
  infile = fopen(argv[1],"r");
  if (infile == NULL) 
     {printf("Cannot open file %s\n",argv[1]);
      return(1);
     }
  while ((ch = fgetc(infile)) != EOF)
   {  
        if (ch == 0x0a) 
         {
          ch = fgetc(infile);
          if (ch != 'S')
             {
               ungetc(ch,infile);     
             }
          delay(1); 
          printf("\nS");
         }    
      else
         printf("%c",ch);   
    }

}
}
