#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <pthread.h>
#include <queue>

#define SOCKET_PATH "/tmp/server"

std::queue<std::string> requestQueue;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int serverSocket;
int reveiveFlag = 0;
int processFlag = 0;
pthread_t receiveThread, processThread;
struct sockaddr_un clientAddr;

void finish() {

    pthread_join(receiveThread, NULL);
    pthread_join(processThread, NULL);
    close(serverSocket);
    unlink(SOCKET_PATH);

}

void sig_handler(int signo) {
    finish();

    std::cout << "Сервер экстренно завершил работу\n";
    exit(EXIT_FAILURE);
}

void *receiveRequests(void* args) {
    std::cout << "Функция приема запросов начала работу\n";
    
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buffer[1024];

    while (reveiveFlag == 0) {
        memset(buffer, 0, sizeof(buffer));
        int rv = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr *) &clientAddr, &clientAddrLen);

        if (rv == -1) {
        	std::cerr << "Ошибка приема запроса" << std::endl;
        	sleep(1);
        } else if (rv == 0) {
        	sleep(1);
        } else {
            std::string clientAddress = clientAddr.sun_path;
            pthread_mutex_lock(&mutex);
            std::string request(buffer);
            requestQueue.push(request);
        	std::cout << "Запрос получен: " << request << std::endl;
        	pthread_mutex_unlock(&mutex);
    		getsockname(serverSocket, (sockaddr*)&clientAddress, &clientAddrLen);
    		std::cout << "Адрес сокета клиента: " << clientAddr.sun_path << std::endl;
            sleep(1);
        }
    }
    std::cout << "Функция приема запросов завершила работу\n";
    pthread_exit(NULL);
}

void *processRequests(void* args) {
    std::cout << "Функция приема ответов начала работу\n";

    while (processFlag == 0) {
        pthread_mutex_lock(&mutex);
        if (requestQueue.empty() == 0) {
            std::string request = requestQueue.front();
            long path = pathconf("/", _PC_PATH_MAX);
        	std::string response = std::to_string(path);
            if (sendto(serverSocket, response.c_str(), response.length(), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) == -1) {
                std::cerr << "Ошибка передачи ответа клиенту" << std::endl;
            }
            std::cout << "Переданный ответ: " << response << std::endl;
            requestQueue.pop();
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }else {
        	std::cout << "Очередь запроса пуста" << std::endl;
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
    }
    std::cout << "Функция приема ответов завершила работу\n";
    pthread_exit(NULL);
}

int main() {
    signal(SIGINT, sig_handler);
    std::cout << "Сервер начал работу\n";
    serverSocket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        return 1;
    }

    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, SOCKET_PATH, sizeof(serverAddr.sun_path) - 1);

    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Ошибка привязки сокета к адресу" << std::endl;
        finish();
        return 1;
    }

    pthread_create(&receiveThread, NULL, receiveRequests, NULL);
    pthread_create(&processThread, NULL, processRequests, NULL);

    std::cout << "Нажмите любую клавишу для завершения работы" << std::endl;
    getchar();
    std::cout << "Клавиша нажата" << std::endl;
    reveiveFlag = 1;
    processFlag = 1;

    finish();

    std::cout << "Сервер завершил работу"<< std::endl;
    
    return 0;
}


