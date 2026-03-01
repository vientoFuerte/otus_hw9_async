// main.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#include <iostream>
#include <fstream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include "async.h"


int main(int argc, char* argv[])
{
  
    //Демонстрационная часть
    size_t N = (argc == 2) ? std::atoi(argv[1]) : 0; // Получаем параметр из командной строки или устанавливаем его в 0 по умолчанию
    
    // Тестовые данные
    std::vector<std::string> test_input = {
        "cmd1", "cmd2", "cmd23", "cmd24","cmd25","{", "cmd3", "cmd4", "}",
        "{", "cmd5", "cmd6", "{", "cmd7", "cmd8", "}", "cmd9", "}",
        "{", "cmd10", "cmd11"
    };
    
    //пока фиксируем N
    N = 3;
    
    // Создаём два контекста
    auto ctx1 = async::connect(N);
    auto ctx2 = async::connect(N);    
    
    // Передаём команды обработчикам
    const char testCmd1[] = "cmd1\ncmd2\ncmd3\ncmd4\ncmd5\n";
    const char testCmd2[] = "cmd6\n{\ncmd7\ncmd8\n}\ncmd9\n";
    //передаем строку без нулевого символа, инача может быть лишняя запятая.
    async::receive(ctx1, testCmd1, sizeof(testCmd1)-1);  
    async::receive(ctx2, testCmd2, sizeof(testCmd2)-1);
    
    // Завершаем оба контекста
    async::disconnect(ctx1);
    async::disconnect(ctx2);

    std::cout << "Demo finished. Check generated log files." << std::endl;
    
}


