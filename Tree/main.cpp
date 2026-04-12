#include <iostream>
#include <vector>
#include<optional>
#include "BinarySearchTree.hpp"
#include "AVLTree.h"
#include "Integer_Test.h"
#include "RedBlackTree.hpp"
using std::cout, std::endl;

void test_opt(std::optional<int> a) {
    cout<<a.value()<<endl;
}

int main() {
    std::vector<Integer> arr = {5,2,3,1,4,7,7,7,7,9};
    BST<Integer> bst1(arr);

    bst1.traverse();
    cout<<endl<<bst1.find_kth_num(7)<<endl<<bst1.Query_rank(7)<<endl;
    bst1.remove(2);
    cout<<endl<<bst1.find_kth_num(7)<<endl<<bst1.Query_rank(7)<<endl;

    AVLT<Integer> avlt1(arr);

    avlt1.traverse();
    cout<<endl<<avlt1.find_kth_num(7)<<endl<<avlt1.Query_rank(7)<<endl;
    avlt1.remove(2);
    cout<<endl<<avlt1.find_kth_num(7)<<endl<<avlt1.Query_rank(7)<<endl;

    // 建议测试序列
    std::vector<int> test_arr = {20, 10, 30, 5, 15, 25, 40, 2, 8, 12, 18, 35, 50, 1, 3, 7, 9, 11, 13, 16, 19};
    AVLT<int> avlt2(test_arr);

    std::vector<int> test_arr_I = {20, 10, 30, 5, 15, 25, 40, 2, 8, 12, 18, 35, 50, 1, 3, 7, 9, 11, 13, 16, 19};
    RBT<int> rbt1(test_arr_I);
    rbt1.traverse();

    cout<<endl<<avlt2.find_kth_num(7)<<" "<<rbt1.find_kth_num(7)<<endl;
    avlt2.remove(2); rbt1.remove(2);
    cout<<endl<<avlt2.find_kth_num(7)<<" "<<rbt1.find_kth_num(7)<<endl;

    return 0;
}