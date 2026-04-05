#include<bits/stdc++.h>

using namespace std;

int main() {
    stack<int> a;

    for (int i = 0 ; i < 9 ; ++i) {
        a.push(i);
    }

    a.top() = 5;
}