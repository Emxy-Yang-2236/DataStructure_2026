//fxxking complicated

#ifndef TREE_REDBLACKTREE_HPP
#define TREE_REDBLACKTREE_HPP
#include <functional>
#include <iostream>
#include <memory>
#include <optional>

enum class Color {
    RED,
    BLACK,
};


template<class T, class cmp = std::less<T>>
class RBT {
    struct Node : public std::enable_shared_from_this<Node>{
        std::optional<T> data;
        Color color;
        std::shared_ptr<Node> lc, rc;
        std::weak_ptr<Node> parent;
        int count{}, size{};

        std::shared_ptr<Node> get_parent() {
            return parent.lock();
        }
        std::shared_ptr<Node> get_grandparent() {
            return get_parent() == nullptr ? nullptr : get_parent()->get_parent();
        }
        std::shared_ptr<Node> get_uncle() {
            return get_grandparent() == nullptr ? nullptr :
                (get_grandparent()->lc == get_parent() ? get_grandparent()->rc : get_grandparent()->lc);
        }
        std::shared_ptr<Node> get_sibling() {
            return get_parent() == nullptr ? nullptr :
                (get_parent()->lc.get() == this ? get_parent()->rc : get_parent()->lc);
        }

        void switch_color() {
            color = color == Color::BLACK ? Color::RED : Color::BLACK;
        }

        Node(const Color c, std::shared_ptr<Node> p, std::optional<T> d = std::nullopt)
            :data(d), color(c), parent(p), lc(nullptr), rc(nullptr), count(1), size(1) {}
        Node (Node && other)noexcept : data(std::move(other.data)), color(other.color), lc(std::move(other.lc)),rc(std::move(other.rc)),
            parent(std::move(other.parent)), count(other.count), size(other.size) {}

        [[nodiscard]] int getSize() const { return this->size; }
        void update() {
            if (this->lc == nullptr && this->rc == nullptr) {   //check NIL
                this->size = 0;
                return;
            }

            int left_size = lc ? lc->size : 0;
            int right_size = rc ? rc->size : 0;
            this->size = this->count + left_size + right_size;
        }
    };

    std::shared_ptr<Node> root;
    std::shared_ptr<Node> NIL;

    void init_NIL(){
        NIL = std::make_shared<Node>(Color::BLACK, nullptr);
        NIL->size = NIL->count = 0;
        NIL->lc = NIL->rc = nullptr;
    }

    std::shared_ptr<Node> insert_in(std::shared_ptr<Node> &node, std::shared_ptr<Node> &p, T val) {
        if (node == NIL) {
            node = std::make_shared<Node>(Color::RED, p, val);
            node->lc = node->rc = NIL;
            return node;
        }

        std::shared_ptr<Node> ret = nullptr;
        if (cmp{}(node->data.value() , val))
            ret = insert_in(node->rc, node, std::move(val));
        else if (cmp{}(val, node->data.value()))
            ret = insert_in(node->lc, node, std::move(val));
        else {
            ++node->count;
            ret = node;
        }

        node->update();
        return ret;
    }

    void rotate_left(std::shared_ptr<Node> node) {
        auto child = node->rc;
        node->rc = child->lc;
        if (child->lc != NIL) child->lc->parent = node;

        auto parent = node->parent.lock();
        child->parent = parent;
        if (!parent || parent == NIL) {
            root = child;
        }else if (parent->lc == node) {
            parent->lc = child;
        }else {
            parent->rc = child;
        }

        child->lc = node;
        node->parent = child;

        node->update();
        child->update();
    }
    void rotate_right(std::shared_ptr<Node> node) {
        auto child = node->lc;
        node->lc = child->rc;
        if (child->rc != NIL) child->rc->parent = node;

        auto parent = node->parent.lock();
        child->parent = parent;
        if (!parent || parent == NIL) {
            root = child;
        }else if (parent->rc == node) {
            parent->rc = child;
        }else {
            parent->lc = child;
        }

        child->rc = node;
        node->parent = child;

        node->update();
        child->update();
    }

    void insert_fix(std::shared_ptr<Node> node) {
        while (node != root && node->get_parent()->color == Color::RED) {
            //node->color = Color::BLACK;

            auto parent = node->get_parent();
            auto grandparent = node->get_grandparent();
            auto uncle = node->get_uncle();

            if (grandparent && grandparent->lc == parent) {     //parent is the left child of gp, rotation type is L_
                //auto uncle = grandparent->rc;
                if (uncle->color == Color::RED) {
                    uncle->switch_color();
                    parent->switch_color();
                    grandparent->switch_color();
                    node = grandparent;
                }else {
                    if (parent->rc == node) {                    //LR
                        node = parent;
                        rotate_left(node);
                        parent = node->get_parent();
                        grandparent = node->get_grandparent();
                    }
                    parent->switch_color();                      //LL
                    grandparent->switch_color();
                    node = grandparent;
                    rotate_right(node);
                    //parent is the initial node, which is black;
                }
            }else {                                              //parent is the right child of gp, rotation type is R_
                if (uncle->color == Color::RED) {
                    uncle->switch_color();
                    parent->switch_color();
                    grandparent->switch_color();
                    node = grandparent;
                }else {
                    if (parent->lc == node) {                     //RL
                        node = parent;
                        rotate_right(node);
                        parent = node->get_parent();
                        grandparent = node->get_grandparent();
                    }
                    parent->switch_color();                       //LL
                    grandparent->switch_color();
                    node = grandparent;
                    rotate_left(node);
                }
            }
        }
    }

    void fix_double_black(std::shared_ptr<Node> node) {
        if (node == NIL || node == root) return;
        if (node->color == Color::RED) {
            node->switch_color();
            return ;
        }
        auto parent = node->get_parent();
        auto sibling = node->get_sibling();

        if (sibling->color == Color::RED) {
            sibling->switch_color();
            parent->switch_color();
            if (parent->lc == node) {
                rotate_left(parent);
            }else {
                rotate_right(parent);
            }
            sibling = (parent->lc == node) ? parent->rc : parent->lc;
        }
            if (sibling->rc->color == Color::BLACK && sibling->lc->color == Color::BLACK) {
                sibling->switch_color();
                fix_double_black(parent);
            }else {
                if (parent->rc == node) {                      //L_ type
                    if (sibling->lc->color == Color::RED){     //LL type
                        sibling->lc->color = sibling->color;
                        sibling->color = parent->color;
                        parent->color = Color::BLACK;
                        rotate_left(parent);
                    }else {                                     //LR type
                        sibling->rc->color = parent->color;
                        parent->color = Color::BLACK;
                        rotate_left(sibling);
                        rotate_right(parent);
                    }
                }else {
                    if (sibling->rc->color == Color::RED){     //RR type
                        sibling->rc->color = sibling->color;
                        sibling->color = parent->color;
                        parent->color = Color::BLACK;
                        rotate_right(parent);
                    }else {                                     //RL type
                        sibling->lc->color = parent->color;
                        parent->color = Color::BLACK;
                        rotate_right(sibling);
                        rotate_left(parent);
                    }
                }
            }

    }

    std::shared_ptr<Node> remove_in(std::shared_ptr<Node> &node, std::shared_ptr<Node> &p, T val) {
        if (!node || node == NIL) return nullptr;
        std::shared_ptr<Node> res = nullptr;

        if (cmp{}(node->data.value(), val)) {
            res =  remove_in(node->rc, node, val);
        }else if (cmp{}(val, node->data.value())){
            res =  remove_in(node->lc, node, val);
        }else {
            --node->count;
            if (node->count > 0) {
                node->update();
                return node;
            }else {
                if (node->rc == NIL && node->lc == NIL) {
                    if (node->color == Color::RED) {
                        node = NIL;
                        res = node;
                    }else {
                        fix_double_black(node);
                        node = NIL;
                        res = node;
                    }
                }else if (node->rc == NIL || node->lc == NIL) {
                    auto parent = node->get_parent();
                    node = std::move(node->rc == NIL ? node->lc : node->rc);
                    if (node && node != NIL) node->parent = parent;
                    node->color = Color::BLACK;
                    res = node;
                }else {
                    auto successor = node->rc;
                    while (successor->lc != NIL) successor = successor->lc;

                    node->data = successor->data;
                    node->count = successor->count;
                    successor->count = 1; // 强制设为1以便物理删除

                    res = remove_in(node->rc, node, node->data.value());
                }
            }
        }
        if (node && node != NIL) {
            node->update();
        }
        return res;
    }

    void In_traverse_in(void (*vis)(std::optional<T>&), std::shared_ptr<Node> n) {
        if (!n || n == NIL) return;
        In_traverse_in(vis, n->lc);
        vis(n->data);
        In_traverse_in(vis, n->rc);
    }

    bool search_in(std::shared_ptr<Node> &node, T val) {
        if (!node || node == NIL) return false;
        if (cmp{}(node->data, val)) return search_in(node->rc, val);
        if (cmp{}(val, node->data)) return search_in(node->lc, val);
        return true;
    }

    int get_rank_in(std::shared_ptr<Node> &n, const T& val) {
        if (!n || n == NIL) return 0;

        if (val == n->data) {
            return (n->lc ? n->lc->size : 0) + 1;
        } else if (cmp{}(val, n->data)) {
            return get_rank_in(n->lc, val);
        } else {
            return (n->lc ? n->lc->size : 0) + n->count + get_rank_in(n->rc, val);
        }
    }

    int get_size_in(std::shared_ptr<Node> &n) {
        return n == NIL ? 0 : n->size;
    }

    T kth_num_in(std::shared_ptr<Node> &n, const int k) {
        int left_size = ((n->lc && n->lc != NIL) ? n->lc->size : 0);
        if (left_size >= k) return kth_num_in(n->lc, k);
        if (left_size + n->count >= k) return n->data.value();
        else return kth_num_in(n->rc, k - left_size - n->count);
    }

public:
    void insert(T val) {
        auto new_node = insert_in(root, NIL, val);
        insert_fix(new_node);
        root->color = Color::BLACK;
    }
    bool remove(T val) {
        bool ret = remove_in(root, NIL, val) != nullptr;
        root->color = Color::BLACK;
        return ret;
    }
    RBT() {
        init_NIL();
        root = NIL;
    }
    RBT(std::vector<T> &arr) {
        init_NIL();
        root = NIL;
        for (auto i = arr.begin(); i != arr.end(); ++i) {
            insert(*i);
        }
    }

    static void defaule_vis(std::optional<T>& data) {
        std::cout<<data.value()<<" ";
    }

    void traverse(void (*vis)(std::optional<T>&) = defaule_vis) {
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
        if (!root || root == NIL) return 0;
        return get_size_in(root);
    }

    T find_kth_num(int k) {
        if (k < 1 || k > get_size()) throw std::runtime_error("index out of bound");
        else {
            return kth_num_in(root, k);
        }
    }
};

#endif //TREE_REDBLACKTREE_HPP
