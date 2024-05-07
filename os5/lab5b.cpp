#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

const char* semaphoreName = "/named_semaphore";
const char* fileName = "file.txt";

sem_t* namedSemaphore;
    
void exit_with_error(const char* errorMessage) {
    std::cout << errorMessage << std::endl;
    sem_close(namedSemaphore);
    sem_unlink(semaphoreName);
    exit(-1);
}

int main() {
    std::cout << "Program 2 started working" << std::endl;
    
    namedSemaphore = sem_open(semaphoreName, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (namedSemaphore == SEM_FAILED) {
        exit_with_error("Semaphore opening failed");
    }

    int fileDescriptor = open(fileName, O_CREAT | O_WRONLY | O_NONBLOCK | O_APPEND, S_IRUSR | S_IWUSR);
    if (fileDescriptor == -1) {
        exit_with_error("File opening failed");
    }

    int epoll_fd = epoll_create(1);
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event);

    int flag = 0;
    while (flag == 0) {
        sem_wait(namedSemaphore);
        std::cout << "Program 2 has taken over the semaphore" << std::endl;

        for (int i = 0; i < 10; i++) {
            write(fileDescriptor, "2", 1);
            std::cout << '2' << std::endl;
            sleep(1);
        }
        
        sem_post(namedSemaphore);
        std::cout << "Program 2 has released the semaphore" << std::endl;
        
        struct epoll_event events;
        int events_count = epoll_wait(epoll_fd, &events, 1, 1000);

        if (events_count > 0) {
            if (events.events & EPOLLIN) {
                char key;
                read(0, &key, 1);
                if (key == '\n') {
                    flag = 1;
                    break;
                }
            }
        }
    }

    close(fileDescriptor);
    sem_close(namedSemaphore);
    sem_unlink(semaphoreName);

    std::cout << "Program 2 ended working" << std::endl;
    return 0;
}
