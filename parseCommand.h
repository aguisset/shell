#include "common.h"


#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>


commandList* init_commandList_struct(char* line);
//int hasPipe(char* line);
int get_pipes_count(char* line);
char* get_base_dir();


char *get_line_from_stdin();
char** read_command(char *line);
char** read_command_with_no_pipes(char *line);
char** read_command_with_pipes(char *line);

void free_arrays_of_pointers(char **arr, size_t size);
int get_command_count(char* input);

// for test
void testStruct();
//void testHasPipe();