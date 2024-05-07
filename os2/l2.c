#include <iostream>
#include <pthread.h>
#include <unistd.h>

int flag1;
int flag2;

void* proc1(void*)
{
    std::cout << "поток 1 начал работу \n";
    while (flag1 == 0)
    {
        for (int i = 0; i < 10; i++) {
            std::cout << "1\n";
            sleep(1);
        }
        sleep(1);
    }
    std::cout << "поток 1 завершил работу \n";
    pthread_exit((void*)1);
}

void* proc2(void*)
{
    std::cout << "поток 2 начал работу \n";
    while (flag2 == 0)
    {
        for (int i = 0; i < 10; i++) {
            std::cout << "2\n";
            sleep(1);
        }
        sleep(1);
    }
    std::cout << "поток 2 завершил работу \n";
    pthread_exit((void*)2);
}


int main()
{
    std::cout << "программа начала работу \n";
    
    pthread_t id1, id2;
    
    pthread_create(&id1, NULL, proc1, NULL);
    pthread_create(&id2, NULL, proc2, NULL);
    
    std::cout << "программа ждет нажатия клавиши \n";
    getchar();
    std::cout << "клавиша нажата \n";
    
    flag1 = 1;
    flag2 = 1;
    
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    
    std::cout << "программа завершила работу \n";
    return 0;
}
