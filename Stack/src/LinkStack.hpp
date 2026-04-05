//
// Created by Yang Jibin on 2026/3/8.
//

#ifndef STACK_LINKSTACK_HPP
#define STACK_LINKSTACK_HPP

//链接实现的stack
#include <memory>
#include "BaseClass.hpp"

template <class elemType>
class linkStack: public stack<elemType> {
private:
    struct node {
        elemType data;
        std::unique_ptr<node> next;
        node(const elemType& x, std::unique_ptr<node> n = nullptr)
            : data(x), next(std::move(n)) {
        }
    };

    std::unique_ptr<node> top_p;

public:
    linkStack() = default;
    ~linkStack() {
        if (top_p) {
            top_p = std::move(top_p->next); //逐个释放
        }
    }

    [[nodiscard]] bool isEmpty() const override {
        return top_p == nullptr;
    }

    void push(const elemType &x) {
        top_p = std::make_unique<node>(x,std::move(top_p));
    }

    elemType pop() override {
        auto old = std::move(top_p);
        top_p = std::move(old->next);
        return old->data;
    }

    elemType& top() const override {
        return top_p.get()->data;
    }
};

#endif //STACK_LINKSTACK_HPP