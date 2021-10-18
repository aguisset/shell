#ifndef __GUARD_STRUCTS__
#define __GUARD_STRUCTS__


#define BUFFER_SIZE 1000
#define STDIN stdin
#define ARGUMENTS 1000
#define MAX_PATH_LENGTH 1000
#define MAX_JOBS 100

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
	char* input_command; // command retrieved from stdin
}command;

typedef struct commandList{
	// Wrapper structure if there is more than one command
	int command_count;
	command* command_list[];
}commandList;


struct job{
	int id; // unique id that will be used to display jobs in order
	char* command; // command used
};

struct jobs{
	struct job* job_list[MAX_JOBS]; // no more than a 100 jobs
};


#endif

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<ctype.h>

int is_built_in(command *command);
char* get_base_dir();