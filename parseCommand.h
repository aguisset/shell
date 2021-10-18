#include "common.h"


#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

int is_built_in(command *command);
char* get_base_dir();
char *get_line_from_stdin();
char* trim(char* str);
commandList* read_command_with_pipes(char* line);
command* read_command_with_no_pipes(char* line);
int get_pipes_count(char* line);
int get_command_count(char* input);
commandList* read_command(char* line);
commandList* init_commandList_struct(char* line);

/** for testing **/
void testInputOutputRedirection();




// for test
void testStruct();
void testInitStructure(commandList* commandList);
void testRedirection();