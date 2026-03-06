#include "async.h"
#include "thread_queue.h"
#include <sstream>
#include <string>
#include <chrono>
#include <atomic>

//для удобной сборки в Visual Studio
#ifdef WIN32
#include <iomanip>
#endif

namespace async {

std::mutex print_mutex;              // мьютекс для вывода в консоль

thread_queue<std::string> log_queue;
thread_queue<std::string> file_queue;

std::thread log_thread;
std::thread file_thread1;
std::thread file_thread2;

void logThreadFunction() {
    std::string cmd;
    while (log_queue.pop(cmd))
    {
        std::cout << cmd<<"\n";
    }
}

void fileThreadFunction(int threadId) {
    std::string cmd;
    while (file_queue.pop(cmd))
    {
        std::cout << cmd << " file\n";
    }
}

std::string generateFilename()
{
    //статический атомарный счётчик
    static std::atomic<unsigned int> counter{ 0 };

#ifdef WIN32
    std::time_t now = std::time(nullptr);
    std::tm timeinfo;
    localtime_s( &timeinfo, &now);
#else
    // Получаем текущее время
    std::time_t now = std::time(nullptr);
    std::tm timeinfo;
    localtime_r(&now, &timeinfo);
#endif
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

void print_block(const std::vector<std::string>& cmds) {
    if(cmds.empty()) {return;}
    std::string filename = generateFilename();
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file." << std::endl;
    }
    std::cout << "\nbulk : ";
    file << "bulk : ";
    
    for (size_t i = 0; i < cmds.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
            file << ", ";
        }
        std::cout << cmds[i];
        file << cmds[i];
    }
    // Файл закроется автоматически при выходе из функции (деструктор ofstream)
    //file.close();
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
        if(depth == 0)
        {
          print_block(commands);
          commands.clear();
        }
        depth++;       
    }
    else if (cmd == "}")
    {
          depth--;
          if(depth<0) {depth = 0;}
          if(depth == 0)
          {
            print_block(commands);
            commands.clear();
          }
    }
    else {
        commands.push_back(cmd);
        if (depth == 0 && commands.size() == bulk_size)
        {
            print_block(commands);
            commands.clear();
        }

    }
}

}  // namespace async






