# se-prototype

本项目演示了一个最简对称可搜索加密 (SSE) 原型：

- **build**：构建倒排索引，参数为文档目录
- **search-enc**：进行加密关键词检索，参数为反转后的关键词

## 编译
```bash
mkdir build && cd build
cmake ..
make