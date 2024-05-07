#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstring>
#include <signal.h>

int flag = 0;
sem_t *write_sem;
sem_t *read_sem;
void *shared_memory;
int RESULT_SIZE = 64;
int shm_id;

void sig_handler(int signo) {
    sem_close(read_sem);
    sem_unlink("read_semaphore");
    sem_close(write_sem);
    sem_unlink("write_semaphore");
    shmdt(shared_memory);
    shmctl(shm_id, IPC_RMID, NULL);
    exit(EXIT_FAILURE);
}

void* thread_function(void* arg) {
    std::cout << "thread_function started working \n";    
    while(flag == 0) {

        sem_wait(write_sem);
        std::cout << "program 2 is waiting fot taking over the reading reading semaphore" << std::endl;

	std::cout << "program 2 realeased the reading semaphore" << std::endl;
        long path;
        memcpy(&path, shared_memory, sizeof(long));

        std::cout << "Result: " << path << std::endl;

        sem_post(read_sem);
        std::cout << "program 2 has taken over the reading semaphore" << std::endl;
    }
    std::cout << "thread_function ended working \n";
    return NULL;
}

int main() {
    std::cout << "programm started working \n";
    pthread_t thread_id;
    key_t key = ftok("shared_memory", 'R');
    shm_id = shmget(key, RESULT_SIZE, IPC_CREAT | 0666);
    shared_memory = shmat(shm_id, NULL, 0);


    write_sem = sem_open("write_semaphore", O_CREAT, S_IRUSR | S_IWUSR, 0);
    read_sem = sem_open("read_semaphore", O_CREAT, S_IRUSR | S_IWUSR, 0);

    pthread_create(&thread_id, NULL, thread_function, NULL);
    
    signal(SIGINT, sig_handler);

    std::cout << "programm is waiting for the key to be pressed \n" << std::endl;
    getchar();
    std::cout << "key pressed \n";

    flag = 1;

    pthread_join(thread_id, NULL);

    sem_close(read_sem);
    sem_unlink("read_semaphore");
    sem_close(write_sem);
    sem_unlink("write_semaphore");

    shmdt(shared_memory);
    shmctl(shm_id, IPC_RMID, NULL);

    std::cout << "programm ended working \n";
    return 0;
}
