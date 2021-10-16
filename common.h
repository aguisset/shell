#ifndef __GUARD_STRUCTS__
#define __GUARD_STRUCTS__


#define BUFFER_SIZE 1000
#define STDIN stdin
#define ARGUMENTS 1000
#define MAX_PATH_LENGTH 1000
// Structure that contains the parsed command
typedef struct command{ 
	int argc; // number of argument including that command
	int isOutput; // boolean set to 1 (true) if there is an output redirection
	int isInput; // boolean set to 1 (true) if there is an input redirection
	int isAppend; // boolean set to 1 (true) if there is >> for output redirection by appending to an existing file
	char* input;
	char* output;
	char* append;
	char* argv[ARGUMENTS]; // contains command + arguments terminated by NULL for convenience
	char* cmd; // actual command like ls
}command;

typedef struct commandList{
	// Wrapper structure if there is more than one command
	int command_count;
	command* command_list[];
}commandList;


#endif

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

int is_built_in(command *command);