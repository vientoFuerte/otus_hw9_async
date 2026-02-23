// main.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#include <atomic>
#include <iostream>
#include <fstream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include "async.h"

std::string generateFilename()
{
    //статический атомарный счётчик
    static std::atomic<unsigned int> counter{0};
    // Задержка чтобы имена файлов гарантированно отличались
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    // Получаем текущее время
    std::time_t now = std::time(nullptr);
    std::tm timeinfo;
    localtime_r(&now, &timeinfo);

    // Формируем имя файла
    std::string filename = "bulk";
    filename += std::to_string(timeinfo.tm_mday);
    filename += std::to_string(timeinfo.tm_mon + 1);
    filename += std::to_string(timeinfo.tm_year + 1900);
    filename += std::to_string(timeinfo.tm_hour);
    filename += std::to_string(timeinfo.tm_min);
    filename += std::to_string(timeinfo.tm_sec);
    
    // Счетчик увеличивается каждом вызове, гарантируя уникальность имени.
    filename += "_" + std::to_string(++counter);
    
    filename += ".log";

    return filename;
}

std::string outputBlockStart(std::ofstream& file)
{
    std::string filename = generateFilename();
    // Открываем файл для записи в бинарном режиме
    file.open(filename, std::ios::binary | std::ios::app);
    bool res = file.is_open();
    if (!res)
    {
        std::cerr << "Failed to open file." << std::endl;
    }

    return filename;
}

void outputBlockStop(std::ofstream& file)
{
    if (file.is_open())
    {
        std::cout << "\n";
        // Закрываем файл
        file.close();
    }
}

void log_to_console(const std::string& log_data) {
    std::cout << "Console log: " << log_data << std::endl;
}

void write_to_file(const std::string& output_file, const std::string& log_data) {
    std::ofstream output_stream(output_file, std::ios::app); // Открываем файл в режиме добавления
    if (!output_stream.is_open()) {
        std::cerr << "Failed to open output file: " << output_file << std::endl;
        return;
    }
    output_stream << "File log: " << log_data << std::endl;
    output_stream.close();
}


int main(int argc, char* argv[])
{
    const std::string output_file1 = generateFilename();
    const std::string output_file2 = generateFilename();

    std::string log_data;

    // Создаем три потока
    std::thread log(log_to_console, log_data);
    std::thread file1(write_to_file, output_file1, log_data);
    std::thread file2(write_to_file, output_file2, log_data);

    // Заполняем log_data с помощью std::cin
    std::cout << "Enter log data (or type 'exit' to finish): ";
    std::getline(std::cin, log_data);

    // Ожидаем завершения всех потоков
    log.join();
    file1.join();
    file2.join();

    std::cout << "All threads have finished." << std::endl;
    
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
    auto handle1 = async::connect(N);
    auto handle2 = async::connect(N);    
    
    // Передаём команды обработчикам
    async::receive(handle1, "cmd1\ncmd2\ncmd3\ncmd4\ncmd5\n", 25);  
    async::receive(handle2, "cmd6\n{\ncmd7\ncmd8\n}\ncmd9\n", 28);
    
    // Завершаем оба контекста
    async::disconnect(handle1);
    async::disconnect(handle2);

    std::cout << "Demo finished. Check generated log files." << std::endl;
    
}

