#include <iostream>
#include <sys/capability.h>
#include <cstring>
#include <cerrno>

int main() {
    
    std::cout << "Программа 2 начала работу" << std::endl;
    
    cap_t caps = cap_get_file("lab9a");

    if (caps == NULL) {
        std::cerr << "Ошибка при чтении возможностей: " << strerror(errno) << std::endl;
        std::cout << "Программа 2 экстренно завершила работу" << std::endl;
        return 1;
    }

    std::cout << "Возможности программы: " << cap_to_text(caps, NULL) << std::endl;

    if (cap_set_file("lab9b", caps) == -1) {
        std::cerr << "Ошибка при применении возможностей: " << strerror(errno) << std::endl;
        cap_free(caps);
        std::cout << "Программа 2 экстренно завершила работу" << std::endl;
        return 1;
    }

    std::cout << "Возможности программы успешно изменены" << std::endl;

    cap_free(caps);
    
    std::cout << "Программа 2 завершила работу" << std::endl;

    return 0;
}

