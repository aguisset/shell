#include "parseCommand.h"
/*

A command: ls
Option: -a -b

Note: if we do ls-a in command line this would result in command not found.
So the first space determines what is a command from what is not.


Current problem: Strncpy doesn't seem to copy anything
*/


int is_built_in(command *command){
	// we are implementing cd, jobs, fg and exit commands
	// if the command enters is one of those 4 is_built_in returns true (1) else false
	// strcmp returns 0 if successful comparison (aka the same) and non 0 if different
	if(!strcmp(command->cmd, "cd") || !strcmp(command->cmd, "jobs") || !strcmp(command->cmd, "fg")
		|| !strcmp(command->cmd, "exit"))
		return 1;

	return 0;
}


char *get_line_from_stdin(){
	printf("[lab2 dir] >");
	char buffer[BUFFER_SIZE];
	char *line = (char*) malloc(sizeof(char) * BUFFER_SIZE);
	if(line == NULL){
		fprintf(stderr, "There was an issue when allocating in get_line_from_stdin()\n");
		exit(1);
	}

	if(fgets(buffer, BUFFER_SIZE, STDIN) != NULL){
		strncpy(line, buffer, BUFFER_SIZE);
		//line = buffer;
	}
	else{
		fprintf(stderr, "There was an issue reading the line\n");
		exit(1);
	}

	return line;
}
char** read_command(char *line){
	//char buffer[BUFFER_SIZE]; // stdin
	
	char delimiter[] = " \n"; // delimiters to use
	int current_token_index = 0;
	char *token = NULL;

	//int token_count = command_count();
	char **tokens = (char**) (malloc(sizeof(char*) * BUFFER_SIZE));
	if(!tokens){
		fprintf(stderr, "Malloc did not work\n");
		exit(1);
	}

	token = strtok(line, delimiter);
	while(token != NULL){
		tokens[current_token_index] = strdup(token); // make a copy of token
		token = strtok(NULL, delimiter);
		current_token_index++;
	}
	tokens[current_token_index] = token;

	return tokens;
}

int command_count(char* input){
	/*
		Count by default is 0 as a blank line can be considered a command.
		Every time it encounters a pipe it increments as it expects a correct input
	*/
	int count = 0;
	//printf("Inside func count =%d\n", count);

	while(*input != '\n' && *input != '\0') {
		char c = *input;
		//printf("c = %c and count = %d\n", c, count);

		if(c == '|'){
			count++;
		}
		
		input++;
	}

	count++; // since we met a \n
	return count;
}

void free_arrays_of_pointers(char **arr, size_t size){
	// free memory for an array of pointers of a certain size

	for(int i = 0; i < size; i++){
		free(arr[i]);
	}

	free(arr);
}


void testStruct(){
	command*command;
	command = malloc(sizeof(command));
	command->argc = 1;
	//char * str1 = "a";
	//char * str2 = "b";
	command->argv[0] = "str2";
	command->argv[1] = "str1";
	command->cmd = "exit";

	printf("%s\n", command->cmd);
	printf("%s\n", command->argv[0]);
	printf("%s\n", command->argv[1]);

	if(is_built_in(command)) printf("It's a built in command\n");
	else printf("Not a built in command\n");
	return;
}

