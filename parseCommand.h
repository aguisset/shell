#include "common.h"


#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>



char* get_base_dir();


char *get_line_from_stdin();
char** read_command(char *line);

void free_arrays_of_pointers(char **arr, size_t size);
int command_count(char* input);

// for test
void testStruct();