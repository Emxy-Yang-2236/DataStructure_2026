//
// Created by Yang Jibin on 2026/3/1.
//

#ifndef HASHMAP_SIMPLE_LINKED_HASHMAP_HPP
#define HASHMAP_SIMPLE_LINKED_HASHMAP_HPP

#include "dLinkList.hpp"
#include "simple_unordered_map.hpp"

template <typename Key, typename Value>
class SimpleLinkedHashMap {
private:
    // 实际存储的数据类型 (Key + Value)
    using Entry = std::pair<Key, Value>;

    // 1. 物理存储数据的双向链表 (维护插入顺序)
    dLinkList<Entry> orderList;

    // 2. 哈希表 (维护 Key -> 链表节点的映射)
    // 注意：Map 的 Value 存的是链表节点的指针！
    SimpleUnorderedMap<Key, typename dLinkList<Entry>::Node*> indexMap;

public:
    // ------------------------------------------------
    // 增 (Put) - O(1)
    // ------------------------------------------------
    void put(const Key& key, const Value& value) {
        // 1. 检查 Key 是否已存在
        auto nodePtrPtr = indexMap.find(key); // 返回的是 (Node*) 的指针

        if (nodePtrPtr != nullptr) {
            // Case A: Key 已存在 -> 更新值
            // nodePtrPtr 是指向 "链表节点指针" 的指针
            // *nodePtrPtr 是 "链表节点指针" (Node*)
            typename dLinkList<Entry>::Node* node = *nodePtrPtr;

            // 更新链表中的实际数据
            node->data.second = value;

            // (可选) 如果要做 LRU，这里可以把 node 移动到链表尾部
        } else {
            // Case B: Key 不存在 -> 新增

            // 1. 先插到链表尾部，拿到节点指针
            auto newNode = orderList.push_back({key, value});

            // 2. 把 <Key, 节点指针> 存入哈希表
            indexMap.insert(key, newNode);
        }
    }

    // ------------------------------------------------
    // 查 (Get) - O(1)
    // ------------------------------------------------
    // 返回 Value 的指针，没找到返回 nullptr
    Value* get(const Key& key) {
        auto nodePtrPtr = indexMap.find(key);
        if (nodePtrPtr != nullptr) {
            // 通过哈希表找到链表节点，再取数据
            return &((*nodePtrPtr)->data.second);
        }
        return nullptr;
    }

    // ------------------------------------------------
    // 删 (Remove) - O(1)
    // ------------------------------------------------
    bool remove(const Key& key) {
        // 1. 在哈希表中查找
        auto nodePtrPtr = indexMap.find(key);
        if (nodePtrPtr == nullptr) {
            return false; // 没找到
        }

        // 2. 拿到链表节点指针
        auto nodeToDelete = *nodePtrPtr;

        // 3. 从链表中删除 (O(1)，因为我们有节点指针)
        orderList.erase(nodeToDelete);

        // 4. 从哈希表中删除 Key
        indexMap.erase(key);

        return true;
    }

    // ------------------------------------------------
    // 遍历 (Iterate) - 按插入顺序
    // ------------------------------------------------
    void print_ordered() {
        std::cout << "遍历 (插入顺序): ";
        orderList.print();
    }

    // 调试用：看哈希表内部结构
    void print_debug() {
        std::cout << "哈希表内部结构 (无序):" << std::endl;
        indexMap.print_debug();
    }
};

#endif //HASHMAP_SIMPLE_LINKED_HASHMAP_HPP