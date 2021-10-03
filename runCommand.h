#include <unistd.h>
#include <signal.h>
#include "parseCommand.h"

void exec_exit();
void exec_built_in_command(command * command);
int exec_cd(char* path);



//testing
void test_built_in();