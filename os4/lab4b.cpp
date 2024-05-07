#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <iostream>

int main(int argc, char* argv[], char* envp[]) {
	std::cout << "Program 2 started working\n"<< 
		"ID of process 2:" << getpid() <<
		"\nID of parent process of the program 2:" << getppid() 						
		<< "\n";
	pid_t pid = fork();
	if (pid == 0) {
		int rv = execvpe("lab4a", argv, envp);
		if (rv == -1) {
			std::cout << "Child program execution error\n";
			exit(1);
		}
	}
	else if (pid > 0) {
		std::cout <<"ID of the child process of the program 2:" << pid << "\n";
		int status, code;
		while ((code = waitpid(pid, &status, WNOHANG)) == 0) {
			std::cout << "Program is waiting for the child program to terminate \n";
			usleep(500000);
		}
		if (code == -1) {
			std::cout << "Waitpid error \n";
		} else {
			std::cout << "Child process of program 2 was terminated with code:  " << WEXITSTATUS(code) << "\n";
		}
		
	} else {
		std::cout << "Process of child program execution error\n";
	}
	std::cout << "Program 2 ended working\n";
	return 0;
}
