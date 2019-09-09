#include <iostream>
#include <fstream>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unordered_map>
#include "myshell.h"
#include "process_ip.h"
#include "execute_ip.h"


#define clear_scr() printf("\033[H\033[J")
//#define HOME "/home/meenu"
using namespace std;
static vector<string> history;
unordered_map<string,string> environment_var;
static char* custom[]={"cd","exit","help","alias","history","echo"};
int pipecount=0;
int overwrite=0;
int append=0;
int appendflag=0;
pid_t mypid;
char PS1='$';
//unordered_map<string,string> aliasmap;

void init_bash()
{ 
  mypid=getpid();
  string s=to_string(mypid);
  environment_var["PS1"]="$";
  environment_var["$$"]=s;
  pid_t myuid=getuid();
  

  ifstream ir1;
  ir1.open("/etc/environment");
  char l[500];
  ir1.getline(l,500);
  string path=l;
  environment_var["$PATH"]=l;
  ir1.close();
  
  ir1.open("/etc/hostname");
  ir1.getline(l,500);
  environment_var["$HOSTNAME"]=l;
  string host=l;
  ir1.close();
  
  ir1.open("/etc/passwd");
  int lineno=1;
  while(lineno!=44)
  {	ir1.getline(l,500);
    lineno++;
  }
  ir1.close();
  string str,home[7];
  int i=0;
  str=strtok(l,":");
  home[i]=str;
  while(i!=6)
  {
   	home[i]=str;
   	i++;
   	str=strtok(NULL,":");
  }
  //cout<<home[5];
  environment_var["$HOME"]=home[5];

  ofstream fout;
  fout.open("mybashrc.txt");
  fout<<"PS1 ="<<"$"<<"\n";
  fout<<"uid ="<<s<<"\n";
  fout<<"HOME="<<home[5]<<"\n";
  fout<<path<<"\n";
  fout<<"HOSTNAME="<<host<<"\n";
  

}

#include <iostream>
#include <fstream>

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
     int bs=0;
     //if(c==9)
      //cout<<"tab!! tab!!\n";
     /*if(c=='^')
        bs=1;
      if(c=='?'&&bs==1);
      {cout<<"backspace\n";bs=0;}*/
      if(c=='|')
        pipecount++;
      else if(appendflag)
      {
          if(c=='>')
          {  append++;
             appendflag=0;
           }
          else
             overwrite++;
         appendflag=0;
       }
      else if(c=='>')
        appendflag=1;


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
  history.push_back(s);
  return s;

}

void printPrompt()
{
   char *buf;
   buf= new char[1024];
   getcwd(buf,1024);
   cout<<"Myshell :"<<buf<<PS1;




}


void enable_noncanonical()
{
  struct termios t;
  tcgetattr(STDOUT_FILENO,&t);
  t.c_lflag &= ~(ICANON);
  tcsetattr(STDIN_FILENO,TCSAFLUSH,&t);
}


void handleSignal(int signal_no) //why not working???
{
  signal(SIGINT,handleSignal);
  //fflush(stdout);
  return;

}


int main()
{
  enable_noncanonical();
  clear_scr();
  init_bash();
  signal(SIGINT,handleSignal);
while(1)
{  init_bash();
   printPrompt();
   pipecount=0;
   appendflag=0;
   append=0;
   overwrite=0;
   char *in=getInput();
   if(strlen(in)==0)
    continue;
   
    char **processed_ip;
   if(pipecount==0&&append==0&&overwrite==0)
       	processed_ip=ProcessString(in,pipecount);
   
   else if(pipecount>0&&overwrite==0)
        processed_ip=ProcessString_withPipe(in);
   
   else if(append>0)
        processed_ip=ProcessString_withAppend(in);
         
   else if(overwrite>0&&pipecount==0)
        processed_ip=ProcessString_withOverwrite(in);
   else if(overwrite>0&&pipecount>0)
    	processed_ip=ProcessString_withOverwrite(in);
    
         
   int ret_val=executeCommand(processed_ip, pipecount, append, overwrite, environment_var,history,custom);
   if(ret_val==-1)
     break;
 }

return 0;
}
