#include "common.h"
#include "parseCommand.h"
#include "runCommand.h"


static void signal_handler(int signo){
	
	switch(signo){
		case SIGINT:
			//CLTR-C
			break;
		case SIGQUIT:
			
			break;
		case SIGTERM:
			
			break;
		case SIGTSTP:
			// CLTR-Z
			break;
		default:
			//other jobs not handled
			break;
	}
}
int init_shell(){
	int status = 0;

	// Signal handling
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGTSTP, signal_handler);
	signal(SIGQUIT, signal_handler);

	commandList* commandList;
	while(!status){
		char* base_dir;
		char* line;
		base_dir = get_base_dir();
		printf("[nyush %s]$ _", base_dir);
		line = get_line_from_stdin();

		//fflush() // remember to flush stdout
		commandList = init_commandList_struct(line);
		status = run_commands(commandList); // this has a segfault if ^C or blank line (FIXED in run_command)
	}
	
	// freeing
	for(int i = 0; i < commandList->command_count; i++){
		free(commandList->command_list[i]);
	}
	free(commandList);
	return status;
}
int main(int argc, char ** argv){
	shell_pid = getpid();
	return init_shell();
}

/*
Resources:
[1] Different types of whitespace in C: https://user-web.icecube.wisc.edu/~dglo/c_class/charfunc.html#:~:text=In%20ASCII%2C%20whitespace%20characters%20are,formfeed%20(%20'%5Cf'%20).
[2] "Advanced Programming in the Unix environment", W.Richard Stevens and Steven A.Rago (Chapter 8, 10)
[3] Flexible array members in a structure: https://www.geeksforgeeks.org/flexible-array-members-structure-c/
[4] How to split strings in C: https://www.youtube.com/watch?v=34DnZ2ewyZo&ab_channel=CodeVault
[5] Valgrind errors: https://stackoverflow.com/questions/9300227/tracking-down-valgrind-40-bytes-in-1-blocks-are-definitely-lost-in-loss-record
[6] How to use strstr: https://www.tutorialspoint.com/c_standard_library/c_function_strstr.htm
[7] Exec system call in Linux: https://linuxhint.com/exec_linux_system_call_c/
[8] Handling redirections: https://stackoverflow.com/questions/11515399/implementing-shell-in-c-and-need-help-handling-input-output-redirection
[9] How to trim leading and trailing whitespaces in C: https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
[10] Implementation of multiple pipes in C: https://stackoverflow.com/questions/8389033/implementation-of-multiple-pipes-in-c
[11] Coding multiple pipes in C: https://stackoverflow.com/questions/17630247/coding-multiple-pipe-in-c
[12] How to correctly stop child process: https://stackoverflow.com/questions/66377870/how-to-correctly-send-sigtstp-signal-to-a-child-process
*/