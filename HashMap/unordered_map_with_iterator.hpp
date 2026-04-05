#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <functional> // for std::hash, std::equal_to
#include <utility>    // for std::pair
#include <stdexcept>  // for std::out_of_range

// --------------------------------------------------------------------------
// SimpleUnorderedMap 类定义
// --------------------------------------------------------------------------
template<
    typename Key,
    typename T,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>
>
class SimpleUnorderedMap {
public:
    // --- 标准类型定义 (Standard Type Definitions) ---
    using key_type        = Key;
    using mapped_type     = T;
    using value_type      = std::pair<const Key, T>; // 注意 Key 是 const 的
    using size_type       = size_t;
    using difference_type = std::ptrdiff_t;
    using hasher          = Hash;
    using key_equal       = KeyEqual;
    using reference       = value_type&;
    using const_reference = const value_type&;

private:
    // --- 内部数据结构 ---
    // 为了简化实现，桶内链表直接存储 value_type
    // 实际 STL 可能使用自定义节点以节省内存
    using BucketType = std::list<value_type>;
    using BucketIterator = typename BucketType::iterator;

    std::vector<BucketType> _buckets;
    size_type _size = 0;
    float _max_load_factor = 1.0f; // 默认最大负载因子

    hasher _hasher;
    key_equal _equal;

    // --- 辅助函数 ---
    size_type bucket_index(const Key& key) const {
        return _hasher(key) % _buckets.size();
    }

public:
    // ----------------------------------------------------------------------
    // 迭代器类 (Forward Iterator)
    // ----------------------------------------------------------------------
    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = std::pair<const Key, T>;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;

    private:
        SimpleUnorderedMap* _map;
        size_type _bucket_idx;
        BucketIterator _list_it;

        friend class SimpleUnorderedMap;

    public:
        iterator(SimpleUnorderedMap* map, size_type idx, BucketIterator list_it)
            : _map(map), _bucket_idx(idx), _list_it(list_it) {}

        // 解引用
        reference operator*() const { return *_list_it; }
        pointer operator->() const { return &(*_list_it); }

        // 前置 ++
        iterator& operator++() {
            ++_list_it; // 链表内前进一步

            // 如果到达当前桶的末尾，寻找下一个非空桶
            if (_list_it == _map->_buckets[_bucket_idx].end()) {
                while (true) {
                    ++_bucket_idx;
                    // 如果所有桶都遍历完了
                    if (_bucket_idx >= _map->_buckets.size()) {
                        // 变为 end() 状态
                        return *this;
                    }
                    // 找到非空桶
                    if (!_map->_buckets[_bucket_idx].empty()) {
                        _list_it = _map->_buckets[_bucket_idx].begin();
                        break;
                    }
                }
            }
            return *this;
        }

        // 后置 ++
        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        // 比较
        bool operator==(const iterator& other) const {
            // 如果都是 end() 状态 (bucket_idx 越界)，视为相等
            if (_bucket_idx >= _map->_buckets.size() &&
                other._bucket_idx >= other._map->_buckets.size()) {
                return true;
            }
            return _bucket_idx == other._bucket_idx && _list_it == other._list_it;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

    // ----------------------------------------------------------------------
    // 构造与析构
    // ----------------------------------------------------------------------
    explicit SimpleUnorderedMap(size_type bucket_count = 8,
                                const Hash& hash = Hash(),
                                const KeyEqual& equal = KeyEqual())
        : _buckets(bucket_count), _hasher(hash), _equal(equal) {
        if (bucket_count == 0) _buckets.resize(8); // 保证最小容量
    }

    // ----------------------------------------------------------------------
    // 迭代器接口
    // ----------------------------------------------------------------------
    iterator begin() {
        for (size_type i = 0; i < _buckets.size(); ++i) {
            if (!_buckets[i].empty()) {
                return iterator(this, i, _buckets[i].begin());
            }
        }
        return end();
    }

    iterator end() {
        return iterator(this, _buckets.size(), {});
    }

    // ----------------------------------------------------------------------
    // 容量查询
    // ----------------------------------------------------------------------
    bool empty() const { return _size == 0; }
    size_type size() const { return _size; }
    size_type bucket_count() const { return _buckets.size(); }
    float load_factor() const { return (float)_size / _buckets.size(); }

    // ----------------------------------------------------------------------
    // 修改操作 (Modifiers)
    // ----------------------------------------------------------------------
    void clear() {
        for (auto& bucket : _buckets) bucket.clear();
        _size = 0;
    }

    // 插入: 返回 pair<iterator, bool>，bool 为 true 表示插入成功，false 表示已存在
    std::pair<iterator, bool> insert(const value_type& value) {
        // 1. 检查扩容
        if (load_factor() > _max_load_factor) {
            rehash(_buckets.size() * 2);
        }

        size_type idx = bucket_index(value.first);
        auto& bucket = _buckets[idx];

        // 2. 检查是否存在
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (_equal(it->first, value.first)) {
                // 已存在，返回指向它的迭代器和 false
                return { iterator(this, idx, it), false };
            }
        }

        // 3. 插入新元素 (头插法效率更高)
        bucket.push_front(value);
        _size++;
        return { iterator(this, idx, bucket.begin()), true };
    }

    // 删除: 通过 Key
    size_type erase(const Key& key) {
        size_type idx = bucket_index(key);
        auto& bucket = _buckets[idx];

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (_equal(it->first, key)) {
                bucket.erase(it);
                _size--;
                return 1; // 返回删除的元素数量
            }
        }
        return 0;
    }

    // operator[]: 如果 Key 不存在则插入默认值
    T& operator[](const Key& key) {
        // 尝试查找
        iterator it = find(key);
        if (it != end()) {
            return it->second;
        }

        // 没找到，插入默认构造的 T
        auto result = insert({key, T()});
        return result.first->second;
    }

    // ----------------------------------------------------------------------
    // 查找操作 (Lookup)
    // ----------------------------------------------------------------------
    iterator find(const Key& key) {
        size_type idx = bucket_index(key);
        auto& bucket = _buckets[idx];

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (_equal(it->first, key)) {
                return iterator(this, idx, it);
            }
        }
        return end();
    }

    size_type count(const Key& key) const {
        // 对于 unordered_map，要么 0 要么 1
        // const_cast 是因为我们复用了非 const 的 find 逻辑，实际可用 const_iterator 优化
        auto* self = const_cast<SimpleUnorderedMap*>(this);
        return (self->find(key) != self->end()) ? 1 : 0;
    }

    // ----------------------------------------------------------------------
    // 哈希策略 (Hash Policy)
    // ----------------------------------------------------------------------
    void rehash(size_type count) {
        if (count <= _buckets.size()) return; // 简化：只扩不缩

        // 1. 创建新桶
        std::vector<BucketType> new_buckets(count);

        // 2. 搬运数据
        // 注意：这里需要重新计算所有元素的 hash
        for (auto& bucket : _buckets) {
            for (auto& val : bucket) {
                size_type new_idx = _hasher(val.first) % count;
                new_buckets[new_idx].push_front(val); // 依然是头插
            }
        }

        // 3. 交换
        _buckets = std::move(new_buckets);
    }
};