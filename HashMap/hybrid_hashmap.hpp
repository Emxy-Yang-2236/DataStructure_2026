//
// Created by Yang Jibin on 2026/3/1.
//

#ifndef HASHMAP_HYBRID_HASHMAP_HPP
#define HASHMAP_HYBRID_HASHMAP_HPP

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <variant> // C++17: 用于表示 "要么是链表，要么是树"
#include <algorithm>

// 定义阈值
const size_t TREEIFY_THRESHOLD = 8;
const size_t UNTREEIFY_THRESHOLD = 6;

template <typename Key, typename Value>
class HybridHashMap {
private:
    // 定义两种存储结构
    using ListType = std::list<std::pair<Key, Value>>;
    using TreeType = std::map<Key, Value>; // std::map 底层就是红黑树

    // 桶的类型：使用 variant 来动态切换状态
    // std::monostate 代表桶是空的
    using Bucket = std::variant<std::monostate, ListType, TreeType>;

    std::vector<Bucket> buckets;
    std::hash<Key> hasher;
    size_t _size = 0;

    size_t get_index(const Key& key) const {
        return hasher(key) % buckets.size();
    }

public:
    HybridHashMap(size_t bucket_count = 16) {
        buckets.resize(bucket_count);
    }

    // ------------------------------------------------
    // 核心逻辑：插入 (Put)
    // ------------------------------------------------
    void put(const Key& key, const Value& value) {
        size_t idx = get_index(key);
        auto& bucket = buckets[idx];

        // 情况 1: 桶是空的 -> 初始化为链表
        if (std::holds_alternative<std::monostate>(bucket)) {
            bucket = ListType(); // 赋值为空链表
        }

        // 情况 2: 桶是链表
        if (std::holds_alternative<ListType>(bucket)) {
            auto& list = std::get<ListType>(bucket);

            // 2.1 检查是否存在，存在则更新
            for (auto& pair : list) {
                if (pair.first == key) {
                    pair.second = value;
                    return;
                }
            }

            // 2.2 不存在，追加到链表
            list.push_back({key, value});
            _size++;

            // 2.3 *** 树化检查 ***
            if (list.size() >= TREEIFY_THRESHOLD) {
                treeify_bucket(idx); // 链表 -> 红黑树
            }
        }
        // 情况 3: 桶已经是红黑树
        else if (std::holds_alternative<TreeType>(bucket)) {
            auto& tree = std::get<TreeType>(bucket);
            tree[key] = value; // std::map 插入/更新是 O(log N)
            _size++;
            // 注意：这里其实应该判断是插入还是更新来维护 size，简化处理
        }
    }

    // ------------------------------------------------
    // 核心逻辑：树化 (Treeify)
    // ------------------------------------------------
    void treeify_bucket(size_t idx) {
        auto& bucket = buckets[idx];
        // 只能从链表转树
        if (!std::holds_alternative<ListType>(bucket)) return;

        std::cout << "[System] Bucket " << idx << " 达到阈值，升级为红黑树!" << std::endl;

        // 取出旧链表
        ListType old_list = std::get<ListType>(bucket);

        // 创建新红黑树
        TreeType new_tree;
        for (const auto& pair : old_list) {
            new_tree[pair.first] = pair.second;
        }

        // 替换桶内容
        bucket = new_tree;
    }

    // ------------------------------------------------
    // 核心逻辑：反树化 (Untreeify)
    // ------------------------------------------------
    void untreeify_bucket(size_t idx) {
        auto& bucket = buckets[idx];
        if (!std::holds_alternative<TreeType>(bucket)) return;

        std::cout << "[System] Bucket " << idx << " 元素减少，退化为链表。" << std::endl;

        TreeType old_tree = std::get<TreeType>(bucket);
        ListType new_list;

        for (const auto& pair : old_tree) {
            new_list.push_back(pair);
        }

        bucket = new_list;
    }

    // ------------------------------------------------
    // 查找 (Get)
    // ------------------------------------------------
    Value* get(const Key& key) {
        size_t idx = get_index(key);
        auto& bucket = buckets[idx];

        // 1. 桶为空
        if (std::holds_alternative<std::monostate>(bucket)) {
            return nullptr;
        }
        // 2. 桶是链表 -> O(N) 遍历
        else if (std::holds_alternative<ListType>(bucket)) {
            auto& list = std::get<ListType>(bucket);
            for (auto& pair : list) {
                if (pair.first == key) return &pair.second;
            }
        }
        // 3. 桶是红黑树 -> O(log N) 查找
        else if (std::holds_alternative<TreeType>(bucket)) {
            auto& tree = std::get<TreeType>(bucket);
            auto it = tree.find(key);
            if (it != tree.end()) return &it->second;
        }
        return nullptr;
    }

    // ------------------------------------------------
    // 删除 (Remove) - 包含退化逻辑
    // ------------------------------------------------
    void remove(const Key& key) {
        size_t idx = get_index(key);
        auto& bucket = buckets[idx];

        if (std::holds_alternative<TreeType>(bucket)) {
            auto& tree = std::get<TreeType>(bucket);
            tree.erase(key);
            _size--;

            // *** 退化检查 ***
            if (tree.size() <= UNTREEIFY_THRESHOLD) {
                untreeify_bucket(idx);
            }
        }
        else if (std::holds_alternative<ListType>(bucket)) {
            auto& list = std::get<ListType>(bucket);
            for (auto it = list.begin(); it != list.end(); ++it) {
                if (it->first == key) {
                    list.erase(it);
                    _size--;
                    return;
                }
            }
        }
    }

    // 调试打印
    void print_status() {
        for (size_t i = 0; i < buckets.size(); ++i) {
            std::cout << "Bucket " << i << ": ";
            if (std::holds_alternative<std::monostate>(buckets[i])) {
                std::cout << "[Empty]";
            } else if (std::holds_alternative<ListType>(buckets[i])) {
                std::cout << "[List] size=" << std::get<ListType>(buckets[i]).size();
            } else if (std::holds_alternative<TreeType>(buckets[i])) {
                std::cout << "[RBT Tree] size=" << std::get<TreeType>(buckets[i]).size();
            }
            std::cout << std::endl;
        }
    }
};

// 模拟哈希冲突攻击的 Hash 函数
struct BadHash {
    size_t operator()(int key) const {
        return 0; // 极其邪恶：所有 Key 都映射到 Bucket 0
    }
};



#endif //HASHMAP_HYBRID_HASHMAP_HPP