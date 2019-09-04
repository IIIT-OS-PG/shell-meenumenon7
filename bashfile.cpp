#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#define clear_scr() printf("\033[H\033[J")
using namespace std;
static char** history;
static char* custom[]={"cd","exit","help"};
void to_history(char *command)
{

}
void printPrompt()
{
   char *buf;
   //size_t size;
   buf= new char[1024];
   getcwd(buf,1024);
   cout<<"Myshell  :"<<buf<<":$";



}
char* getInput()
{ int sz=1024,i=0;
  char c;
  char *s=(char*)malloc((sizeof(char))*1024);
  if(s==NULL)
  { //fprintf(stderr, "lsh: allocation error\n");
    cout<<"ERROR memory allocation failed";
    exit(EXIT_FAILURE);
  }
  while((c=getchar())!='\n')
  {  //cout<<"c "<<c<<endl;
     if(c==EOF)
     { free(s);
       return NULL;

     }
     s[i++]=c;
     //cout<<s[i-1];
     if(i>=sz)
     { sz=2*sz;
     s=(char *)realloc(s,sz);
     }
     if(s==NULL)
     {
      cout<<"ERROR memory allocation failed";
      exit(EXIT_FAILURE);
     }
  }
  s[i]=NULL;
  return s;

}
char** ProcessString(char *in)
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
int check_custom(char **processed_ip)
{
    for(int i=0;i<3;i++)
    {   if(strcmp(processed_ip[0],custom[i])==0)
        {
           return 1;
        }
     }
  return 0;
}
/*void executeCustom(char **processed_ip)
{

}*/
void executeCommand(char **processed_ip)
{
  if(check_custom(processed_ip)==1)
  {
    executeCustom(char **processed_ip);
  }

  pid_t pid_child=fork();

  if(pid_child==-1)
  {
     cout<<"Failed forking the child process";
     return;
  }
  else if(pid_child==0)
  {
   int t=execvp(processed_ip[0],processed_ip);
   if(t<0)
        cout<<"Command not found!\n";
    return;
  }
  else
  {   wait(NULL);
      return;
   }
}
void handleSignal(int signal_no)
{
  signal(SIGINT,handleSignal);
  fflush(stdout);

}
int main()
{
clear_scr();
signal(SIGINT,handleSignal);
while(1)
{
   printPrompt();
   char *in=getInput();
   char **processed_ip=ProcessString(in);
   executeCommand(processed_ip);
}
return 0;
}
