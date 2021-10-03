#ifndef __GUARD_STRUCTS__
#define __GUARD_STRUCTS__

#define BUFFER_SIZE 1000
#define STDIN stdin
#define ARGUMENTS 1000
#define MAX_PATH_LENGTH 1000
// Structure that contains the parsed command
typedef struct command{ 
	int argc; // number of argument including that command
	char* argv[ARGUMENTS]; // arguments like -a
	char *cmd; // actual command like ls
}command;

typedef struct commandList{
	// Wrapper structure if there is more than one command
	int command_count;
	command* cmds[];
}commandList;

#endif

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>



char* get_base_dir();

int is_built_in(command *command);
char *get_line_from_stdin();
char** read_command(char *line);

void free_arrays_of_pointers(char **arr, size_t size);
int command_count(char* input);

// for test
void testStruct();