#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "common.h"
#define EXIT -1 // might delete this later

int exec_exit();
int exec_cd(char* path);
int exec_built_in_command(command * command);
int is_abs_path(char * path);
int run_command(command* command, int command_count, int command_index, int p[2], int fd_in);
int run_commands(commandList* commandList);

//testing
void test_built_in();
void test_my_system();
command* read_command_with_no_pipes(char *line); // testing this TO DELETE IF NECESSARY
commandList* read_command_with_pipes(char* line); // testing this TO DELETE IF NECESSARY
commandList* read_command(char* line);
