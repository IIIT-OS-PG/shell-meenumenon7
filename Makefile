#shellcompile:g++ -o merabash myshell.cpp process_ip.cpp execute_ip.cpp
#SOURCES = myshell.cpp process_ip.cpp execute_ip.cpp
#g++ -g -Wall -o merabash $(SOURCES)CC = gcc
CC = g++
CFLAGS = -Wall -g

myshellc: myshell.cpp process_ip.cpp execute_ip.cpp
	${CC} ${CFLAGS} -o myshellc myshell.cpp process_ip.cpp execute_ip.cpp

