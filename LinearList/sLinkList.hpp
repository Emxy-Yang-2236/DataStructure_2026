#ifndef LINEAR_LIST_SLINKLIST_HPP
#define LINEAR_LIST_SLINKLIST_HPP

#include <iostream>

template <class elemType>
class sLinkList {
private:
    // 定义内部节点结构
    struct node {
        elemType data;
        node *next;

        // 构造函数：方便创建节点
        node(const elemType &x, node *n = nullptr) : data(x), next(n) {}
        node() : next(nullptr) {}
    };

    node *head; // 头指针，指向头结点
    int currentLength; // 当前长度

    // 辅助函数：找到第 i 个位置的节点（从 -1 开始算，-1 是头结点）
    node *move(int i) const {
        node *p = head;
        while (i-- >= 0) p = p->next;
        return p;
    }

public:
    sLinkList();
    ~sLinkList();

    void clear();
    int length() const { return currentLength; }

    void insert(int i, const elemType &x);
    void remove(int i);
    int search(const elemType &x) const;
    elemType visit(int i) const;

    void traverse() const;
    void traverse(void(*visit)(elemType&));

    bool detectCycle();
};



// 构造函数：初始化一个空的头结点
template <class elemType>
sLinkList<elemType>::sLinkList() {
    head = new node(); // 创建头结点，不存有效数据
    currentLength = 0;
}

// 析构函数：释放所有节点
template <class elemType>
sLinkList<elemType>::~sLinkList() {
    clear();
    delete head; // 最后删掉头结点
}

// 清空表：保留头结点，删除后面所有节点
template <class elemType>
void sLinkList<elemType>::clear() {
    node *p = head->next, *q;
    head->next = nullptr;
    while (p != nullptr) {
        q = p->next;
        delete p;
        p = q;
    }
    currentLength = 0;
}

// 插入：在第 i 个位置插入 x
template <class elemType>
void sLinkList<elemType>::insert(int i, const elemType &x) {
    if (i < 0 || i > currentLength) return; // 下标越界检查

    // 找到第 i-1 个节点（前驱节点）
    node *pos = move(i - 1);

    // 创建新节点并链接
    pos->next = new node(x, pos->next);
    currentLength++;
}

// 删除：删除第 i 个位置的元素
template <class elemType>
void sLinkList<elemType>::remove(int i) {
    if (i < 0 || i >= currentLength) return; // 下标越界

    // 找到第 i-1 个节点
    node *pos = move(i - 1);

    // 保存待删除节点
    node *delNode = pos->next;

    // 重新链接
    pos->next = delNode->next;

    // 释放内存
    delete delNode;
    currentLength--;
}

// 查找：返回 x 第一次出现的位置，没找到返回 -1
template <class elemType>
int sLinkList<elemType>::search(const elemType &x) const {
    node *p = head->next;
    int i = 0;
    while (p != nullptr && p->data != x) {
        p = p->next;
        i++;
    }
    if (p == nullptr) return -1;
    return i;
}

// 访问：返回第 i 个元素的值
template <class elemType>
elemType sLinkList<elemType>::visit(int i) const {
    if (i < 0 || i >= currentLength) throw std::out_of_range("Index out of bounds"); // 简单的越界检查
    return move(i)->data;
}

// 遍历（默认打印）
template <class elemType>
void sLinkList<elemType>::traverse() const {
    node *p = head->next;
    std::cout << std::endl;
    while (p != nullptr) {
        std::cout << p->data << " ";
        p = p->next;
    }
    std::cout << std::endl;
}

// 遍历（通用扩展）
template <class elemType>
void sLinkList<elemType>::traverse(void(*visit)(elemType&)) {
    node *p = head->next;
    while (p != nullptr) {
        visit(p->data);
        p = p->next;
    }
}

template<class elemType>
bool sLinkList<elemType>::detectCycle() {
    node* slow = head ; node* fast = head;
    while (fast->next) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            return true;
        }
    }
    return false;
}


#endif // LINEAR_LIST_SLINKLIST_HPP
