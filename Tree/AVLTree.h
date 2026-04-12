//A kind of self-balancing BST, using balance factor to maintain the difference between the lc and rc

#ifndef TREE_AVLTREE_H
#define TREE_AVLTREE_H
#include <functional>
#include <iostream>
#include <memory>

template<class T, class cmp = std::less<T>>
class AVLT {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> lc, rc;
        int count = 0;
        int size = 0;
        int height = 0;

        Node(Node &&other) noexcept :data(std::move(other.data)), lc(std::move(other.lc)), rc(std::move(other.rc)),
            count(other.count), size(other.size), height(other.height){}
        template<typename... Args>
        Node(Args&&... args) : data(std::forward<Args>(args)...), lc(nullptr), rc(nullptr), size(1), count(1) {}

        [[nodiscard]] int getSize() const { return this->size; }
        void update() {
            int left_size = lc ? lc->size : 0;
            int right_size = rc ? rc->size : 0;
            this->size = this->count + left_size + right_size;

            int left_height = lc ? lc->height : 0;
            int right_height = rc ? rc->height : 0;
            this->height = 1 + std::max(left_height, right_height);
        }
        int get_factor() {
            int left_height = lc ? lc->height : 0;
            int right_height = rc ? rc->height : 0;
            return left_height - right_height;
        }
        bool Is_unbalance() {
            return std::abs(get_factor()) > 1;
        }
    };
    std::unique_ptr<Node> root;

    void rotate_right(std::unique_ptr<Node> &node) {
        auto tmp = std::move(node);
        node = std::move(tmp->lc);
        tmp->lc = std::move(node->rc);
        node->rc = std::move(tmp);

        node->rc->update();
        node->update();
    }
    void rotate_left(std::unique_ptr<Node> &node) {
        auto tmp = std::move(node);
        node = std::move(tmp->rc);
        tmp->rc = std::move(node->lc);
        node->lc = std::move(tmp);

        node->lc->update();
        node->update();
    }
    void rotate(std::unique_ptr<Node> &node) {
        if (node->get_factor() > 0) {          //L_ type
            if (node->lc->get_factor() >= 0) {  //LL type  -> rotate node right
                rotate_right(node);
            }else {                            //LR type  -> rotate lc left then node right
                rotate_left(node->lc);
                rotate_right(node);
            }
        }else {                                //R_ type
            if (node->rc->get_factor() <= 0) {  //RR type  -> rotate node left
                rotate_left(node);
            }else {                            //RL type  ->rotate rc right then node left
                rotate_right(node->rc);
                rotate_left(node);
            }
        }
    }

    void insert_in(std::unique_ptr<Node> &node, T val) {
        if (!node) {
            node = std::make_unique<Node>(std::move(val));
            return;
        }
        if (cmp{}(node->data , val)) insert_in(node->rc, std::move(val));
        else if (cmp{}(val, node->data)) insert_in(node->lc, std::move(val));
        else {
            ++node->count;
        }

        node->update();
        if (node->Is_unbalance()) {
            rotate(node);
        }
    }

    bool remove_in(std::unique_ptr<Node> &node, T val){
        if (!node) return false;
        bool res = false;

         if (cmp{}(node->data, val)) {
            res =  remove_in(node->rc, val);
        }else if (cmp{}(val, node->data)){
            res =  remove_in(node->lc, val);
        }else {
            res = true;
            --node->count;
            if (node->count > 0) {
                node->update();
                if (node->Is_unbalance()) {
                    rotate(node);
                }
                return res;
            }
            else {
                if (node->rc == nullptr || node->lc == nullptr) {
                    node = std::move(node->rc ? node->rc : node->lc);
                }else {
                    Node* successor = node->rc.get();
                    while (successor->lc) successor = successor->lc.get();

                    node->data = std::move(successor->data);
                    node->count = successor->count;

                    successor->count = 1;
                    remove_in(node->rc, node->data);
                }
            }
        }

        if (node) {
            node->update();
            if (node->Is_unbalance()) {
                rotate(node);
            }
        }
        return res;
    }

    void In_traverse_in(void (*vis)(T&), std::unique_ptr<Node> &n) {
        if (!n) return;
        In_traverse_in(vis, n->lc);
        vis(n->data);
        In_traverse_in(vis, n->rc);
    }

    bool search_in(std::unique_ptr<Node> &node, T val) {
        if (!node) return false;
        if (cmp{}(node->data, val)) return search_in(node->rc, val);
        if (cmp{}(val, node->data)) return search_in(node->lc, val);
        return true;
    }

    int get_rank_in(std::unique_ptr<Node> &n, const T& val) {
        if (!n) return 1;

        if (val == n->data) {
            return (n->lc ? n->lc->size : 0) + 1;
        } else if (cmp{}(val, n->data)) {
            return get_rank_in(n->lc, val);
        } else {
            return (n->lc ? n->lc->size : 0) + n->count + get_rank_in(n->rc, val);
        }
    }

    int get_size_in(std::unique_ptr<Node> &n) {
        return n->size;
    }

    T kth_num_in(std::unique_ptr<Node> &n, const int k) {
        int left_size = (n->lc ? n->lc->size : 0);
        if (left_size >= k) return kth_num_in(n->lc, k);
        if (left_size + n->count >= k) return n->data;
        else return kth_num_in(n->rc, k - left_size - n->count);
    }

public:

    void insert(T val) {
        insert_in(root, val);
    }
    bool remove(T val) {
        return remove_in(root, val);
    }
    void clear() {
        root = nullptr;
    }

    AVLT(std::vector<T> &arr) {
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            this->insert(*it);
        }
    }

    static void defaule_vis(T& data) {
        std::cout<<data<<" ";
    }

    void traverse(void (*vis)(T&) = defaule_vis) {
        In_traverse_in(vis, root);
    }

    bool search(T val) {
        return search_in(root, val);
    }

    int Query_rank(T val) {
        if (!search(val)) return -1;
        return get_rank_in(root, val);
    }

    int get_size() {
        if (!root) return 0;
        return get_size_in(root);
    }

    int get_height() {
        if (!root) return 0;
        return root->height + 1;
    }
    T find_kth_num(int k) {
        if (k < 1 || k > get_size()) throw std::runtime_error("index out of bound");
        else {
            return kth_num_in(root, k);
        }
    }
};

#endif //TREE_AVLTREE_H
