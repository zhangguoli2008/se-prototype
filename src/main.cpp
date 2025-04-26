#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

// 简单模拟加密：反转字符串
std::string encrypt(const std::string& plain) {
    std::string c = plain;
    std::reverse(c.begin(), c.end());
    return c;
}
// 简单模拟解密
std::string decrypt(const std::string& cipher) {
    std::string p = cipher;
    std::reverse(p.begin(), p.end());
    return p;
}

// 倒排索引类型：关键词 -> 文档列表
using InvertedIndex = std::unordered_map<std::string, std::vector<std::string>>;

// 构建索引：扫描目录下所有 .txt 文件
void build_index(const std::string& dir, InvertedIndex& idx) {
    for (auto& p : fs::recursive_directory_iterator(dir)) {
        if (p.is_regular_file() && p.path().extension() == ".txt") {
            std::ifstream in(p.path());
            std::string word;
            while (in >> word) {
                idx[word].push_back(p.path().string());
            }
        }
    }
    std::cout << "Index built: " << idx.size() << " unique terms." << std::endl;
}

// 查询关键词，返回文档列表
std::vector<std::string> search_index(const InvertedIndex& idx, const std::string& keyword) {
    auto it = idx.find(keyword);
    if (it == idx.end()) return {};
    return it->second;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:
            "
            << "  se-prototype build <data_dir>
            "
            << "  se-prototype search-enc <encrypted_keyword>
            ";
            return 1;
    }
    static InvertedIndex index;
    static bool built = false;

    std::string cmd = argv[1];
    if (cmd == "build") {
        std::string data_dir = argv[2];
        build_index(data_dir, index);
        built = true;
    }
    else if (cmd == "search-enc") {
        if (!built) {
            std::cerr << "Index not built yet. Run 'se-prototype build <data_dir>' first." << std::endl;
            return 1;
        }
        std::string trapdoor = argv[2];
        std::string keyword = decrypt(trapdoor);
        std::cout << "Searching for term: " << keyword << std::endl;
        auto docs = search_index(index, keyword);
        std::cout << "Found " << docs.size() << " documents:" << std::endl;
        for (auto& d : docs) std::cout << "  " << d << std::endl;
    }
    else {
        std::cerr << "Unknown command: " << cmd << std::endl;
        return 1;
    }
    return 0;
}