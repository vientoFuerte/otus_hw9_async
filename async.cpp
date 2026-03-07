#include "async.h"
#include "thread_queue.h"
#include <sstream>
#include <string>
#include <chrono>
#include <atomic>
#include <thread>

//для удобной сборки в Visual Studio
#ifdef WIN32
#include <iomanip>
#endif

namespace async {

std::mutex console_mutex;              // мьютекс для вывода в консоль
std::mutex file_mutex;                 // мьютекс для вывода в файл

std::condition_variable console_cv;
std::condition_variable file_cv;

thread_queue<std::vector<std::string>> log_queue;
thread_queue<std::vector<std::string>> file_queue;

std::thread log_thread;
std::thread file_thread1;
std::thread file_thread2;

//Функция для потока вывода в консоль
void logThreadFunction() {
    std::vector <std::string> output_block;
    while(log_queue.pop(output_block))
    {print_block_to_console(output_block);}
}

//Функция для потока вывода в файл
void fileThreadFunction(int threadId) {
    std::vector<std::string> output_block;
    while(file_queue.pop(output_block))
    {print_block_to_file(output_block);}
}

//Функция формирования имени файла, статический счетчик исключает совпадение имен.
std::string generateFilename()
{
    //статический атомарный счётчик
    static std::atomic<unsigned int> counter {0};

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


// вывод блока в консоль потоком log
void print_block_to_console(const std::vector<std::string>& cmds) {
    if(cmds.empty()) {return;}
    std::lock_guard<std::mutex> lock(console_mutex);
    
    std::cout << "\nbulk : ";
  
    for (size_t i = 0; i < cmds.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << cmds[i];
    } 
  }

// запись блока в файл файловым потоком
void print_block_to_file(const std::vector<std::string>& cmds) {
    if(cmds.empty()) {return;}
    std::string filename = generateFilename();
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file." << std::endl;
    }

    file << "bulk : ";
    
    for (size_t i = 0; i < cmds.size(); ++i) {
        if (i > 0) {
            file << ", ";
        }
        file << cmds[i];
    }
    // Файл закроется автоматически при выходе из функции (деструктор ofstream)
    //file.close();
}

BulkContext* connect(std::size_t bulk) {
    auto* ctx = new BulkContext(bulk);
    return ctx;
}

//добавляем сформированные блоки в очереди
void add_block_to_queues(std::vector <std::string> block)
{
    // в очередь для консоли
    {
        std::lock_guard<std::mutex> lock(console_mutex);
        log_queue.push(block);
    }
    console_cv.notify_one();
    
    //в очередь для файлов
    {
        std::lock_guard<std::mutex> lock(file_mutex);
        file_queue.push(block);
    }
    file_cv.notify_one();

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
          add_block_to_queues(commands);
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
            add_block_to_queues(commands);
            commands.clear();
          }
    }
    else {
        commands.push_back(cmd);
        if (depth == 0 && commands.size() == bulk_size)
        {
            add_block_to_queues(commands);
            commands.clear();
        }

    }
}

}  // namespace async






