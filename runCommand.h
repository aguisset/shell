#include <unistd.h>
#include <signal.h>
#include "common.h"

int is_abs_path(char * path);
int my_system(command* command);
void exec_exit();
void exec_built_in_command(command * command);
int exec_cd(char* path);



//testing
void test_built_in();
void test_my_system();