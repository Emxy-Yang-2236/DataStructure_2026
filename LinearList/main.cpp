#include <algorithm>
#include <iostream>
#include <vector>
#include "seqList.hpp"

using namespace std;
void PrintVec(const vector<int>& v) ;


vector<int> trial = {38, 27, 43, 3, 9, 82, 10};
void merge(vector<int>& arr1 , vector<int>& arr2) {
    vector<int> tmp;
    int i = 0 , j = 0;
    while (i <arr1.size()&& j < arr2.size()) {
        if (arr1[i] < arr2[j]) {
            tmp.emplace_back(arr1[i++]);
        }else {
            tmp.emplace_back(arr2[j++]);
        }
    }
    while (i <arr1.size()) tmp.emplace_back(arr1[i++]);
    while (j < arr2.size()) tmp.emplace_back(arr2[j++]);
    arr2.clear();
    arr1 = tmp;
}
void BottomupMergeSort_trial(vector<int>& arr){
    vector<vector<int>> counter(64); 
    vector<int> carry;
    int fill = 0;

    for (int num : arr){
        carry.insert(carry.begin(),num);

        int i = 0;
        while (i < fill && !counter[i].empty()) {
            merge(carry,counter[i]);
            ++i;
        }

        counter[i] = std::move(carry);
        //carry.clear();

        if (i == fill) ++fill;
    }
    for (int i = 1; i < fill; ++i) {
        merge(counter[i],counter[i-1]);
    }
    arr = counter[fill-1];
}

void PrintVec(const vector<int>& v) {
    for (auto i : v) {
        cout<<i<<" ";
    }
    cout<<endl;
}

int main() {
    PrintVec(trial);
    BottomupMergeSort_trial(trial);
    PrintVec(trial);
    return 0;
}