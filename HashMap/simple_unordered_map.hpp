#pragma once

#include <iostream>
#include <vector>
#include <list>     //用于实现链表（处理哈希冲突）
#include <functional> //用于 std::hash
#include <utility>  //用于 std::pair

template <typename Key, typename Value>
class SimpleUnorderedMap {
private:
    // 定义桶中的节点类型：存储 Key-Value 对
    using HashNode = std::pair<Key, Value>;

    // 核心数据结构：Bucket Array（桶数组）
    // std::vector 代表数组，std::list 代表桶里的链表
    std::vector<std::list<HashNode>> buckets;

    size_t _size = 0; // 当前元素个数

    // 哈希函数对象
    std::hash<Key> hasher;

    // 辅助函数：计算 Key 对应的桶索引
    size_t get_bucket_index(const Key& key) const {
        // 1. 计算哈希值
        size_t hash_code = hasher(key);
        // 2. 取模运算，映射到数组范围内
        return hash_code % buckets.size();
    }

public:
    // 构造函数：初始化桶的数量（例如 8 个）
    SimpleUnorderedMap(size_t bucket_count = 8) {
        buckets.resize(bucket_count);
    }

    // ---------------------------------------------------------
    // 算法 1: 插入 (Insert)
    // ---------------------------------------------------------
    void insert(const Key& key, const Value& value) {
        // 检查负载因子，如果太拥挤则扩容 (Load Factor > 1.0)
        if ((double)_size / buckets.size() >= 1.0) {
            rehash();
        }

        // 1. ���位桶
        size_t index = get_bucket_index(key);

        // 2. 遍历链表，检查 Key 是否已存在（去重）
        auto& chain = buckets[index]; // 引用该桶的链表
        for (auto& node : chain) {
            if (node.first == key) {
                node.second = value; // 如果存在，更新值（覆盖）
                return;
            }
        }

        // 3. 如果不存在，插入到链表头部
        chain.push_back({key, value});
        _size++;
    }

    // ---------------------------------------------------------
    // 算法 2: 查找 (Find)
    // ---------------------------------------------------------
    // 返回指针，如果没找到返回 nullptr (为了简化演示，不使用迭代器)
    Value* find(const Key& key) {
        // 1. 定位桶
        size_t index = get_bucket_index(key);

        // 2. 遍历链表查找
        auto& chain = buckets[index];
        for (auto& node : chain) {
            if (node.first == key) {
                return &node.second; // 找到，返回 Value 的地址
            }
        }

        // 3. 没找到
        return nullptr;
    }

    // ---------------------------------------------------------
    // 算法 3: 删除 (Erase)
    // ---------------------------------------------------------
    bool erase(const Key& key) {
        // 1. 定位桶
        size_t index = get_bucket_index(key);

        // 2. 遍历链表
        auto& chain = buckets[index];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (it->first == key) {
                // 3. 找到并移除
                chain.erase(it);
                _size--;
                return true;
            }
        }
        return false;
    }

    // ---------------------------------------------------------
    // 算法 4: 扩容 (Rehash) - 最昂贵的操作
    // ---------------------------------------------------------
    void rehash() {
        // 1. 记录旧桶数据
        auto old_buckets = buckets;

        // 2. 数组大小翻倍
        size_t new_bucket_count = buckets.size() * 2;
        buckets.clear();
        buckets.resize(new_bucket_count);
        _size = 0; // 重置 size，insert 会重新加回来

        std::cout << "[System] 触发扩容：桶大小从 " << old_buckets.size()
                  << " -> " << new_bucket_count << std::endl;

        // 3. 数据迁移：遍历旧桶中每一个元素，重新插入新桶
        for (auto& chain : old_buckets) {
            for (auto& node : chain) {
                insert(node.first, node.second); // 重新走一遍哈希计算
            }
        }
    }

    // 打印当前内部结构（调试用）
    void print_debug() {
        for (size_t i = 0; i < buckets.size(); ++i) {
            std::cout << "Bucket " << i << ": ";
            for (auto& node : buckets[i]) {
                std::cout << "[" << node.first << ":" << node.second << "] -> ";
            }
            std::cout << "nullptr" << std::endl;
        }
    }
};

