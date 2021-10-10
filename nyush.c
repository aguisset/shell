#include "common.h"
#include "parseCommand.h"
#include "runCommand.h"

void init_shell(){

	while(1){
		char* base_dir;
		char* line;
		commandList* commandList;

		base_dir = get_base_dir();
		printf("[nyush %s]$ _", base_dir);
		line = get_line_from_stdin();
		commandList = init_commandList_struct(line);
		testInitStructure(commandList);
	}
}
int main(int argc, char ** argv){
	//char *line = get_line_from_stdin();
	//read_command(line);
	//testStruct();
	//test_built_in();
	//test_my_system();
	//testStruct();
 	//testRedirection();
	//init_shell();
	return 0;
}

/*
Resources:
- Different types of whitespace in C: https://user-web.icecube.wisc.edu/~dglo/c_class/charfunc.html#:~:text=In%20ASCII%2C%20whitespace%20characters%20are,formfeed%20(%20'%5Cf'%20).
- "Advanced Programming in the Unix environment", W.Richard Stevens and Steven A.Rago
- Flexible array members in a structure: https://www.geeksforgeeks.org/flexible-array-members-structure-c/
- How to split strings in C: https://www.youtube.com/watch?v=34DnZ2ewyZo&ab_channel=CodeVault
- Valgrind errors: https://stackoverflow.com/questions/9300227/tracking-down-valgrind-40-bytes-in-1-blocks-are-definitely-lost-in-loss-record
- How to use strstr: https://www.tutorialspoint.com/c_standard_library/c_function_strstr.htm
*/