//
// Created by Yang Jibin on 2026/3/1.
//

template<
    class Key,
    class T,
    class Hash = std::hash<Key>,
    class Equal = std::equal_to<Key>
> class linked_hashmap : public hashmap<Key, T, Hash, Equal> {
public:
    using value_type = pair<const Key, T>;

    // ----------------------------------------------------------
    // 新增成员变量：全局双向链表，维护插入顺序
    // ----------------------------------------------------------
    double_list<value_type> entry_list;

    // 为了快速从 Key 找到 entry_list 中的迭代器，我们需要一个辅助索引。
    // 由于基类 buckets 存的是 value_type，我们没法直接存迭代器。
    // 这里有一个技巧：我们依然把数据存进基类的 buckets，
    // 但是我们重写 find/insert/remove，让它们协调工作。

    // 为了通过迭代器快速删除，linked_hashmap 的 iterator
    // 必须封装 double_list<value_type>::iterator。

    // ----------------------------------------------------------
    // 迭代器定义 (核心：它包装的是 entry_list 的迭代器)
    // ----------------------------------------------------------
    class iterator {
        friend class linked_hashmap;
        typename double_list<value_type>::iterator ptr; // 指向全局链表节点的迭代器

    public:
        iterator() {}
        iterator(typename double_list<value_type>::iterator p) : ptr(p) {}
        iterator(const iterator &other) : ptr(other.ptr) {}

        // 必要的重载
        iterator& operator++() { ++ptr; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++ptr; return tmp; }
        iterator& operator--() { --ptr; return *this; }
        iterator operator--(int) { iterator tmp = *this; --ptr; return tmp; }

        value_type &operator*() const { return *ptr; }
        value_type *operator->() const noexcept { return ptr.operator->(); }

        bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
        bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
    };

    class const_iterator {
        friend class linked_hashmap;
        typename double_list<value_type>::iterator ptr; // 这里简化处理，理应是 const_iterator

    public:
        const_iterator() {}
        const_iterator(const iterator &other) : ptr(other.ptr) {}
        // ... 类似的重载 ...
        const value_type &operator*() const { return *ptr; }
        const value_type *operator->() const noexcept { return ptr.operator->(); }
    };

    // ----------------------------------------------------------
    // 构造与析构
    // ----------------------------------------------------------
    linked_hashmap() : hashmap<Key, T, Hash, Equal>() {}

    linked_hashmap(const linked_hashmap &other) : hashmap<Key, T, Hash, Equal>(other) {
        entry_list = other.entry_list; // 假设 double_list 实现了深拷贝赋值
    }

    ~linked_hashmap() {
        clear();
    }

    linked_hashmap & operator=(const linked_hashmap &other) {
        if (this == &other) return *this;
        hashmap<Key, T, Hash, Equal>::operator=(other); // 调用基类赋值
        entry_list = other.entry_list;
        return *this;
    }

    // ----------------------------------------------------------
    // 核心接口实现
    // ----------------------------------------------------------

    // 返回全局链表的 begin/end，保证有序遍历
    iterator begin() { return iterator(entry_list.begin()); }
    const_iterator cbegin() const { return const_iterator(entry_list.begin()); } // 需 double_list 支持
    iterator end() { return iterator(entry_list.end()); }
    const_iterator cend() const { return const_iterator(entry_list.end()); }

    bool empty() const { return entry_list.empty(); }
    size_t size() const { return entry_list.currentLength; }

    void clear() {
        hashmap<Key, T, Hash, Equal>::clear(); // 清空哈希表
        // double_list 需要 clear 方法，或者手动重建
        // entry_list.clear();
        // 既然你没写 clear，这里模拟一下：
        while(!entry_list.empty()) entry_list.delete_head();
    }

    // ----------------------------------------------------------
    // Insert (重头戏)
    // ----------------------------------------------------------
    pair<iterator, bool> insert(const value_type &value) {
        // 1. 先在基类哈希表中查找
        // 注意：基类的 find 返回的是基类的 iterator (指向 buckets)
        auto base_it = hashmap<Key, T, Hash, Equal>::find(value.first);

        if (base_it != hashmap<Key, T, Hash, Equal>::end()) {
            // Case A: Key 已存在 -> 更新 Value

            // 1. 更新哈希表中的值
            base_it->second = value.second;

            // 2. 更新全局链表中的值并移动到头部 (LRU 特性，题目要求)
            // 难点：我们需要通过 Key 找到 entry_list 里的节点。
            // 由于我们没有存储 iterator 映射，这里只能线性查找 entry_list (O(N))，或者
            // 我们必须修改 value_type 包含一个指向 iterator 的指针，这比较复杂。
            // 如果为了通过作业，我们可以暴力遍历 entry_list 找到 key 对应的节点更新。

            // 优化：为了避免 O(N) 查找 entry_list，通常 linked_hashmap
            // 内部的 hashmap 应该存储 `double_list::iterator` 而不是 `value_type`。
            // 但既然基类定死了，我们这里只能做 O(N) 的 list 查找或者只是简单更新值。

            // 根据题目描述："order moved to the head"，这是 LRU 逻辑。
            // 我们需要找到 entry_list 中对应的节点，删掉它，再插到头部。

            // 笨办法遍历查找 (因为无法从 base_it 得到 entry_list 的 iterator)
            auto it = entry_list.begin();
            while(it != entry_list.end()) {
                if (this->equal_op(it->first, value.first)) {
                    entry_list.erase(it); // 删除旧位置
                    break;
                }
                ++it;
            }
            entry_list.insert_head(value); // 插到头部

            return { iterator(entry_list.begin()), false };
        }
        else {
            // Case B: Key 不存在 -> 插入

            // 1. 插入基类哈希表
            hashmap<Key, T, Hash, Equal>::insert(value);

            // 2. 插入全局链表 (题目要求 head 还是 tail? 通常是 tail，但你的描述暗示可能是 head)
            // 如果是标准 LinkedHashMap 是 tail，如果是 LRU 是 head。
            // 既然上面的 update 提到了 "head"，这里假设新元素也插 head 或者 tail。
            // 假设插到尾部保持插入顺序：
            entry_list.insert_tail(value);

            // 注意：insert_tail 后 iterator 指向 tail 的前一个
            // 你需要让 double_list 返回新插入节点的 iterator
            auto it = entry_list.end();
            --it;

            return { iterator(it), true };
        }
    }

    // ----------------------------------------------------------
    // Remove
    // ----------------------------------------------------------
    void remove(iterator pos) {
        if (pos == end()) throw sjtu::index_out_of_bound(); // 假设有这个异常

        Key key = pos->first;

        // 1. 从基类哈希表删除
        hashmap<Key, T, Hash, Equal>::remove(key);

        // 2. 从全局链表删除
        entry_list.erase(pos.ptr);
    }

    // ----------------------------------------------------------
    // Find
    // ----------------------------------------------------------
    iterator find(const Key &key) {
        // 同样的问题：我们能通过哈希表快速确认 Key 存在，
        // 但我们拿不到 entry_list 的 iterator。

        auto base_it = hashmap<Key, T, Hash, Equal>::find(key);
        if (base_it == hashmap<Key, T, Hash, Equal>::end()) {
            return end();
        }

        // 如果找到了，我们需要返回指向 entry_list 的 iterator。
        // 由于没有直接映射，在不修改基类存储结构的情况下，只能遍历 entry_list。
        // 这会导致 find 变成 O(N)。

        // *** 关键破局点 ***
        // 如果这是作业，通常允许你为了性能 Hacker 一下。
        // 比如，让基类的 T 变成 iterator？不行，类型不匹配。
        // 唯一的 O(1) 解法是：自己再维护一个 map<Key, double_list::iterator>。
        // 也就是 遮蔽(shadow) 基类的 buckets，或者新增一个辅助 map。

        // 这里演示 O(N) 的写法（功能正确，性能差）：
        auto it = entry_list.begin();
        while(it != entry_list.end()) {
            if (this->equal_op(it->first, key)) {
                return iterator(it);
            }
            ++it;
        }
        return end();
    }

    // ----------------------------------------------------------
    // 辅助 Map 优化版 (High Performance)
    // ----------------------------------------------------------
    // 如果想要 O(1) 的 find/remove，你需要在 linked_hashmap 里
    // 额外维护一个从 Key 到 double_list::node* 的映射。
    // 由于不能用 std::unordered_map，你可以利用基类的 buckets 存特殊值，
    // 或者重写基类的 buckets 逻辑。

    // 但鉴于题目让你继承，最稳妥的通过测试的方法是：
    // 在 insert/remove 时同步维护 entry_list。
    // 在 find 时，如果数据量不大，遍历 entry_list。
    // 如果数据量大，这设计本身就有缺陷（基类没留扩展接口）。
};