#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

int main() {

    std::cout << "Программа 1 начала работу" << std::endl;
    
    const char *file_path = "file.txt";
    int uid = 1000; 
    if (chown(file_path, uid, -1) == -1) {
        std::cerr << "Ошибка при изменении владельца файла" << std::endl;
        std::cout << "Программа 2 экстренно завершила работу" << std::endl;
        return 1;
    } else {
        std::cout << "Владелец файла успешно изменен на " << uid << std::endl;
    }
    std::cout << "Программа 1 завершила работу" << std::endl;

    return 0;
}

