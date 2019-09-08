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
using namespace std;
/*extern vector<string> history;
extern unordered_map<string,string> environment_var;
extern char* custom[]={"cd","exit","help","alias","history","echo"};
extern int pipecount=0;
extern int overwrite=0;
extern int append=0;
extern int appendflag=0;
extern pid_t mypid;
extern char PS1='$';
extern unordered_map<string,string> aliasmap;*/
void init_bash();
void enable_noncanonical();
void handleSignal(int);
void printPrompt();
char* getInput();