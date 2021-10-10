#include <unistd.h>
#include <signal.h>
#include "common.h"
#define EXIT -1 // might delete this later

int is_abs_path(char * path);
int my_system(command* command);
int exec_exit();
int exec_built_in_command(command * command);
int exec_cd(char* path);
int run_commands(commandList* commandList);
int run_command(command* command);


//testing
void test_built_in();
void test_my_system();