#include <unistd.h>
#include <stdio.h>
#include <iostream>

int main(int argc, char* argv[], char* envp[]) {
	std::cout << "Program 1 started working\n"<< 
		"ID of process 1:" << getpid() <<
		"\nID of parent process of the program 1:" << getppid() 						
		<< "\nArumets passed to the program 1:\n";
	for (int i = 0; i < argc; i++) {
		std::cout << "Argument " << i+1 << ": " << argv[i] << "\n";
		sleep(1);
	}
	std::cout << "Enviroment variables: \n";
	for (int i = 0; envp[i] != NULL; ++i){
		std::cout << "EV " << i+1 << ": " << envp[i] << "\n";
		sleep(1);
	}
	std::cout << "Program 1 ended working\n";
	exit(4);
}
