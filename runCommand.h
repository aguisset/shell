#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "common.h"
#define EXIT -1 // might delete this later

int is_abs_path(char * path);
int my_system(command* command);
int exec_exit();
int exec_built_in_command(command * command);
int exec_cd(char* path);
int run_commands(commandList* commandList);
int run_command(command* command);
int run_command_with_pipes(command* command, int command_count, int command_index);
int pipe_redirect(int* fd, int redirect_type);

//testing
void test_built_in();
void test_my_system();
command* read_command_with_no_pipes(char *line); // testing this TO DELETE IF NECESSARY
commandList* read_command_with_pipes(char* line); // testing this TO DELETE IF NECESSARY
commandList* read_command(char* line);
