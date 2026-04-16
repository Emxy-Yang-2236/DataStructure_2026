#ifndef HEAP_BINARYHEAP_HPP
#define HEAP_BINARYHEAP_HPP
#include <functional>

template<class cmp = std::less<int>>
class BH {     //max heap
public:
    cmp comparator;

private:
    std::vector<int> heap_;

    void shift_up(int idx, std::vector<int> &heap) {
        while (idx > 1) {
            if (comparator(heap[idx>>1], heap[idx])) {
                std::swap(heap[idx], heap[idx>>1]);
                idx >>= 1;
            }else {
                break;
            }
        }
    }
    void shift_down(int idx, std::vector<int> &heap) {
        while ((idx<<1) < heap.size()) {
            int j = idx<<1;
            if (((idx << 1) | 1) < heap.size() && comparator(heap[idx<<1], heap[(idx << 1) | 1])) {
                ++j;
            }
            if (comparator(heap[idx], heap[j])) {
                std::swap(heap[idx], heap[j]);
                idx = j;
            }else {
                break;
            }
        }
    }

public:
    BH() {
        heap_.emplace_back(-1);
    }
    BH(const std::vector<int> &v) {
        heap_.resize(v.size() + 1);
        heap_[0] = 0;
        for (int i = 1; i <= v.size(); ++i) {
            heap_[i] = v[i-1];
        }

        for (auto i = v.size() / 2; i >= 1; i--) {
            shift_down(i, this->heap_);
        }
    }

    size_t size() {
        return heap_.size() - 1;
    }

    void push(int val) {
        heap_.push_back(val);
        shift_up(size(), this->heap_);
    }
    void pop() {
        if (size() < 1) return;
        heap_[1] = heap_.back();
        heap_.pop_back();
        if (size() > 0) {
            shift_down(1, this->heap_);
        }
    }

    std::vector<int> sort_res() {
        std::vector<int> temp = heap_;
        std::vector<int> ans;

        while (temp.size() > 1) {
            ans.emplace_back(temp[1]);
            temp[1] = temp.back();
            temp.pop_back();
            shift_down(1,temp);
        }

        return ans;
    }

    [[nodiscard]] int top() const { return heap_[1]; }
    [[nodiscard]] bool empty() const { return size() == 0; }
};

#endif //HEAP_BINARYHEAP_HPP
