#ifndef INDEX_HPP
#define INDEX_HPP

#include <vector>
#include <string>

namespace Index {
void build_index(const std::vector<std::string>& documents);
std::vector<int> search(const std::string& keyword);
}

#endif
