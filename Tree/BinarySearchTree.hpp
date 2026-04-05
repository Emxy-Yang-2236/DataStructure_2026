#ifndef TREE_BINARYSEARCHTREE_HPP
#define TREE_BINARYSEARCHTREE_HPP
#include <functional>

template<class T, class cmp = std::less<T>>
class BST {
private:
    struct node {
        T data;
        node* lc , rc;
    };
};

#endif //TREE_BINARYSEARCHTREE_HPP
