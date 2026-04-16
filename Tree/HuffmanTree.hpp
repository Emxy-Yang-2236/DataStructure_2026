#ifndef TREE_HUFFMANTREE_HPP
#define TREE_HUFFMANTREE_HPP

#include <iostream>
#include <map>
#include <optional>
#include <memory>
#include <string>
#include <queue>

class HFMT {
    struct Node{
        std::optional<char> ch_;
        int frequency_ = 0;
        std::unique_ptr<Node> lc_ ,rc_;
        Node(std::optional<char> ch = std::nullopt, int frequency = 0):
            ch_(ch),frequency_(frequency) , lc_(nullptr), rc_(nullptr){}
        [[nodiscard]] bool isLeaf() const {
            return ch_.has_value();
        }
    };

    struct Cmp {
        bool operator()(const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) const {
            return a->frequency_ > b->frequency_; // 小顶堆
        }
    };

    std::unique_ptr<Node> root;

public:
    std::map<char, std::string> encodingTable;

    void generateCodes(const Node* node, const std::string& currentPath) {
        if (!node) return;

        if (node->isLeaf()) {
            encodingTable[node->ch_.value()] = currentPath;
            return;
        }

        generateCodes(node->lc_.get(), currentPath + "0");
        generateCodes(node->rc_.get(), currentPath + "1");
    }

    HFMT(const std::string& s) {
        if (s.empty()) return;
        std::map<char,int> mp;
        for (auto i : s) {
            ++mp[i];
        }

        std::priority_queue<std::unique_ptr<Node>, std::vector<std::unique_ptr<Node>>, Cmp> minHeap;
        for (auto const& [ch, freq] : mp) {
            minHeap.push(std::make_unique<Node>(ch, freq));
        }

        while (minHeap.size() > 1) {
            auto leftNode = std::move(const_cast<std::unique_ptr<Node>&>(minHeap.top()));
            minHeap.pop();
            auto rightNode = std::move(const_cast<std::unique_ptr<Node>&>(minHeap.top()));
            minHeap.pop();

            // 创建内部结点 (data 为 std::nullopt)
            auto parent = std::make_unique<Node>(std::nullopt, leftNode->frequency_ + rightNode->frequency_);
            parent->lc_ = std::move(leftNode);
            parent->rc_ = std::move(rightNode);

            minHeap.push(std::move(parent));
        }

        root = std::move(const_cast<std::unique_ptr<Node>&>(minHeap.top()));
        minHeap.pop();
        generateCodes(root.get(), "");
    }

    void printCodes() const {
        std::cout << "--- Huffman Encodings ---" << std::endl;
        for (auto const& [ch, code] : encodingTable) {
            std::cout << "'" << ch << "': " << code << std::endl;
        }
    }
};

#endif //TREE_HUFFMANTREE_HPP
