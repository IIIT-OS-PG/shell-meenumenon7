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
#define HOME "/home/meenu"
using namespace std;
static char** history;
static char* custom[]={"cd","exit","help"};
int pipecount=0;
void to_history(char *command)
{

}
void printPrompt()
{
   char *buf;
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
      if(c=='|')
        pipecount++;
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
  //pipecount++;
  s[i]=NULL;
  return s;

}
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
  cout<<piped_token[0]<<" "<<piped_token[1]<<"inside fn\n";
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
char** ProcessString(char *in)
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
  { // cout<<"hello\n";
      char** ab=ProcessString_withSpace(in);
      //cout<<"bye"<<ab[0][0];
      return ab;
  }

//return piped_token_with_space;

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
int executeCustom(char **processed_ip)
{
  if(strcmp(processed_ip[0],"cd")==0)
  {
     if(strcmp(processed_ip[1],"~")==0)
     {
        chdir(HOME);
     }
     else
     {
     int ret=chdir(processed_ip[1]);
     if(ret<0)
     {
       cout<<"Failed to change directory\n";
     }
     }
     return 0;

  }
  if(strcmp(processed_ip[0],"exit")==0)
  {
     return -1;

  }
}
int executeCommand(char **processed_ip)
{

  if(pipecount==0)
  {
    if(check_custom(processed_ip)==1)
    {
     int val=executeCustom(processed_ip);
     return val;
    }

    pid_t pid_child=fork();

    if(pid_child==-1)
    {
        cout<<"Failed forking the child process";
        return 0;
    }
    else if(pid_child==0)
    {
        int t=execvp(processed_ip[0],processed_ip);
        if(t<0)
        cout<<"Command not found!\n";
        return 0;
    }
    else
    {   wait(NULL);
        return 0;
    }
 }
 else
 { cout<<"pipecount"<<pipecount<<endl;
    if(check_custom(processed_ip)==1)
  {
   cout<<"Error !this command is not allowed in pipe";
   return 0;
    //return val;
  }
   cout<<"not custom";
  //int (*pipes_fd)[2] = (int (*)[2])calloc((pipecount+1) * sizeof(int[2]), 1);
  int pipes_fd[2];

        int file_des=0;
        int commandscount=pipecount+1;
        cout<<"commands "<<commandscount;
       for (int k = 0; k <commandscount; k++) {
       //int y=executeCommand_piped(processed_ip,pipes_fd,k);
          cout<<"pip"<<processed_ip[k]<<" "<<k;
          //cout<<processed_ip[1];
          pipe(pipes_fd);

         pid_t child_pid = fork();

        if (child_pid == -1) {
                cout<< "error: fork error\n";
		return 0;
        }


        if (child_pid == 0) {


        dup2(file_des,0);
        if(k+1<commandscount)
            dup2(pipes_fd[1],STDOUT_FILENO);
        close(pipes_fd[0]);
        close(pipes_fd[1]);
        /*cout<<"first "<<processed_ip[0];
        cout<<"\nsec "<<processed_ip[1];
        cout<<"\n3rd "<<processed_ip[2];*/
         char **a=ProcessString_withSpace(processed_ip[k]);
          cout<<a[0]<<endl;
          cout<<a[1];
         if(execvp(a[0], a)==-1)
            cout<<"Command cannot  be executed\n";



        }
        else
        {
          wait(NULL);
          file_des=pipes_fd[0];
           close(pipes_fd[1]);
        }


   }
return 0;




 }

//}
}


void handleSignal(int signal_no) //why not working???
{
  signal(SIGINT,handleSignal);
  //fflush(stdout);

}
int main()
{
clear_scr();
signal(SIGINT,handleSignal);
while(1)
{
   printPrompt();
   pipecount=0;
   char *in=getInput();
   if(strlen(in)==0)
    continue;
    char **processed_ip;
   if(pipecount==0)
        processed_ip=ProcessString(in);
   else
     processed_ip=ProcessString_withPipe(in);
   //char** processed_ip=ProcessString_withSpace(in);
//   cout<<"main "<<processed_ip[0]<<processed_ip[1]<<processed_ip[2]<<processed_ip[3];
  int ret_val=executeCommand(processed_ip);
   if(ret_val==-1)
     break;
 }

return 0;
}
