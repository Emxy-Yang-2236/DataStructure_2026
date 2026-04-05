//递归函数的非递归实现
//本质上递归就是利用系统栈来隐式的保存
//现在用显示的栈来模拟系统的行为

#include <vector>


#include "../src/LinkStack.hpp"

struct node {
    int left;
    int right;
    node(int x , int y) : left(x) , right(y) {}
};

int partition(std::vector<int>& arr , int low , int high) {
    int& pivot = arr[high];
    int i = low - 1;

    for (int j = i + 1 ; j < high ; ++j) {
        if (arr[j] <= pivot) {
            std::swap(arr[j] , arr[++i]);
        }
    }
    std::swap(arr[++i] , pivot);

    return i;
}

void QuickSort(std::vector<int>& arr) {
    size_t n = arr.size();
    if (n <= 1) return;

    linkStack<node> st;
    st.push({0 , static_cast<int>(n - 1)});

    while (!st.isEmpty()) {
        auto range = st.top();
        st.pop();

        int low = range.left , high = range.right;
        int p = partition(arr,low,high);

        if (p + 1 < high) {
            st.push({p + 1 , high});
        }
        if (p - 1 < low) {
            st.push({low , p - 1});
        }
    }
}