#include "parseCommand.h"

/*

A command: ls
Option: -a -b

Note: if we do ls-a in command line this would result in command not found.
So the first space determines what is a command from what is not.

*/


commandList* init_commandList_struct(char* line){
	/* Initialize the struct after parsing commmand */
	int command_count = get_command_count(line);
	commandList* commands;

	// we use calloc to initialize directly here
	commands = calloc(sizeof(commandList) + command_count * sizeof(command*), sizeof(commands));
	if(commands == NULL){
		fprintf(stderr, "Error when allocating command list in init_command_struct()\n");
		exit(1);
	}
	commands->command_count;

	return commands;
}

char** get_command_list(char* line){
	char** command_list = (char**) malloc(sizeof(char*)*);

	char buff[BUFFER_SIZE];
    char delim[] = "|";
    strcpy(buff, line);
    int count = 0;
    char* piece = strtok(buff, delim);
    
    while(piece != NULL){

        piece = strtok(NULL, delim);
    }
	return command_list;
}
/* // might need to delete
int hasPipe(char* line){
	// Given line taken from stdin, we check whether or not the command has pipes.
	//   Return number of pipes.
	 

    char buff[BUFFER_SIZE];
    char delim[] = "|";
    strcpy(buff, line);
    int count = 0;
    char* piece = strtok(buff, delim);
    
    if(piece) count++;
    while(piece != NULL){
        count++;
        piece = strtok(NULL, delim);
    }
    printf("%d\n", count); // for debug
    if(count %2 != 0) printf("Invalid command \n"); // for debug

    return count;
}

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
char* get_base_dir(){
	
	char buff[MAX_PATH_LENGTH];
	char delimiter[] = "/"; // delimiters to use
	char *token = NULL;

	char *base_dir = (char*) malloc(sizeof(char) * MAX_PATH_LENGTH);
	if(base_dir == NULL){
		fprintf(stderr, "Error when allocating string in get_base_dir()\n");
		exit(1);
	}
	memset(base_dir, ' ', MAX_PATH_LENGTH);
	memset(buff, ' ', MAX_PATH_LENGTH);
	
	//printf("buff values: %c\n", buff[0]);
	getcwd(buff, MAX_PATH_LENGTH);

	char* prev = NULL;
	token = strtok(buff, delimiter);
	while(token != NULL){
		prev = strdup(token);
		token = strtok(NULL, delimiter);
	}

	if(prev == NULL)
		strcpy(base_dir, token);
	else
		strcpy(base_dir, prev);
	
	return base_dir;
}

char *get_line_from_stdin(){
	/*
		This function returns the line from stdin
	*/
	char buffer[BUFFER_SIZE];
	char *line = (char*) malloc(sizeof(char) * BUFFER_SIZE);
	memset(buffer, ' ', BUFFER_SIZE);
	if(line == NULL){
		fprintf(stderr, "There was an issue when allocating in get_line_from_stdin()\n");
		exit(1);
	}

	if(fgets(buffer, BUFFER_SIZE, STDIN) != NULL){
		strncpy(line, buffer, BUFFER_SIZE);
		//line = buffer;
	}
	else{
		fprintf(stderr, "There was an issue reading the line in get_line_from_stdin()\n");
		exit(1);
	}

	return line;
}

char ** read_command(char* line){
	/* Will call appropriate helper function depending on pipe's count */
	return get_pipes_count(line) == 0 ? read_command_with_no_pipes(line) : read_command_with_pipes(line);
}

char** read_command_with_no_pipes(char* line){
	//char buffer[BUFFER_SIZE]; // stdin
	
	char delimiter[] = " \n"; // delimiters to use
	int current_token_index = 0;
	char *token = NULL;

	//int token_count = command_count();
	char **tokens = (char**) (malloc(sizeof(char*) * BUFFER_SIZE));
	if(tokens == NULL){
		fprintf(stderr, "Error while allocating memory in read_command_with_no_pipe()\n");
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

char** read_command_with_pipes(char* line){
	char **tokens = (char**) (malloc(sizeof(char*) * BUFFER_SIZE));
	if(tokens == NULL){
		fprintf(stderr, "Error while allocating memory in read_command_with_pipe()\n");
		exit(1);
	}

	char delimiter[] = "|"; // delimiters to use
	int current_token_index = 0;
	char *token = NULL;

	//int token_count = command_count();
	token = strtok(line, delimiter);
	while(token != NULL){
		tokens[current_token_index] = strdup(token); // make a copy of token
		token = strtok(NULL, delimiter);
		current_token_index++;
	}
	tokens[current_token_index] = token;
}

int get_pipes_count(char* line){
	int count = 0;

	while(*line != '\0'){
		if(*line == '|')
			count++;
		line++;
	}

	return count;
}
int get_command_count(char* line){
	/*
		Count by default is 0 as a blank line can be considered a command.
		Every time it encounters a pipe it increments as it expects a correct input
	*/
	int count = 0;
	//printf("Inside func count =%d\n", count);
	
	// Invalid command
	if(line[0] == '|' || line[strlen(line)-1] == '|'){
		fprintf(stderr, "Error: Invalid command\n");
		return 0; // I want to stop right here
	}

	while(*line != '\n' && *line != '\0') {
		char c = *line;
		//printf("c = %c and count = %d\n", c, count);

		if(c == '|'){
			count++;
		}
		
		line++;
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

/*
void testHasPipe(){
	char *line = "There is a pipe in this string"; // should pass
	if(hasPipe(line) > 0)
		printf("There is a pipe\n");
	else if(hasPipe(line) == 0)
		printf("There is no pipe\n");
}
*/
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

