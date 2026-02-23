#include "async.h"
#include <sstream>
#include <string>
#include <chrono>
#include <atomic>

namespace async {

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
    file.open(filename, std::ios::binary | std::ios::app);
    if (!file.is_open())
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
        file.close();
    }
}


void printBlock(const std::vector<std::string>& block, std::ostream& file) {
    std::cout << "bulk : ";
    file << "bulk : ";
    
    for (size_t i = 0; i < block.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
            file << ", ";
        }
        std::cout << block[i];
        file << block[i];
    }
}

BulkContext* connect(std::size_t bulk) {
    auto* ctx = new BulkContext(bulk);
    return ctx;
}

void receive(BulkContext* ctx, const char* data, std::size_t size) {
    if (ctx){
      //  копируем в строку size символов из data
      std::string input(data, size);
      //чтобы работать как с входным потоком
      std::istringstream stream(input);
      std::string line;
      
      while (std::getline(stream, line))  {
        ctx->process(line);
      }
    
    }

}

void disconnect(BulkContext* ctx) {
    if (ctx) {
        delete ctx;
    }
}

void BulkContext::process(const std::string& cmd)
{
    if (cmd.empty()) return;
    
    if (cmd == "{")
    {
    
    }
    else if (cmd == "}")
    {
    
    }
    
    
    std::cout<<cmd<<std::endl;
}

}  // namespace async



