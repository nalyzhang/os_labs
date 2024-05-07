#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <pthread.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/server"

int clientSocket;
int sendFlag = 0;
int receiveFlag = 0;
int id = 1;
pthread_t sendThread, receiveThread;
struct sockaddr_un clientAddress;
    
void finish() {
    pthread_join(sendThread,NULL);
    pthread_join(receiveThread,NULL);
    unlink("/tmp/client");
}

void sig_handler(int signo) {
    finish();
    close(clientSocket);
    std::cout << "Клиент экстренно завершил работу\n";
    exit(EXIT_FAILURE);
}

void* receiveResponses(void *args) {
    std::cout << "Функция передачи запросов начала работу\n";
    char buffer[1024];
    socklen_t len = sizeof(clientAddress);
    

    while (receiveFlag == 0) {
        memset(buffer, 0, sizeof(buffer));

        struct sockaddr_un serverAddr;
        serverAddr.sun_family = AF_UNIX;
        strncpy(serverAddr.sun_path, SOCKET_PATH, sizeof(serverAddr.sun_path) - 1);
        socklen_t len = sizeof(serverAddr);

        
        int rv = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr *) &serverAddr, &len);

        if (rv == -1) {
        	std::cerr << "Ошибка приема ответа" << std::endl;
        	sleep(1);
        } else if (rv == 0) {
        	sleep(1);
        } else {
        	std::cout << "Ответ получен: " << buffer << std::endl;
            sleep(1);
        }
    }
    std::cout << "Функция передачи запросов завершила работу\n";
    pthread_exit(NULL);
}

void* sendRequests(void* args) {
    signal(SIGINT, sig_handler);
    std::cout << "Функция приема ответов начала работу\n";
    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, SOCKET_PATH, sizeof(serverAddr.sun_path) - 1);

    while (sendFlag == 0) {
        std::string message = "Запрос №" + std::to_string(id++);
        if (sendto(clientSocket, message.c_str(), message.length(), 0, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
            std::cerr << "Ошибка отправки запроса" << std::endl;
        } else {
        std::cout << "Запрос отправлен: " << message << std::endl;
        }
        sleep(1);
    }
    std::cout << "Функция приема ответов завершила работу\n";
    pthread_exit(NULL);
}

int main() {
    std::cout << "Сервер начал работу\n";

    clientSocket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Ошибка при создании сокета" << std::endl;
        return 1;
    }

    
    clientAddress.sun_family = AF_UNIX;
    strncpy(clientAddress.sun_path, "/tmp/client",sizeof(clientAddress.sun_path));
    if (bind(clientSocket, (struct sockaddr*)&clientAddress, sizeof(clientAddress)) < 0) {
        std::cout << "Ошибка при привязке сокета к адресу" << std::endl;
        finish();
        return -1;
    }

    pthread_create(&sendThread, NULL, sendRequests,NULL);
    pthread_create(&receiveThread, NULL, receiveResponses,NULL);


    std::cout << "Нажмите любую клавишу для завершения работы" << std::endl;
    getchar();
    std::cout << "Клавиша нажата" << std::endl;

    sendFlag = 1;
    receiveFlag = 1;
    
    close(clientSocket);
    finish();

    std::cout << "Клиент завершил работу\n";
    return 0;
}