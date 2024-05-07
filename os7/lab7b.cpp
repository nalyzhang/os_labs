#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <mqueue.h>
#include <signal.h>
#include <cstring>
#include <cstdio>

int flag = 0;
mqd_t mq;
char buf[sizeof(long)];
const char* name = "/myqueue";

void* threadFunction(void *arg){
    std::cout << "thread_function started working \n";
    while (flag == 0){
        memset(buf, 0, sizeof(long));
        long path;
        int rv = mq_receive(mq, buf, sizeof(long), NULL);
        if (rv == -1) {
            perror("mq_recieve");
        } else  if (rv > 0){
            std::cout << "Read message: " << buf << std::endl;
        }
        sleep(1);
    }
    std::cout << "thread_function ended working \n";
    pthread_exit(NULL);
}

int main(){
    std::cout << "program 2 started working \n";
    pthread_t threadID;
    
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(long);
    attr.mq_curmsgs = 0;
    
    mq = mq_open(name,O_CREAT | O_RDONLY | O_NONBLOCK, S_IRUSR | S_IWUSR, &attr);
    
    pthread_create(&threadID, NULL, threadFunction, NULL);

    std::cout << "program 2 is waiting for the key to be pressed \n" << std::endl;
    getchar();
    std::cout << "key pressed \n";
    flag = 1;
    
    pthread_join(threadID, NULL);
    
    mq_close(mq);
    mq_unlink(name);

    std::cout << "program 2 ended working \n";
    return 0;
}

