#ifndef LINEAR_LIST_DLINKLIST_HPP
#define LINEAR_LIST_DLINKLIST_HPP

#include <iostream>
#include <iterator>

template <class T>
class dLinkList {
private:
    // 1. 节点定义
    struct node {
        T data;
        node *prev, *next;

        node(const T &x, node *p = nullptr, node *n = nullptr)
            : data(x), prev(p), next(n) {}
        node() : prev(nullptr), next(nullptr) {}
    };

    node *head, *tail; // 头尾哨兵，简化边界判断
    int currentLength;

public:
    // 2. 迭代器定义 (核心部分)
    class iterator {
    private:
        node *ptr; // 迭代器本质就是封装了一个指针
        friend class dLinkList; // 让 dLinkList 能访问 ptr

    public:
        // 迭代器特质 (Iterator Traits) - 为了适配 STL 算法
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;

        // 构造函数
        explicit iterator(node *p = nullptr) : ptr(p) {}

        // 解引用操作
        T& operator*() const { return ptr->data; }
        T* operator->() const { return &ptr->data; }

        // 前置 ++ (推荐)
        iterator& operator++() {
            ptr = ptr->next;
            return *this;
        }
        // 后置 ++ (不推荐，有拷贝开销)
        iterator operator++(int) {
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        // 前置 --
        iterator& operator--() {
            ptr = ptr->prev;
            return *this;
        }
        // 后置 --
        iterator operator--(int) {
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        bool operator==(const iterator &other) const { return ptr == other.ptr; }
        bool operator!=(const iterator &other) const { return ptr != other.ptr; }
    };

    // 暴露迭代器接口
    iterator begin() { return iterator(head->next); }
    iterator end() { return iterator(tail); } // end指向尾哨兵



    dLinkList() {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
        currentLength = 0;
    }

    ~dLinkList() {
        clear();
        delete head;
        delete tail;
    }

    [[nodiscard]] int size() const { return currentLength; }

    void clear() {
        node *p = head->next;
        while (p != tail) {
            node *tmp = p;
            p = p->next;
            delete tmp;
        }
        head->next = tail;
        tail->prev = head;
        currentLength = 0;
    }

    // 在迭代器 pos 之前插入 x
    void insert(iterator pos, const T &x) {
        node *p = pos.ptr;
        node *newNode = new node(x, p->prev, p);
        p->prev->next = newNode;
        p->prev = newNode;
        currentLength++;
    }

    // 删除迭代器 pos 指向的节点，返回下一个迭代器
    iterator erase(iterator pos) {
        node *p = pos.ptr;
        iterator ret(p->next);

        p->prev->next = p->next;
        p->next->prev = p->prev;

        delete p;
        currentLength--;
        return ret;
    }

    void push_back(const T &x) { insert(end(), x); }
    void push_front(const T &x) { insert(begin(), x); }

    // 将 list 的所有节点移动到 pos 之前
    void splice(iterator pos, dLinkList &list) {
        // 1. 自身检测：如果是自己 splice 自己，什么都不做，直接返回
        if (&list == this) return;

        if (list.currentLength == 0) return;

        // 2. 核心移动操作 (transfer 负责指针修改)
        transfer(pos, list.begin(), list.end());

        this->currentLength += list.currentLength;
        list.currentLength = 0; // 对方被掏空
    }

    void reverse() {
        if (currentLength < 2) return;

        // 策略：遍历链表，把每个节点都移到 begin() 之前
        // 但为了简单，直接从第二个节点开始，依次移到最前面
        iterator first = begin();
        ++first; // 从第二个节点开始

        while (first != end()) {
            iterator old = first;
            ++first; // 先保存下一个
            transfer(begin(), old, first); // 把 old 移到 begin() 之前
            // 每次移动完成后 first 永远是最开始的第一个后面的那个 node
        }
    }

    void merge(dLinkList &other) {
        iterator it1 = begin();
        iterator it2 = other.begin();

        while (it1 != end() && it2 != other.end()) {
            if (*it2 < *it1) {
                iterator next2 = it2;
                ++next2;
                // 将 it2 移动到 it1 之前
                transfer(it1, it2, next2);
                it2 = next2;
                this->currentLength++;
                other.currentLength--;
            } else {
                ++it1;
            }
        }
        // 如果 other 还有剩余，全部接过来
        if (it2 != other.end()) {
            splice(end(), other);
        }
    }

    // 补充：精确移动 [first, last) 来自 other 的节点
    void splice(iterator pos, dLinkList &other, iterator first, iterator last) {
        if (first == last) return;

        if (&other != this) {
            int count = 0;
            iterator it = first;
            while (it != last) {
                ++count;
                ++it;
            }

            // 维护长度
            this->currentLength += count;
            other.currentLength -= count;
        }
        this->transfer(pos, first, last);
    }

    void sort() {
        if (currentLength <= 1) return;

        dLinkList<T> carry;
        dLinkList<T> counter[64]; // 模拟 64 层归并栈
        int fill = 0;

        while (currentLength > 0) {
            // 取出一个节点放入 carry
            carry.splice(carry.begin(), *this, begin(), ++begin()); // 移动一个节点

            int i = 0;
            while(i < fill && counter[i].currentLength > 0) {
                counter[i].merge(carry);
                carry.swap(counter[i]);
                i++;
            }
            carry.swap(counter[i]);
            if (i == fill) fill++;
        }

        for (int i = 1; i < fill; ++i) {
            counter[i].merge(counter[i-1]);
        }
        swap(counter[fill-1]);
    }

    // 简单的 swap 用于辅助
    void swap(dLinkList &other) {
        std::swap(head, other.head);
        std::swap(tail, other.tail);
        std::swap(currentLength, other.currentLength);
    }

protected:
    // 将 [first, last) 移动到 position 之前
    void transfer(iterator position, iterator first, iterator last) {
        if (position == last) return; // 原地移动，无需操作
        // 注意：这里没有检查 position 是否在 [first, last) 内部，调用者需保证

        node *posNode = position.ptr;
        node *firstNode = first.ptr;
        node *lastNode = last.ptr; // last 指向区间末尾的下一个节点
        node *beforeLast = lastNode->prev; // 区间真正的最后一个节点

        // 1. 将 [first, beforeLast] 从原位置摘除
        firstNode->prev->next = lastNode;
        lastNode->prev = firstNode->prev;

        // 2. 插入到 posNode 之前
        firstNode->prev = posNode->prev;
        beforeLast->next = posNode;

        posNode->prev->next = firstNode;
        posNode->prev = beforeLast;
    }
};

#endif //LINEAR_LIST_DLINKLIST_HPP