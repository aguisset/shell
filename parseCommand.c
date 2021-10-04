#include "parseCommand.h"

/*

A command: ls
Option: -a -b

Note: if we do ls-a in command line this would result in command not found.
So the first space determines what is a command from what is not.

*/


commandList* init_commandList_struct(char* line){
	/* Initialize the struct after parsing commmand */
	return read_command(line);
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

commandList* read_command(char* line){
	/* Will call appropriate helper function depending on pipe's count */
	int command_count = get_command_count(line);

	commandList* commandList = calloc(sizeof(commandList) + command_count * sizeof(command*), sizeof(commandList));
	if(get_pipes_count(line) == 0){
		
		command*command = read_command_with_no_pipes(line);
		commandList->command_count = 1; // we count blank command as one as well!
		commandList->command_list[0] = command;
	} 
	else
		commandList = read_command_with_pipes(line);

	return commandList;
}

command* read_command_with_no_pipes(char* line){
	//char buffer[BUFFER_SIZE]; // stdin
	command* command;
	char delimiter[] = " \n"; // delimiters to use
	int current_token_index = 0;
	int count = 0;
	char *token = NULL;
	command = malloc(sizeof(command));
	if(command == NULL){
		fprintf(stderr, "Error while allocating memory in read_command_with_no_pipes()\n");
		exit(1);
	}

	//int token_count = command_count();

	token = strtok(line, delimiter);

	if(token == NULL){
		fprintf(stderr, "Error: No space to split in read_command_with_no_pipes()");
		exit(1);
	}
	
	command->cmd = token; // first part of split corresponds to the actual command
	
	// getting arguments of command
	while(token != NULL){
		count++;
		command->argv[current_token_index] = token; 
		token = strtok(NULL, delimiter);
		current_token_index++;
	}

	command->argv[current_token_index] = token; // set last arguments to NULL for convenience
	command->argc = count;

	return command;
}


commandList* read_command_with_pipes(char* line){
	commandList* commandList;
	int command_count = get_command_count(line);
	char delimiter[] = "|"; // delimiters to use
	int current_token_index = 0;
	char *token = NULL;

	// we use calloc to initialize directly here
	commandList = calloc(sizeof(commandList) + command_count * sizeof(command*), sizeof(commandList));
	if(commandList == NULL){
		fprintf(stderr, "Error when allocating command list in read_command_with_pipes()\n");
		exit(1);
	}
	commandList->command_count = command_count;
	
	//int token_count = command_count();
	token = strtok(line, delimiter);
	if(token == NULL){
		fprintf(stderr, "Error: No pipes to split in read_command_with_pipes()");
		exit(1);
	}

	while(token != NULL){
		commandList->command_list[current_token_index] = read_command_with_no_pipes(token);
		//tokens[current_token_index] = read_command_with_no_pipes(token); // make a copy of token
		token = strtok(NULL, delimiter);
		current_token_index++;
	}
	commandList->command_list[current_token_index] = NULL; // set last command_list to null

	return commandList;
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
	
	//commandList* commandList;
	commandList* commandList2;
	//char line[] = "ls -a\n"; // STRTOK MODIFIES THE STRING SO USE char arr[] instead
	char line2[] = "cat something | cat somethingAgain\n";
	//commandList = read_command(line);
	commandList2 = read_command(line2);
	/*
	printf("%d\n", commandList->command_count);
	printf("%s\n", commandList->command_list[0]->cmd);
	printf("%d\n", commandList->command_list[0]->argc);
	printf("%s\n", commandList->command_list[0]->argv[0]);
	printf("%s\n", commandList->command_list[0]->argv[1]);
	*/

	printf("%d\n", commandList2->command_count);

	printf("%s\n", commandList2->command_list[1]->cmd);
	printf("%d\n", commandList2->command_list[1]->argc);
	printf("%s\n", commandList2->command_list[1]->argv[0]);
	printf("%s\n", commandList2->command_list[1]->argv[1]);
	
	//printf("%s\n", commandList->command_list[0]->cmd);
	return;
}

void testInitStructure(commandList* commandList){
	printf("----Testing init structure based on sdin----\n");
	printf("command_count = %d\n", commandList->command_count);
	printf("commands:\n");
	for(int i = 0; i < commandList->command_count; i++){
		command*command = commandList->command_list[i];
		printf("Command %d is: [%s] with %d total args:\n", i, command->cmd, command->argc);
		int j = 0;
		while(command->argv[j] != NULL)
			printf("\t%s\n", command->argv[j++]);
	}

	return;
}