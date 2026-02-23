// main.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#include <atomic>
#include <iostream>
#include <fstream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>

std::string generateFilename()
{
    //статический атомарный счётчик
    static std::atomic<unsigned int> counter{0};
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


int main()
{
    const std::string output_file1 = "output1.txt";
    const std::string output_file2 = "output2.txt";

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
}

