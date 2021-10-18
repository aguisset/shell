#include "runCommand.h"
#define BINARY_PATH "/bin/"
#define USR_BINARY_PATH "/usr/bin/"
#define EXEC_PATH "./"

int job_count = 0; // global variable that will be incremented everytime a job is stopped
struct jobs* jobs;
char* current_command = NULL;

int is_abs_path(char * path){
	// Returns 1 if it's an absolute path 0 if it is relative
	
	return path[0] == '/' ? 1 : 0;
}

static void child_sig_handler(int signo){
	switch(signo){
		case SIGTSTP:
			printf("Handling SIGTSTP: command = [%s] and job_count = %d\n", current_command, job_count);// for debug
			jobs->job_list[job_count]->id = job_count+1;
			if(current_command != NULL){ 
				printf("Current_command has been assigned properly\n");
				jobs->job_list[job_count]->command = strdup(current_command);
			}
			job_count++;
			printf("Handled SIGTSTP and now job_count = %d\n", job_count);// for debug

			break;

	}

	return;
}
int run_commands(commandList* commandList){
	/* Wrapper function for executing a command */
	/* executing multiple commands */
	pid_t pid[commandList->command_count];
	int p[2]; // file descriptors for the 
	int fd_in = 0;
	int status;
	// checking global variable allocation
	jobs = calloc(sizeof(struct jobs) + MAX_JOBS*sizeof(struct job*), sizeof(struct jobs));
	if(jobs== NULL){
		fprintf(stderr, "Error: Allocating memory\n");
		exit(1);
	}


	for(int i = 0; i < commandList->command_count; i++){
		if(pipe(p) < 0){
			fprintf(stderr, "Error creating pipe\n");
			exit(1);
		}
		pid[i] = run_command_with_pipes(commandList->command_list[i], jobs, commandList->command_count, i, p, fd_in);

		// back in parent
		
		close(p[1]);
		fd_in = p[0]; // save for the next command if any

	}
	
	/*
	// clear zombies
	for(int i = 0; i < commandList->command_count; i++){
		waitpid(pid[i], &status, WNOHANG);
	}
	*/
	// closing pipes
	close(p[0]);
	close(p[1]);
	return 0;
}


int run_command_with_pipes(command* command, struct jobs* jobs, int command_count, int command_index, int p[2], int fd_in){

	printf("----- ENTERING RUN COMMAND WITH PIPES for %s -----\n", command->cmd); // for debug
		
	pid_t pid;
	int fd[2];
	current_command = command->input_command;// update global variable command

	pid = fork();
	switch(pid){
		case 0: // child process
			// handling signals before execution
			signal (SIGTSTP, SIG_DFL); // revert to previous prehavior
			//signal(SIGTSTP, &child_sig_handler);

			if(command_count > 1){
				// multiple commands that requires pipes see [11]
				printf("There are multiple commands %s\n", command->cmd); // for debug
				
				// redirecting stdin and stdout [10]

				// change the input according to the old one
				if(dup2(fd_in, STDIN_FILENO) < 0){
					fprintf(stderr, "Error: Creation of pipe failed\n");
					exit(1);
				} 
		        
				if(command_index != command_count -1){
					// not last command
					printf("Command %s with command_index= %d\n", command->cmd, command_index); // for debug
					
					if(dup2(p[1], STDOUT_FILENO) < 0){
						fprintf(stderr, "Error: Creation of pipe failed 2\n");
					}
					
				}
				// closing unused pipe
				close(p[0]);
			}

			/*Handling input/output redirection here*/
			if(command->isInput){ // see [8] Handling redirections
				printf("There are some input redirections\n"); // for debug
				fd[0] = open(command->input, O_RDONLY);
				fflush(stdin);
				if(dup2(fd[0], STDIN_FILENO) < 0){
					fprintf(stderr, "Error: Redirecting standard input failed\n");
					exit(1);
				}
				close(fd[0]);
			}
			
			if(command->isOutput){ // see [8] Handling redirections
				printf("There are some input redirections to %s\n", command->output); // for debug
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

			/*
				...now the child has stdin coming from the input file, 
				...stdout going to the output file, and no extra files open.
				...it is safe to execute the command to be executed.
			*/
			
			exec_command(command);

			break;
		case -1:
			fprintf(stderr, "Error while forking\n");
			break;
	}
	
	//waitpid(pid, NULL, WNOHANG);
	int status;
	waitpid(pid, &status, WNOHANG);
	if (WIFEXITED(status)) {
	    // child process terminated normally
	    printf("Child process terminated normally\n");
	} else if (WIFSIGNALED(status)) {
	    //child process terminated by a signal
	    printf("Child process terminated by a signal\n");

	} else if (WIFSTOPPED(status)) {
		// child process was stopped not terminated
		printf("Child process was stopped not terminated\n");
		printf("[nyush %s]$ _", get_base_dir());
		fflush(stdout);
	}

	return pid;
}

void exec_command(command* command){
	char binary_path[MAX_PATH_LENGTH];
	char usr_binary_path[MAX_PATH_LENGTH];
	char exec_path[MAX_PATH_LENGTH];

	int ARGUMENT_SIZE = command->argc;//should have my command + all arguments and one spot for null char
	char *argv[ARGUMENT_SIZE+1]; // will contain all the arguments and NULL (See execv doc)
	char *path = strdup(command->cmd);
	int state;
	argv[0] = strdup(command->cmd);
	
	// making a copy of element of struct command (if we don't want to make any changes to it)
	for(int i = 1; i < ARGUMENT_SIZE; i++){
		argv[i] = strdup(command->argv[i]);
	}
	argv[ARGUMENT_SIZE] = NULL;
	
	// keep local mutable copy of path to use
	strcpy(binary_path, BINARY_PATH);
	strcpy(usr_binary_path, USR_BINARY_PATH);
	strcpy(exec_path, EXEC_PATH);


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
	char* input = "cat"; // to test jobs
	//char* input = "ls -l"; // PASS
	//char* input = "ps | grep nyush | wc"; // PASS
	//char* input = "cat > output.txt\n"; // PASS
	//char* input = "cat < output.txt\n"; // PASS
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