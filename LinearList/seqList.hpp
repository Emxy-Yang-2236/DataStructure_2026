#ifndef LINEAR_LIST_SEQLIST_HPP
#define LINEAR_LIST_SEQLIST_HPP

#include <iostream>

template<class elemType>
class seqList {
private:
    elemType *data;
    int currentLength;
    int maxSize;

    void doubleSpace();
    void smartshrink();

public:
    seqList(int initSize = 10);

    ~seqList() { delete [] data; }
    void clear() { currentLength = 0; }
    int length() const { return currentLength; }
    int size() const { return maxSize; }

    void insert(int i, const elemType &x);

    void remove(int i);
    void remove(int i , int j);

    void resize(int newSize);

    int search(const elemType &x) const;

    elemType visit(int i) const { return data[i]; }

    void traverse(void(*visit)(elemType&));
    void print_list();
};



template<class elemType>
seqList<elemType>::seqList(int initSize) {
    data = new elemType[initSize];
    maxSize = initSize;
    currentLength = 0;
}

template<class elemType>
int seqList<elemType>::search(const elemType &x) const {
    for (int i = 0; i < currentLength && data[i] != x; ++i) {
        if (i == currentLength) return -1;
        else return i;
    }
    return -1;
}

template<class elemType>
void seqList<elemType>::traverse(void (*visit)(elemType&)) {
    for (int i = 0; i < currentLength; ++i)
        visit(data[i]); // 对每个元素调用 visit 函数
}

template<class elemType>
void seqList<elemType>::print_list() {
    auto print_element = [](elemType& ele)->void {
        std::cout<<ele<<std::endl;
    };

    traverse(print_element);
}

template <class elemType>
void seqList<elemType>::insert(int i, const elemType &x)
{
    if (currentLength == maxSize)
        doubleSpace();   //扩容
    for ( int j = currentLength; j > i; j--)   //元素移动
        data[j] = data[j-1];
    data[i] = x;
    ++currentLength;
}

template<class elemType>
void seqList<elemType>::doubleSpace() {
    elemType *tmp = data;
    maxSize *= 2;
    data = new elemType[maxSize];
    for (int i = 0; i < currentLength; ++i) {
        data[i] = std::move(tmp[i]);
    }
    delete[] tmp;
}

template<class elemType>
void seqList<elemType>::smartshrink() {
    while (currentLength > 0 && currentLength <= maxSize / 4 && maxSize > 10) {
        resize(maxSize / 2);
    }
}

template <class elemType>
void seqList<elemType>::remove(int i)
{
    for (int j = i; j < currentLength - 1; j++)
        data[j] = data[j+1];
    --currentLength;
    smartshrink();
}

template<class elemType>
void seqList<elemType>::remove(int i, int j) {
    while(j<currentLength)
        data[i++]=data[j++];
    currentLength=i;
    smartshrink();
}

template <class elemType>
void seqList<elemType>::resize(int newSize) {
    elemType *tmp = data;
    data = new elemType[newSize];

    for (int k = 0; k < currentLength; ++k) {
        data[k] = std::move(tmp[k]);
    }
    delete[] tmp;
    maxSize = newSize;
}


#endif //LINEAR_LIST_SEQLIST_HPP
