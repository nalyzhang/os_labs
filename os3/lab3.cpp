#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>


int flag1;
int flag2;
int fd[2];
int sizeBuf = 32;

void sig_handler(int signo)
{
	printf("\nget SIGINT; %d\n", signo);
	exit(0);
}

void* proc1(void*)
{
    std::cout << "proc1 started working \n";
    char buf[sizeBuf];
    long path;
    while (flag1 == 0)
    {
    	memset(buf, 0, sizeBuf);
    	path = pathconf("/", _PC_MAX_INPUT);
    	size_t count = (size_t)(sprintf(buf, "%ju", (uintmax_t)path));
        int rv = write(fd[1], &buf, sizeBuf);
        if (rv != -1) {
        	std::cout <<"Writer: " << buf << "\n";
        } else {
        	std::cout << "Failed write with error: " << strerror(rv) << "\n";
        }
        sleep(1);
    }
    std::cout << "proc1 ended working \n";
    pthread_exit((void*)1);

}

void* proc2(void*)
{
    std::cout << "proc2 started working \n";
    char buf[sizeBuf];
    errno = 0;
    while (flag2 == 0)
    {
    	memset(buf, 0, sizeBuf);
    	int rv = read(fd[0], &buf, sizeBuf);
        if(rv == 0) {
        	std::cout << "Reader: no data or end of file \n";
        } else if (rv == -1) {
        	std::cout << "Failed read with error: " << strerror(errno) << "\n";
        } else {
        	std::cout << "Reader: " << buf << "\n";
        }
        sleep(1);
    }
    std::cout << "proc2 ended working \n";
    pthread_exit((void*)2);
}

int main(int argc, char* argv[])
{
    std::cout << "programm started working \n";
    
    pthread_t id1, id2;
    signal(SIGINT, sig_handler);
    int rv;
    
    if (strcmp(argv[2], "1\0") == 0 || strcmp(argv[2], "2\0") == 0 || strcmp(argv[2], "3\0") == 0) {
	    
	    if (strcmp(argv[2], "1\0") == 0) {
	    	std::cout << "pipe is working\n";
	    	rv = pipe(fd);
	    } else if (strcmp(argv[2], "2\0") == 0) {
	    	std::cout << "pipe2 is working\n";
	    	rv = pipe2(fd, O_NONBLOCK);
	    } else if (strcmp(argv[2], "3\0") == 0) {
	    	std::cout << "pipe and fcntl is working\n";
	    	rv = pipe(fd);
	    	fcntl(fd[0], F_SETFL, O_NONBLOCK);
	    	fcntl(fd[1], F_SETFL, O_NONBLOCK);
	    }
	    
	    
	    pthread_create(&id1, NULL, proc1, NULL);
	    pthread_create(&id2, NULL, proc2, NULL);
	    
	    std::cout << "programm is waiting for the key to be pressed \n";
	    getchar();
	    std::cout << "key pressed \n";
	    
	    flag1 = 1;
	    flag2 = 1;
	    
	    pthread_join(id1, NULL);
	    pthread_join(id2, NULL);
	    
	    close(fd[0]);
	    close(fd[1]);
    }
    

    std::cout << "programm ended working \n";
    return 0;
}
