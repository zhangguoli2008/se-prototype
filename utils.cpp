#include "utils.hpp"
#include <sstream>

namespace Utils {
std::vector<std::string> split(const std::string& text, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(text);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        tokens.push_back(item);
    }
    return tokens;
}
}
