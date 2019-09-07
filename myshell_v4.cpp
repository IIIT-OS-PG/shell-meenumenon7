#include <iostream>
#include <fstream>
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

#define clear_scr() printf("\033[H\033[J")
#define HOME "/home/meenu"
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
unordered_map<string,string> aliasmap;

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
  {ir1.getline(l,500);
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
  fout<<"PATH="<<path<<"\n";
  fout<<"HOSTNAME="<<host<<"\n";
  

}


void print_history()
{
   for(int i=0;i<history.size();i++)
      cout<<history[i]<<endl;
}
void printPrompt()
{
   char *buf;
   buf= new char[1024];
   getcwd(buf,1024);
   cout<<"Myshell :"<<buf<<PS1;




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
  { 
      
      char** ab=ProcessString_withSpace(in);
      //cout<<"bye"<<ab[0][0];
      return ab;
  }

//return piped_token_with_space;

}
int executeAlias(char **processed_ip)
{
 char **token_stream=(char**)malloc(1024*sizeof(char*));
     if(token_stream==NULL)
     {
        cout<<"Memory alocation failed\n";
        exit(EXIT_FAILURE);
     }
    char* str;
    int i=0;
    str=strtok(processed_ip[1],"=");
    while(str!=NULL)
    {
        token_stream[i]=str;
        i++;
        str=strtok(NULL,"=");
    }
    token_stream[i]=NULL;
    
    
    string token2=string(token_stream[1]);
    if(token2[0]==39)
    token2=token2.substr(1);
    string token3,str3;
    if(processed_ip[2]!=NULL)
    {
       token3=string(processed_ip[2]);
       int l=token3.length();
       token3[l-1]='\0';

    }
    string key=token_stream[0];
    //cout<<"key "<<key;
    //cout<<"\nstr2"<<token2;
    //cout<<"\nstr3"<<token3;
    string finaltoken=token2+" "+token3;
    aliasmap[key]=finaltoken;
    
   
    return 0;

}

int check_custom(char **processed_ip)
{
    for(int i=0;i<6;i++)
    {   if(strcmp(processed_ip[0],custom[i])==0)
        {
           return 1;
        }
     }
  return 0;
}
void executeEcho(char **processed_ip)
{
  unordered_map<string,string>::iterator itr;
  itr=environment_var.find(processed_ip[1]);
  if(itr!=environment_var.end())
  {
    cout<<itr->second<<endl;
  }
  else
  {
    pid_t pid_child=fork();

    if(pid_child==-1)
    {
        cout<<"Failed forking the child process";
        
    }
    else if(pid_child==0)
    {
        int t=execvp(processed_ip[0],processed_ip);
        if(t<0)
        cout<<"Command not found!\n";
        
    }
    else
    {   wait(NULL);
        
    }  
    
  }


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
  if(strcmp(processed_ip[0],"alias")==0)
  {
    int x=executeAlias(processed_ip);
    return x;
  }
  if(strcmp(processed_ip[0],"history")==0)
  {
    print_history();
    return 0;
  }
  if(strcmp(processed_ip[0],"echo")==0)
  {  
    executeEcho(processed_ip);
    return 0;
    
  }
}
int check_for_alias(char **processed_ip)
{ 
  if(aliasmap.find(processed_ip[0])==aliasmap.end())
    return 0;
  else
    return 1;
}
int fetch_and_run_alias(char **processed_ip)
{
  unordered_map<string,string>::iterator itr=aliasmap.find(processed_ip[0]);
  string str=itr->second;
  char *a=new char[str.length()+1];
  strcpy(a,str.c_str());
  char **p=ProcessString_withSpace(a);
  pid_t pid_child=fork();

    if(pid_child==-1)
    {
        cout<<"Failed forking the child process";
        return 0;
    }
    else if(pid_child==0)
    {
        int t=execvp(p[0],p);
        if(t<0)
        cout<<"Command not found!\n";
        return 0;
    }
    else
    {   wait(NULL);
        return 0;
    }  

}
int executeCommand(char **processed_ip)
{ //cout<<"append 6767676"<<append;

  if(append!=0&&pipecount==0)
    {   int destn;
        pid_t pid_child=fork();

        if(pid_child==-1)
        {
            cout<<"Failed forking the child process";
           // return 0;
        }
       else if(pid_child==0)
        { //cout<<"INSIDE child!!!!!!!";
         destn=open(processed_ip[1],O_WRONLY | O_CREAT | O_APPEND , S_IRUSR|S_IWUSR);
        if(destn==-1)
         {
            perror("DESTN FILE ERROR");
            exit(0);
         }
        dup2(destn,STDOUT_FILENO);
        char** command=ProcessString_withSpace(processed_ip[0]);
        //cout<<"command "<<command[0]<<endl;
            int t=execvp(command[0],command);
            if(t<0)
            cout<<"Command not found!\n";
            //return 0;
        }
        else
        {   wait(NULL);
       // return 0;
        }
        return 0;
    }

  else if(overwrite!=0&&pipecount==0)
    {   int destn;
        pid_t pid_child=fork();

        if(pid_child==-1)
        {
            cout<<"Failed forking the child process";
           // return 0;
        }
       else if(pid_child==0)
        { //cout<<"INSIDE child!!!!!!!";
         destn=open(processed_ip[1],O_WRONLY | O_CREAT  , S_IRUSR|S_IWUSR);
        if(destn==-1)
         {
            perror("DESTN FILE ERROR");
            exit(0);
         }
        dup2(destn,STDOUT_FILENO);
        char** command=ProcessString_withSpace(processed_ip[0]);
        //cout<<"command "<<command[0]<<endl;
            int t=execvp(command[0],command);
            if(t<0)
            cout<<"Command not found!\n";
            //return 0;
        }
        else
        {   wait(NULL);
       // return 0;
        }
        return 0;
    }


  else if(pipecount==0&&append==0&&overwrite==0)
  {
    int al=check_for_alias(processed_ip);
    if(al==1)
    {  int val= fetch_and_run_alias(processed_ip);
       return val;
    }
    if(check_custom(processed_ip)==1)
    {   //cout<<"yes it is a custo command\n";
     int val=executeCustom(processed_ip);
     return val;
    }
    /*if(strcmp(processed_ip[1],"$$")==0)
    {
      cout<<myuid;
      return 0;
    }*/
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
 else if(pipecount>0&&append==0&&overwrite==0)
 { //cout<<"pipecount"<<pipecount<<endl;
    if(check_custom(processed_ip)==1)
  {
   cout<<"Error !this command is not allowed in pipe";
   return 0;
    //return val;
  }

  //int (*pipes_fd)[2] = (int (*)[2])calloc((pipecount+1) * sizeof(int[2]), 1);
  int pipes_fd[2];

        int file_des=0;
        int commandscount=pipecount+1;
        //cout<<"commands "<<commandscount;
       for (int k = 0; k <commandscount; k++) {

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
                char **a=ProcessString_withSpace(processed_ip[k]);

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
 else if(pipecount>0&&overwrite!=0)
 {
   if(check_custom(processed_ip)==1)
  {
   cout<<"Error !this command is not allowed in pipe";
   return 0;
    //return val;
  }
        
  //int (*pipes_fd)[2] = (int (*)[2])calloc((pipecount+1) * sizeof(int[2]), 1);
        int pipes_fd[2];
        
        int file_des=0;
        int commandscount=pipecount+1;
        //cout<<"commands "<<commandscount;
       for (int k = 0; k <commandscount; k++) {

          pipe(pipes_fd);

          pid_t child_pid = fork();

          if (child_pid == -1) {
                cout<< "error: fork error\n";
            return 0;
           }


          if (child_pid == 0) {
            //destn=open(processed_ip[1],O_WRONLY | O_CREAT  , S_IRUSR|S_IWUSR);
        /*if(destn==-1)
         {
            perror("DESTN FILE ERROR");
            exit(0);
         }*/
               dup2(file_des,0);
               if(k!=commandscount)
                    dup2(pipes_fd[1],STDOUT_FILENO);

                close(pipes_fd[0]);
                close(pipes_fd[1]);
                char **a=ProcessString_withSpace(processed_ip[k]);

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

}

void executeow(char **processed_ip)
{

int destn;
        pid_t pid_child=fork();

        if(pid_child==-1)
        {
            cout<<"Failed forking the child process";
           // return 0;
        }
       else if(pid_child==0)
        { //cout<<"INSIDE child!!!!!!!";
         destn=open(processed_ip[1],O_WRONLY | O_CREAT | O_APPEND , S_IRUSR|S_IWUSR);
        if(destn==-1)
         {
            perror("DESTN FILE ERROR");
            exit(0);
         }
        dup2(destn,STDOUT_FILENO);
        char** command=ProcessString_withSpace(processed_ip[0]);
        cout<<"command "<<command[0]<<endl;
            int t=execvp(command[0],command);
            if(t<0)
            cout<<"Command not found!\n";
            //return 0;
        }
        else
        {   wait(NULL);
       // return 0;
        }
}




void handleSignal(int signal_no) //why not working???
{
  signal(SIGINT,handleSignal);
  //fflush(stdout);

}
int main()
{
clear_scr();
init_bash();
signal(SIGINT,handleSignal);
while(1)
{   
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
        {processed_ip=ProcessString(in);}
   else if(pipecount>0)
        processed_ip=ProcessString_withPipe(in);
   else if(append>0)
        {processed_ip=ProcessString_withAppend(in);
         //executeow(processed_ip);
         }
   else if(overwrite>0)
        {processed_ip=ProcessString_withOverwrite(in);
         }
   int ret_val=executeCommand(processed_ip);
   if(ret_val==-1)
     break;
 }

return 0;
}
