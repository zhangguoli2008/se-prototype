#include "index.hpp"
#include <iostream>

namespace Index {
void build_index(const std::vector<std::string>& documents) {
    std::cout << "[Server] Building index for " << documents.size() << " documents...\n";
}

std::vector<int> search(const std::string& keyword) {
    std::cout << "[Server] Searching for keyword: " << keyword << "\n";
    return {1, 2, 3}; // 返回假设的文档ID
}
}
