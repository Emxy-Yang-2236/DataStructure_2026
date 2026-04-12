//This is a test class without defalt integer

#ifndef TREE_INTEGER_TEST_H
#define TREE_INTEGER_TEST_H

#include <iostream>

class Integer {
    int val;
public:
    Integer() = delete;
    Integer(int val_): val(val_) {}

    bool operator==(const Integer &other) const {
        return other.val == this->val;
    }
    bool operator<(const Integer &other) const {
        return this->val < other.val;
    }
    bool operator>(const Integer &other) const {
        return this->val > other.val;
    }
    int value() const{return val;}

    friend class ostream;
};

std::ostream& operator<<(std::ostream &os, Integer i) {
    os<<i.value();
    return os;
}

#endif //TREE_INTEGER_TEST_H
