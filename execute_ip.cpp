#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unordered_map>
#include "process_ip.h"
#include "myshell.h"
#include "execute_ip.h"
using namespace std;
unordered_map<string,string> aliasmap;



void print_history(vector<string> history)
{  
    vector<int>::iterator it; 
    
    for (auto it = history.begin(); it != history.end(); ++it) 
        cout << *it<<endl; 
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
    if(token2[0]==39)//ascii for '
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

int check_custom(char **processed_ip,char * custom[])
{
    for(int i=0;i<6;i++)
    {   if(strcmp(processed_ip[0],custom[i])==0)
        {
           return 1;
        }
     }
  return 0;
}
void executeEcho(char **processed_ip,unordered_map<string,string> environment_var)
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
int executeCustom(char **processed_ip,unordered_map<string,string> environment_var,vector<string> history,char *custom[])
{
  if(strcmp(processed_ip[0],"cd")==0)
  {
     if(strcmp(processed_ip[1],"~")==0)
     {
        unordered_map<string,string>::iterator itr=environment_var.find("$HOME");
        string str=itr->second; 
        char * c= new char [str.length()+1];
        strcpy (c, str.c_str());
        
        chdir(c);
     }
     else
     {
         if(processed_ip[1]!=NULL)
        {
        
          int ret=chdir(processed_ip[1]);
          if(ret<0)
          {
            cout<<"Failed to change directory\n";
          }
        }

        else 
          {
            cout<<"Please mention a directory \n";
            
          }
        return 0;
     }

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
    print_history(history);
    return 0;
  }
  if(strcmp(processed_ip[0],"echo")==0)
  {  
    executeEcho(processed_ip,environment_var);
    return 0;
    
  }
}
int check_for_alias(char **processed_ip)
{ 
  if(aliasmap.find(processed_ip[0])==aliasmap.end())
    {return 0;}
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
int executeCommand(char **processed_ip,int pipecount,int append,int overwrite,unordered_map<string,string> environment_var,vector<string> history,char *custom[])
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
    if(check_custom(processed_ip,custom)==1)
    {   //cout<<"yes it is a custo command\n";
     int val=executeCustom(processed_ip,environment_var,history,custom);
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
    if(check_custom(processed_ip,custom)==1)
  {
   cout<<"Error !this command is not allowed in pipe";
   return 0;
    //return val;
  }

 
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
               
               if(k!=0)
                    dup2(file_des,STDIN_FILENO);
               if(k<(commandscount-1))
                    dup2(pipes_fd[1],STDOUT_FILENO);

                close(pipes_fd[0]);
               
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
   if(check_custom(processed_ip,custom)==1)
  {
   cout<<"Error !this command is not allowed in pipe\n";
   return 0;
    //return val;
  }
   string filename=processed_ip[1];

    
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

          char **b=ProcessString_withPipe(processed_ip[0]);
          if (child_pid == 0) {
            //destn=open(processed_ip[1],O_WRONLY | O_CREAT  , S_IRUSR|S_IWUSR);
        /*if(destn==-1)
         {
            perror("DESTN FILE ERROR");
            exit(0);
         }*/
               int f1=open(processed_ip[1],O_WRONLY | O_CREAT  , S_IRUSR|S_IWUSR);
                if(f1==-1)
                {
                  perror("DESTN FILE ERROR");
                  exit(0);
                }
                dup2(file_des,0);
                if(k<(commandscount-1))
                    dup2(pipes_fd[1],STDOUT_FILENO);
                else if(k==(commandscount-1))
                    dup2(f1,STDOUT_FILENO);

                close(pipes_fd[0]);
                close(pipes_fd[1]);
                char **a=ProcessString_withSpace(b[k]);

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
