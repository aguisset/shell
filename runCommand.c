#include "runCommand.h"
#define BINARY_PATH "/bin/"
#define USR_BINARY_PATH "/usr/bin/"
#define EXEC_PATH "./"


int exec_exit(){
	/* This command terminates your shell. 
	However, if there are currently suspended jobs, your shell should not terminate.

	NOTE: FOR NOW THIS KILLS ALL THE PROCESSES and doesn't take into account suspended jobs 
	(those that are in the background), we need to count them
	*/

	printf("Terminating the shell\n");
	printf("Kill all processes including suspended jobs, SEE README\n");
	kill(shell_pid, SIGKILL);
	
	exit(0);
}

int exec_cd(char* path){
	int chdir_value = chdir(path); // returns 0 if success
	
	if(chdir_value){
		fprintf(stderr, "Error: invalid directory\n");
		//exit(1); // no need we still need process
	}

	return 0;
}

int exec_built_in_command(command * command){
	
	int state = 0;
	if(!strcmp(command->cmd, "cd")){ 
		if(command->argc == 2)
			exec_cd(command->argv[1]);
		else{
			fprintf(stderr, "Error: invalid command\n");
			exit(1);
		}
		
	}
	else if(!strcmp(command->cmd, "jobs")){
		printf("[jobs] command has not been implemented yet. See README\n");
		//exec_jobs();
	}
	else if(!strcmp(command->cmd, "exit")){
		if(command->argc > 1)
			fprintf(stderr, "Error: invalid command\n");
		else
			state = exec_exit();
	}

	return state;
}

int is_abs_path(char * path){
	// Returns 1 if it's an absolute path 0 if it is relative
	
	return path[0] == '/' ? 1 : 0;
}

int run_command(command* command, int command_count, int command_index, int p[2], int fd_in){		
	pid_t pid;
	int ARGUMENT_SIZE = command->argc;//should have my command + all arguments and one spot for null char
	char *argv[ARGUMENT_SIZE+1]; // will contain all the arguments and NULL (See execv doc)
	char *path = strdup(command->cmd);
	int fd[2];
	// keep local mutable copy of path to use
	char binary_path[MAX_PATH_LENGTH];
	char usr_binary_path[MAX_PATH_LENGTH];
	char exec_path[MAX_PATH_LENGTH];
	strcpy(binary_path, BINARY_PATH);
	strcpy(usr_binary_path, USR_BINARY_PATH);
	strcpy(exec_path, EXEC_PATH);

	argv[0] = strdup(command->cmd);
	
	// making a copy of element of struct command (if we don't want to make any changes to it)
	for(int i = 1; i < ARGUMENT_SIZE; i++){
		argv[i] = strdup(command->argv[i]);
	}
	argv[ARGUMENT_SIZE] = NULL;
	
	pid = fork();
	switch(pid){
		case 0: // child process
			
			if(command_count > 1){
				// multiple commands that requires pipes see [10] & [11]
				
				// change the input according to the old one
				if(dup2(fd_in, STDIN_FILENO) < 0){
					fprintf(stderr, "Error: Creation of pipe failed\n");
					exit(1);
				} 
		        
				if(command_index != command_count -1){
					// not last command
					if(dup2(p[1], STDOUT_FILENO) < 0){
						fprintf(stderr, "Error: Creation of pipe failed 2\n");
					}
					
				}
				// closing unused pipe
				close(p[0]);
			}

			/*Handling input/output redirection here*/
			if(command->isInput){ // see [8] Handling redirections
				
				fd[0] = open(command->input, O_RDONLY);
				fflush(stdin);
				if(dup2(fd[0], STDIN_FILENO) < 0){
					fprintf(stderr, "Error: Redirecting standard input failed\n");
					exit(1);
				}
				close(fd[0]);
			}
			if(command->isOutput){ // see [8] Handling redirections
				if((fd[1] = open(command->output , O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1){
					fprintf(stderr, "Invalid file\n");
					exit(1);
				}
				fflush(stdout);
				if(dup2(fd[1], STDOUT_FILENO) < 0){
					fprintf(stderr, "Error: Redirecting standard output failed\n");
					exit(1);
				}
				close(fd[1]);
			}

			if(command->isAppend){
				if((fd[1] = open(command->append, O_WRONLY| O_APPEND)) == -1){
					fprintf(stderr, "Invalid file\n");
					exit(1);
				}
				fflush(stdout);
				if(dup2(fd[1], STDOUT_FILENO) < 0){
					fprintf(stderr, "Error: Redirecting standard output failed\n");
					exit(1);
				}
				close(fd[1]);
			}

			if(path[0] == '.' && path[1] == '/'){
				if(execv(path, argv) == -1){
					fprintf(stderr, "Error: Invalid Program\n");
					exit(1);
				}
			}
			else if(!is_abs_path(path)){
				// Relative path this is where you can find built in command
				strcat(binary_path, command->cmd);
				argv[0] = strdup(binary_path);
				
				/* execv usage example:
					char* someArgs[] = {"/bin/ls", "-l", NULL};
					execv("/bin/ls", someArgs); // for debug
				*/


				if(is_built_in(command)){
					exec_built_in_command(command);
				}
	 			else if(execv(binary_path, argv) == -1){
	 				// will reach here if not found in /bin/ls
					strcat(usr_binary_path, command->cmd);
					argv[0] = strdup(usr_binary_path);

					if(execv(usr_binary_path, argv) == -1){
						fprintf(stderr, "Error: Invalid Program\n");
						exit(1);
					}
				}
			}
			else if(is_abs_path(path)){
				// absolute path
				argv[0] = strdup(path);
				if(execv(path, argv) == -1){
					fprintf(stderr, "Error: Invalid Program\n");
					exit(1);
				}
			}
			else{
				fprintf(stderr, "Error: Invalid Program\n");
				exit(1);
			}

			break;
		case -1:
			fprintf(stderr, "Error while forking\n");
			break;
	}
	
	return pid;
}

int run_commands(commandList* commandList){
	/* Wrapper function for executing a command */
	/* executing multiple commands */
	int pid[commandList->command_count];
	int p[2]; // file descriptors for the 
	int fd_in = 0;

	for(int i = 0; i < commandList->command_count; i++){
		if(pipe(p) < 0){
			fprintf(stderr, "Error creating pipe\n");
			exit(1);
		}
		pid[i] = run_command(commandList->command_list[i], commandList->command_count, i, p, fd_in);

		// back in parent
		waitpid(pid[i], NULL, 0);
		close(p[1]);
		fd_in = p[0]; // save for the next command if any
	}
	
	// closing pipes
	close(p[0]);
	close(p[1]);
	return 0;
}

/******* Testing ******/


void test_my_system(){
	
	command*command;
	commandList* commandList;
	char* input = "ls -l\n"; // PASS (needed to change the delimiter in strtok from " " to " \n")
	//char* input = "ps | grep nyush | wc"; // PASS
	//char* input = "cat > output.txt\n"; // PASS
	//char* input = "cat < output.txt\n"; // PASS
	//char* input = "cat >> output.txt\n"; // FAILED
	//char* input = "./nyuc\n"; // PASS (needed to change the delimiter in strtok from " " to " \n")
	//char* input = "./nyuc"; // PASS
	//char* input = "./nyuc Abdoul"; // PASS
	//char* input = "/Users/guisset/Documents/NYU/Classes/OS/labs/nyush/nyuc\n"; // PASS
	//char* input = "/Users/guisset/Documents/NYU/Classes/OS/labs/nyush/nyuc"; // PASS
	//char* input = "cat | cat"; // Each is executed without redirecting FAILED
	//char* test = "ls -l";
	//char* input = "ls -l | grep Makefile"; // PASS
	//char* input = "du | sort | head"; // PASS (but only the first 2 lines)
	//char* input = "ls -l| grep Makefile | wc"; // PASS
	//command = read_command_with_no_pipes(input);
	commandList = read_command(input);


	// for debug 
	for(int i = 0; i < commandList->command_count; i++){
		for(int j = 0; j < commandList->command_list[i]->argc; j++){

			printf("commandList->command_list[%d]->argv[%d] = %s\n", i, j, commandList->command_list[i]->argv[j]);

		}
		if(commandList->command_list[i]->isOutput) printf("Output: %s\n", commandList->command_list[i]->output);
		if(commandList->command_list[i]->isInput) printf("Output: %s\n", commandList->command_list[i]->input);
	}

	//end for debug */ 

	//run_command(commandList->command_list[0]);
	run_commands(commandList);
}

void test_built_in(){
	char* input = "cd test"; // PASS
	//char* input = "exit"; // PASS
	command*command; // for cd
	command = read_command_with_no_pipes(input);
	
	char currdir[255];
	char newdir[255];

	
	if(is_built_in(command)){
		printf("Executing %s command with arguments %s\n", command->cmd, command->argv[1]);
		printf("Current directory: %s\n", getcwd(currdir, 255));
		exec_built_in_command(command);
		printf("New directory: %s\n", getcwd(newdir, 255));
	}
	else printf("Not a built in command\n");
}
