#include "runCommand.h"


void exec_built_in_command(command * command){
	if(!strcmp(command->cmd, "cd")){ 
		//printf("cd command is being executed\n");
		if(command->argc == 2)
			exec_cd(command->argv[0]);
		else{
			fprintf(stderr, "Error: invalid command\n");
			exit(1);
		}
		
	}
	else if(!strcmp(command->cmd, "jobs")){
		//printf("jobs command is being executed\n");
		//exec_jobs();
	}
	else if(!strcmp(command->cmd, "exit")){
		exec_exit();
	}

	return;
}
int exec_cd(char* path){
	int chdir_value = chdir(path); // returns 0 if success
	
	if(chdir_value){
		fprintf(stderr, "Error: invalid directory\n");
		exit(1);
	}

	return 0;
}

void test_built_in(){
	command*command; // for cd
	command*command_jobs;

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
	command_jobs->argc = 2; 
	if(is_built_in(command_jobs)){

	}
	if(is_built_in(command)){
		printf("Executing %s command with arguments %s\n", command->cmd, command->argv[0]);
		printf("Current directory: %s\n", getcwd(currdir, 255));
		exec_built_in_command(command);
		printf("New directory: %s\n", getcwd(newdir, 255));
	}
	else printf("Not a built in command\n");
}