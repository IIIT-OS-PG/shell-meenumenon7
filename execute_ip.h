#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

int executeAlias(char **);
int check_custom(char **,char* []);
void executeEcho(char **,unordered_map<string,string>);
int executeCustom(char **,unordered_map<string,string>,vector<string>,char *[]);
int check_for_alias(char **);
int fetch_and_run_alias(char **);
int executeCommand(char **,int,int,int,unordered_map <string,string> ,vector<string>,char *[]);
void executeow(char **);
void print_history(vector<string> v);


