#include "runCommand.h"
#define BINARY_PATH "/bin/"
#define USR_BINARY_PATH "/usr/bin/"
#define EXEC_PATH "./"


int is_abs_path(char * path){
	// Returns 1 if it's an absolute path 0 if it is relative
	
	return path[0] == '/' ? 1 : 0;
}

int count_arg(command*command){
	/* After last argument there is a NULL character in argv  */
	int count = 0;
	int i = 0;
	while(command->argv[i++] != NULL)
		count++;

	return count;
}


int run_command(command* command){
	/*Executing a command without replacing the current process requires to use create a new process
	*/
	if(command->cmd == NULL){
		// blank line
		return 0;
	}
	pid_t pid;
	char binary_path[MAX_PATH_LENGTH];
	char usr_binary_path[MAX_PATH_LENGTH];
	char exec_path[MAX_PATH_LENGTH];
	int state = 0; // will tell us if exit command has been called
	int status;
	int ARGUMENT_SIZE = command->argc;//should have my command + all arguments and one spot for null char
	char *argv[ARGUMENT_SIZE+1]; // will contain all the arguments and NULL (See execv doc)
	char *path = strdup(command->cmd);
	int fd[2]; // file descriptors
	
	//memset(argv, '\0', sizeof(char*) * (ARGUMENT_SIZE+1));

	strcpy(binary_path, BINARY_PATH);
	strcpy(usr_binary_path, USR_BINARY_PATH);
	strcpy(exec_path, EXEC_PATH);

	argv[0] = strdup(command->cmd);
	printf("Printing argv from 1: \n"); // for debug
	// making a copy of element of struct command (if we don't want to make any changes to it)
	for(int i = 1; i < ARGUMENT_SIZE; i++){
		argv[i] = strdup(command->argv[i]);
		printf("argv[%d] = %s\n", i, command->argv[i]); // for debug
	}
	argv[ARGUMENT_SIZE] = NULL;
	printf("argv[ARGUMENT_SIZE+1]= %s\n", argv[ARGUMENT_SIZE]);
	//printf("Last element of ARGV is %s\n", argv[ARGUMENT_SIZE]); // for debug

	pid = fork();

	switch(pid){
		case 0:
			// child process
			/*Handling input/output redirection here*/
			
			if(command->isInput){ // see [8] Handling redirections
				fd[0] = open(command->input, O_RDONLY);
				fflush(stdin);
				dup2(fd[0], STDIN_FILENO);
				close(fd[0]);
			}
			
			if(command->isOutput){ // see [8] Handling redirections
				if((fd[1] = open(command->output , O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1){
					fprintf(stderr, "Invalid file \n");
					exit(1);
				}
				fflush(stdout);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
			}

			if(command->isAppend){
				if((fd[1] = open(command->append, O_WRONLY| O_APPEND)) == -1){
					fprintf(stderr, "Invalid file \n");
					exit(1);
				}
				fflush(stdout);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
			}

			/*
				...now the child has stdin coming from the input file, 
				    ...stdout going to the output file, and no extra files open.
				    ...it is safe to execute the command to be executed.
			*/
			if(path[0] == '.' && path[1] == '/'){
				printf("Executable detected\n"); // for debug

				
				//printf("Binary path %s\n", binary_path); // for debug
				//argv[0] = strdup(path);
				//printf("argv[0] = %s\n", argv[0]); // for debug
				//printf("pathh = %s\n", path); // for debug
				printf("%s\n", command->cmd);
				if(path[strlen(path)] == '\n') printf("something\n");
				if(execv(path, argv) == -1){
					fprintf(stderr, "Error: Invalid Program\n");
					exit(1);
				}
			}
			else if(!is_abs_path(path)){
				// Relative path this is where you can find built in command
				printf("Relative path\n"); // for debug

				//Relative path must look in /bin first then /usr/bin/

				strcat(binary_path, command->cmd);
				printf("Binary path %s\n", binary_path); // for debug
				argv[0] = strdup(binary_path);
				//char* someArgs[] = {"/bin/ls", "-l", NULL}; // for debug
				//execv("/bin/ls", someArgs); // for debug
				for(int i = 0; i <= ARGUMENT_SIZE; i++)
					printf("argv[%d]: %s\n", i, argv[i]);
				
				if(is_built_in(command)){
					printf("[rc]: Built in command\n"); // for debug
					state = exec_built_in_command(command);
				}
	 			else if(execv(binary_path, argv) == -1){
	 				// will reach here only if there is an issue
					printf("[rc]: Not a built in command\n"); // for debug
					strcat(usr_binary_path, command->cmd);
					printf("usr_binary path %s\n", usr_binary_path); // for debug
					argv[0] = strdup(usr_binary_path);
					printf("argv[0] = %s\n", argv[0]); // for debug
					if(execv(usr_binary_path, argv) == -1){

						fprintf(stderr, "Error: Invalid Program\n");
						exit(1);
					}
				}
			}
			else if(is_abs_path(path)){
				printf("Absolute path\n"); // for debug
				// absolute path
				argv[0] = strdup(path);
				printf("path is: %s\n", path);
				

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
	
	// back to parent
	wait(NULL); // wait until the child process is done

	return state;
}

int run_commands(commandList* commandList){
	/* Wrapper function for executing a command */
	/* executing multiple commands */
	int pid[commandList->command_count];
	for(int i = 0; i < commandList->command_count; i++){
		pid[i] = run_command_with_pipes(commandList->command_list[i], commandList->command_count, i);
	}
			
	// clear zombies
	for(int i = 0; i < commandList->command_count; i++){
		waitpid(pid[i], NULL, 0);
	}

	return 0;
}


int run_command_with_pipes(command* command, int command_count, int command_index){
	/*@param hasPipe is the number of commands, if it is greater than 0 then the command has pipes.
	*/

	printf("----- ENTERING RUN COMMAND WITH PIPES for %s -----\n", command->cmd); // for debug
	int nbPipe = command_count-1; // all the pipes that needs to be created + 1 for the launcher process
	int pipes[nbPipe][2]; // file descriptors where 0 is for reading and 1 is for writing.

	// opening pipes
	if(command_count > 1){
		// parent creating all pipes at the start
		for(int i = 0; i < nbPipe; i++){
			if(pipe(pipes[i]) < 0){
				fprintf(stderr, "Error: Pipe has not been created\n");
				exit(1);
			}
		}
	}
		
	pid_t pid;
	char binary_path[MAX_PATH_LENGTH];
	char usr_binary_path[MAX_PATH_LENGTH];
	char exec_path[MAX_PATH_LENGTH];
	int state;
	int ARGUMENT_SIZE = command->argc;//should have my command + all arguments and one spot for null char
	char *argv[ARGUMENT_SIZE+1]; // will contain all the arguments and NULL (See execv doc)
	char *path = strdup(command->cmd);
	int fd[2];
	// keep local mutable copy of path to use
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
		case 0:
			// child process
			if(command_count > 1){
				// multiple commands that requires pipes
				printf("There are multiple commands %s\n", command->cmd); // for debug
				
				// closing unused pipes for current child process
				
				// redirecting stdin and stdout [10]

				
				if(command_index == 0){
					// first command (stdin -> command1 -> pipe0)
					printf("Command %s with command_index= %d\n", command->cmd, command_index);
					if(dup2(pipes[command_index][1], STDOUT_FILENO) < 0){
						fprintf(stderr, "Error: Redirecting standard output failed\n");
						exit(1);
					}
					close(pipes[command_index][0]);
					close(pipes[command_index][1]);
				}
				else if(command_index == command_count-1){
					// last command (previousPipe -> lastCommand -> stdout)
					printf("Command %s with command_index= %d\n", command->cmd, command_index);
					if(dup2(pipes[command_index-1][0], STDIN_FILENO) < 0){
						fprintf(stderr, "Error: Redirecting standard input failed 1\n");
						exit(1);
					}

					close(pipes[command_index-1][0]);
					close(pipes[command_index-1][1]);
				}
				else{
					// not first nor last command (previousPipe -> currentCommand -> nextPipe)
					if(dup2(pipes[command_index-1][0], STDIN_FILENO) < 0){
						fprintf(stderr, "Error: Redirecting standard input failed 2\n");
						exit(1);
					}

					if(dup2(pipes[command_index+1][1], STDOUT_FILENO) < 0){
						fprintf(stderr, "Error: Redirecting standard output failed\n");
						exit(1);
					}

					close(pipes[command_index-1][0]);
					close(pipes[command_index-1][1]);
					close(pipes[command_index+1][0]);
					close(pipes[command_index+1][0]);
				}

			}
			/*Handling input/output redirection here*/
			if(command->isInput){ // see [8] Handling redirections
				fd[0] = open(command->input, O_RDONLY);
				fflush(stdin);
				if(dup2(fd[0], STDIN_FILENO) < 0){
					fprintf(stderr, "Error: Redirecting standard output failed");
					exit(1);
				}
				close(fd[0]);
			}
			
			if(command->isOutput){ // see [8] Handling redirections
				if((fd[1] = open(command->output , O_CREAT | O_TRUNC | O_WRONLY, 0644)) == -1){
					fprintf(stderr, "Invalid file \n");
					exit(1);
				}
				fflush(stdout);
				if(dup2(fd[1], STDOUT_FILENO) < 0){
					fprintf(stderr, "Error: Redirecting standard output failed");
					exit(1);
				}
				close(fd[1]);
			}

			if(command->isAppend){
				if((fd[1] = open(command->append, O_WRONLY| O_APPEND)) == -1){
					fprintf(stderr, "Invalid file \n");
					exit(1);
				}
				fflush(stdout);
				if(dup2(fd[1], STDOUT_FILENO) < 0){
					fprintf(stderr, "Error: Redirecting standard output failed");
					exit(1);
				}
				close(fd[1]);
			}

			/*
				...now the child has stdin coming from the input file, 
				    ...stdout going to the output file, and no extra files open.
				    ...it is safe to execute the command to be executed.
			*/
			if(path[0] == '.' && path[1] == '/'){
				printf("Executable detected\n"); // for debug

				if(execv(path, argv) == -1){
					fprintf(stderr, "Error: Invalid Program\n");
					exit(1);
				}
			}
			else if(!is_abs_path(path)){
				// Relative path this is where you can find built in command
				printf("Relative path %s\n", command->cmd); // for debug


				strcat(binary_path, command->cmd);
				argv[0] = strdup(binary_path);
				//char* someArgs[] = {"/bin/ls", "-l", NULL}; // for debug
				//execv("/bin/ls", someArgs); // for debug
				for(int i = 0; i <= ARGUMENT_SIZE; i++)
					printf("argv[%d]: %s\n", i, argv[i]);
				
				if(is_built_in(command)){
					printf("[rc]: Built in command\n"); // for debug
					state = exec_built_in_command(command);
				}
	 			else if(execv(binary_path, argv) == -1){
	 				// will reach here only if there is an issue
					printf("[rc]: Not a built in command\n"); // for debug
					strcat(usr_binary_path, command->cmd);
					printf("usr_binary path %s\n", usr_binary_path); // for debug
					argv[0] = strdup(usr_binary_path);

					// for debug
					for(int i = 0; i <= ARGUMENT_SIZE; i++)
						printf("argv[%d]: %s\n", i, argv[i]);
					// end for debug
					printf("argv[0] = %s\n", argv[0]); // for debug
					if(execv(usr_binary_path, argv) == -1){

						fprintf(stderr, "Error: Invalid Program\n");
						exit(1);
					}
				}
			}
			else if(is_abs_path(path)){
				printf("Absolute path\n"); // for debug
				// absolute path
				argv[0] = strdup(path);
				printf("path is: %s\n", path);
				

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
	
	// waiting
	//waitpid(pid, NULL, 0);

	// closing pipes
	if(command_count > 1){
		// parent created all pipes at the start, they have to be closed
		for(int i = 0; i < nbPipe; i++){
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
	}

	return pid;
}


int exec_built_in_command(command * command){
	printf("[exBuiltIn]: Executing built in command\n"); // for debug
	int state = 0;
	if(!strcmp(command->cmd, "cd")){ 
		printf("cd command is being executed\n"); // for debug
		if(command->argc == 2)
			exec_cd(command->argv[1]);
		else{
			printf("cd doesn't have the required argument\n"); // for debug
			fprintf(stderr, "Error: invalid command\n");
			//exit(1);
		}
		
	}
	else if(!strcmp(command->cmd, "jobs")){
		//printf("jobs command is being executed\n");
		//exec_jobs();
	}
	else if(!strcmp(command->cmd, "exit")){
		if(command->argc > 1)
			fprintf(stderr, "Error: invalid command\n");
		else
			state = exec_exit();
	}
	else{
		// for any other program we will execute it using exec
	}

	return state;
}

int exec_exit(){
	/* This command terminates your shell. 
	However, if there are currently suspended jobs, your shell should not terminate.

	NOTE: FOR NOW THIS KILLS ALL THE PROCESSES and doesn't take into account suspended jobs 
	(those that are in the background), we need to count them
	*/
	printf("Terminating the shell\n");
	exit(0);
	
	return -1;
}

int exec_cd(char* path){
	int chdir_value = chdir(path); // returns 0 if success
	
	if(chdir_value){
		fprintf(stderr, "Error: invalid directory\n");
		//exit(1); // no need we still need process
	}

	return 0;
}

/******* Testing ******/


void test_my_system(){
	
	command*command;
	commandList* commandList;
	char* input = "ls -l"; // PASS
	//char* input = "cat > output.txt\n"; // PASS
	//char* input = "./nyuc\n"; // PASS (needed to change the delimiter in strtok from " " to " \n")
	//char* input = "./nyuc"; // PASS
	//char* input = "./nyuc Abdoul"; // PASS
	//char* input = "/Users/guisset/Documents/NYU/Classes/OS/labs/nyush/nyuc\n"; // PASS
	//char* input = "/Users/guisset/Documents/NYU/Classes/OS/labs/nyush/nyuc"; // PASS
	//char* inputWithPipes = "cat | cat"; // Each is executed without redirecting FAILED
	//char* test = "ls -l";
	char* inputWithPipes = "ls -l | grep Makefile";
	//command = read_command_with_no_pipes(input);
	commandList = read_command(inputWithPipes);


	// for debug 
	for(int i = 0; i < commandList->command_count; i++){
		for(int j = 0; j < commandList->command_list[i]->argc; j++){

			printf("commandList->command_list[%d]->argv[%d] = %s\n", i, j, commandList->command_list[i]->argv[j]);
		}
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