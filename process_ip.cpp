#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "process_ip.h"
#include "myshell.h"
using namespace std;



char** ProcessString_withPipe(char *in)
{
   char **piped_token=(char **)malloc(1024*sizeof(char *));
   if(piped_token==NULL)
   {
    cout<<"Memory alocation failed\n";
    exit(EXIT_FAILURE);
   }
   int i=0;
   char* str=strtok(in,"|");
  while(str!=NULL)
  {
   piped_token[i]=str;
   i++;
   str=strtok(NULL,"|");
  }
  piped_token[i]=NULL;
  //cout<<piped_token[0]<<" "<<piped_token[1]<<"inside fn\n";
  return piped_token;
}
char** ProcessString_withSpace(char *in)
{
  char **token_stream=(char**)malloc(1024*sizeof(char*));
  if(token_stream==NULL)
  {
   cout<<"Memory alocation failed\n";
   exit(EXIT_FAILURE);
  }
  char* str;
  int i=0;
  str=strtok(in," ");
  while(str!=NULL)
  {
   token_stream[i]=str;
   i++;
   str=strtok(NULL," ");
  }
  token_stream[i]=NULL;
  return token_stream;


}
char** ProcessString_withAppend(char *in)
{
     char **token_stream=(char**)malloc(1024*sizeof(char*));
     if(token_stream==NULL)
     {
        cout<<"Memory alocation failed\n";
        exit(EXIT_FAILURE);
     }
    char* str;
    int i=0;
    str=strtok(in,">>");
    while(str!=NULL)
    {
        token_stream[i]=str;
        i++;
        str=strtok(NULL,">>");
    }
    token_stream[i]=NULL;
    return token_stream;
}


char** ProcessString_withOverwrite(char *in)
{
     char **token_stream=(char**)malloc(1024*sizeof(char*));
     if(token_stream==NULL)
     {
        cout<<"Memory alocation failed\n";
        exit(EXIT_FAILURE);
     }
    char* str;
    int i=0;
    str=strtok(in,">");
    while(str!=NULL)
    {
        token_stream[i]=str;
        i++;
        str=strtok(NULL,">");
    }
    token_stream[i]=NULL;
    return token_stream;
}
char** ProcessString(char *in,int pipecount)
{

  char **piped_token_stream;
  char **piped_token_with_space=(char**)malloc(1024*sizeof(char*));

  if(piped_token_with_space==NULL)
  {
        cout<<"Memory alocation failed\n";
        exit(EXIT_FAILURE);
  }
  char* str;
  if(pipecount!=0)
  {  // cout<<pipecount<<"  pipe\n";
        piped_token_stream=ProcessString_withPipe(in);

        int index=0;
        for(int j=0;j<(pipecount+1);j++)
        {
            char** y=ProcessString_withSpace(piped_token_stream[j]);
            int pos=0;
            while(y[pos]!=NULL)
            {
                piped_token_with_space[index]=y[pos];
                index++;pos++;

            }
            piped_token_with_space[index]=NULL;


        }
        return piped_token_with_space;
  }
  else
  { 
      
      char** ab=ProcessString_withSpace(in);
      //cout<<"bye"<<ab[0][0];
      return ab;
  }

//return piped_token_with_space;

}