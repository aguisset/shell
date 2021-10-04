#include "common.h"
#include "parseCommand.h"
#include "runCommand.h"

void init_shell(){

	while(1){
		char* base_dir;
		char* line;

		base_dir = get_base_dir();
		printf("[nyush %s]$ _", base_dir);
		line = get_line_from_stdin();

	}
}
int main(int argc, char ** argv){
	//char *line = get_line_from_stdin();
	//read_command(line);
	//testStruct();
	//test_built_in();
	//test_my_system();
	
	
	//init_shell();
	return 0;
}

/*
Resources:
- Different types of whitespace in C: https://user-web.icecube.wisc.edu/~dglo/c_class/charfunc.html#:~:text=In%20ASCII%2C%20whitespace%20characters%20are,formfeed%20(%20'%5Cf'%20).
- "Advanced Programming in the Unix environment", W.Richard Stevens and Steven A.Rago
- Flexible array members in a structure: https://www.geeksforgeeks.org/flexible-array-members-structure-c/
- How to split strings in C: https://www.youtube.com/watch?v=34DnZ2ewyZo&ab_channel=CodeVault
*/