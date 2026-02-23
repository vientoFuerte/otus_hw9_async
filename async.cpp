#include "async.h"
#include <sstream>
#include <string>

namespace async {

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

void BulkContext::process(const std::string& line)
{
    std::cout<<line<<std::endl;
}

}  // namespace async









