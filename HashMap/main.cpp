#include <iostream>

#include "simple_unordered_map.hpp"

int main() {
    SimpleUnorderedMap<std::string, int> map;

    // 测试插入
    map.insert("Apple", 100);
    map.insert("Banana", 200);
    map.insert("Cherry", 300);

    // 这里的 "Durian" 可能会导致冲突，或者最终触发扩容
    // 为了演示扩容，我们多加几个
    for(int i=0; i<10; ++i) {
        map.insert("Item" + std::to_string(i), i);
    }

    // 测试查找
    if (int* val = map.find("Banana")) {
        std::cout << "Found Banana: " << *val << std::endl;
    }

    // 测试删除
    map.erase("Apple");

    std::cout << "\n最终哈希表结构：" << std::endl;
    map.print_debug();

    return 0;
}
