#include "BinaryHeap.hpp"
#include <iostream>

using namespace std;

void printvec(const vector<int>& v) {
    for (auto i : v) {
        cout<<i<<" ";
    }
    cout<<endl;
}

int main() {
    vector a = {1,2,3,4,5};
    BH bh1(a);

    auto v1 = bh1.sort_res();
    printvec(v1);

    bh1.pop();
    bh1.push(9);
    bh1.push(-1);
    v1 = bh1.sort_res();
    printvec(v1);
}