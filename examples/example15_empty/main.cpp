#include <iostream>
#include <string>

int main() {
    while(true){
    std::string name;
    std::cout << "Введите ваше имя: ";
    std::getline(std::cin, name); // считывает строку с пробелами

    std::cout << "Доброе утро, " << name << "!" << std::endl;
    }
    return 0;
}
