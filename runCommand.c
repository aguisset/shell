#include "runCommand.h"
#define BINARY_PATH "/bin/"
#define USR_BINARY_PATH "usr/bin/"
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

int run_commands(commandList* commandList){
	int status = 0;
	if(commandList->command_count > 1){
		// more than one command to execute
		
		for(int i = 0; i < commandList->command_count; i++){
			status = run_command(commandList->command_list[i]);
		}
	}
	else{
		// only one command to execute
		status = run_command(commandList->command_list[0]);
	}

	return status;
}
int run_command(command* command){
	/*Executing a command without replacing the current process requires to use create a new process
	*/
	if(command->cmd == NULL){
		// blank line
		return 0;
	}

	int state = 0; // will tell us if exit command has been called
	pid_t pid;
	char binary_path[MAX_PATH_LENGTH];
	char usr_binary_path[MAX_PATH_LENGTH];
	char exec_path[MAX_PATH_LENGTH];

	int ARGUMENT_SIZE = command->argc;//should have my command + all arguments and one spot for null char
	char *path = strdup(command->cmd);
	
	char *argv[ARGUMENT_SIZE+1]; // will contain all the arguments and NULL (See execv doc)
	//memset(argv, '\0', sizeof(char*) * (ARGUMENT_SIZE+1));

	strcpy(binary_path, BINARY_PATH);
	strcpy(usr_binary_path, USR_BINARY_PATH);
	strcpy(exec_path, EXEC_PATH);

	// making a copy of element of struct command (if we don't want to make any changes to it)
	for(int i = 1; i < ARGUMENT_SIZE; i++){
		argv[i] = strdup(command->argv[i]);
		//printf("argv[%d] = %s\n", i, command->argv[i-1]); // for debug
	}
	argv[ARGUMENT_SIZE] = NULL;
	
	//printf("Last element of ARGV is %s\n", argv[ARGUMENT_SIZE]); // for debug

	pid = fork();

	if(pid == 0){
		// child process
		//printf("Child process\n"); // for debug
		if(path[0] == '.' && path[1] == '/'){
			printf("Executable detected\n"); // for debug

			
			//printf("Binary path %s\n", binary_path); // for debug
			argv[0] = strdup(path);
			//printf("argv[0] = %s\n", argv[0]); // for debug

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
			//printf("Binary path %s\n", binary_path); // for debug
			argv[0] = strdup(binary_path);
			//printf("argv[0] = %s\n", argv[0]); // for debug

			if(is_built_in(command)){
				printf("[rc]: Built in command\n"); // for debug
				state = exec_built_in_command(command);
			}
 			else if(execv(binary_path, argv) == -1){
 				// will reach here only if there is an issue
				printf("[rc]: Not a built in command\n"); // for debug
				strcat(usr_binary_path, command->cmd);
				//printf("usr_binary path %s\n", usr_binary_path); // for debug
				argv[0] = strdup(usr_binary_path);
				//printf("argv[0] = %s\n", argv[0]); // for debug
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

	wait(NULL); // wait until the child process is done

	return state;
}

int exec_built_in_command(command * command){
	printf("[exBuiltIn]: Executing built in command\n"); // for debug
	int state = 0;
	if(!strcmp(command->cmd, "cd")){ 
		printf("cd command is being executed\n"); // for debug
		if(command->argc == 2)
			exec_cd(command->argv[0]);
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
	//exit(0);
	
	return EXIT;
}

int exec_cd(char* path){
	int chdir_value = chdir(path); // returns 0 if success
	
	if(chdir_value){
		fprintf(stderr, "Error: invalid directory\n");
		//exit(1); // no need we still need process
	}

	return 0;
}

void test_my_system(){
	
	command*command;

	command = malloc(sizeof(command));

	/* // Testing relative path
	command->argc = 2;
	command->cmd = "ls";
	command->argv[0] = "-a";
	command->argv[1] = NULL;
	*/

	/*//Testing absolute path
	command->argc = 2;
	command->cmd = "/Users/guisset/Documents/NYU/Classes/OS/labs/nyuc/nyuc";
	command->argv[0] = "something";
	command->argv[1] = NULL;
	*/

	/*
	// Testing current working directory (should PASS)
	command->argc = 1;
	command->cmd = "./nyuc";
	command->argv[0] = NULL;
	//command->argv[1] = NULL;
	*/

	// Testing current working directory (should not PASS)
	command->argc = 1;
	command->cmd = "nyuc";
	command->argv[0] = NULL;
	//command->argv[1] = NULL;
	
	run_command(command);
}

void test_built_in(){
	command*command; // for cd
	struct command* command_jobs;

	//commandList *commandList;

	/* Init cd */
	command = malloc(sizeof(command));
	command->argc = 2; 
	//char * str1 = "a";
	//char * str2 = "b";
	command->argv[1] = "/str2";
	command->argv[0] = "test";
	command->cmd = "cd";

	char currdir[255];
	char newdir[255];

	/*Init jobs */
	command_jobs = malloc(sizeof(command));
	command_jobs->argc = 1; 
	command_jobs->cmd = "exit"; 
	if(is_built_in(command_jobs)){
		printf("Executing %s command\n", command_jobs->cmd);
		exec_built_in_command(command_jobs);
		printf("Nothing should have happened here....\n");
	}
	if(is_built_in(command)){
		printf("Executing %s command with arguments %s\n", command->cmd, command->argv[0]);
		printf("Current directory: %s\n", getcwd(currdir, 255));
		exec_built_in_command(command);
		printf("New directory: %s\n", getcwd(newdir, 255));
	}
	else printf("Not a built in command\n");
}