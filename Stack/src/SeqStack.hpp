
#ifndef STACK_SEQSTACK_HPP
#define STACK_SEQSTACK_HPP
#include <memory>
#include "BaseClass.hpp"

//顺序实现的栈
template <class elemType>
class seqStack : public stack<elemType> {
private:
    std::unique_ptr<elemType[]> elem;
    int top_p;                           // 栈顶指针，-1 表示空栈
    size_t maxSize;
    void doubleSpace() {
        size_t newSize = maxSize * 2;
        auto newElem = std::make_unique<elemType[]>(newSize);
        for (int i = 0; i <= top_p; ++i) {
            newElem[i] = std::move(elem[i]);
        }
        elem = std::move(newElem);
        maxSize = newSize;
    }
public:
    explicit seqStack(size_t initSize = 16) : elem(std::make_unique<elemType>(initSize)) , top_p(-1) , maxSize(initSize) {}
    ~seqStack() noexcept override = default;

    [[nodiscard]] bool isEmpty() const noexcept override {
        return top_p == -1;
    }

    void push(const elemType& x) override {
        if (top_p + 1 == maxSize) {
            doubleSpace();
        }

        elem[++top_p] = x;
    }

    elemType pop() override {
        return std::move(elem[top_p--]);
    }

    elemType& top() const override {
        return elem[top_p];
    }
};



#endif //STACK_SEQSTACK_HPP