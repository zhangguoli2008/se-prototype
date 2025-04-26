#include "index.hpp"
#include "crypto.hpp"
#include <iostream>
#include <vector>

void run_server() {
    std::vector<std::string> documents = {"doc1 content", "doc2 content", "doc3 content"};
    Index::build_index(documents);
}
