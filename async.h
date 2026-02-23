#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>


namespace async {

struct BulkContext {
    std::size_t bulk_size;
    std::vector<std::string> commands;

    BulkContext(std::size_t size) : bulk_size(size) {}

};


BulkContext * connect(std::size_t bulk);
void receive(BulkContext * ctx, const char* data, std::size_t size);
void disconnect(BulkContext * ctx);

} 
