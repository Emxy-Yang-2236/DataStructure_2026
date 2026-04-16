#ifndef TREE_SPLAYTREE_HPP
#define TREE_SPLAYTREE_HPP

//The balance of the tree is determined by the visiting behavior
#include <functional>
#include <iostream>
#include <memory>
#include <optional>

template<class T, class cmp = std::less<T>>
class SPT {
public:
    cmp comparator_;
private:
    struct Node : public std::enable_shared_from_this<Node>{
        std::optional<T> data;
        std::shared_ptr<Node> lc, rc;
        std::weak_ptr<Node> parent;
        int count{}, size{};

        std::shared_ptr<Node> get_parent() {
            return parent.lock();
        }
        std::shared_ptr<Node> get_grandparent() {
            return get_parent() == nullptr ? nullptr : get_parent()->get_parent();
        }


        Node(std::shared_ptr<Node> p, std::optional<T> d = std::nullopt)
            :data(d), parent(p), lc(nullptr), rc(nullptr), count(1), size(1) {}
        Node (Node && other)noexcept : data(std::move(other.data)), lc(std::move(other.lc)),rc(std::move(other.rc)),
            parent(std::move(other.parent)), count(other.count), size(other.size) {}

        [[nodiscard]] int getSize() const { return this->size; }
        void update() {
            int left_size = lc ? lc->size : 0;
            int right_size = rc ? rc->size : 0;
            this->size = this->count + left_size + right_size;
        }
    };

    std::shared_ptr<Node> root;

    void rotate_left(std::shared_ptr<Node> node) {
        auto child = node->rc;
        node->rc = child->lc;
        if (child->lc) child->lc->parent = node;

        auto parent = node->get_parent();
        child->parent = parent;
        if (!parent) {
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
        if (child->rc) child->rc->parent = node;

        auto parent = node->get_parent();
        child->parent = parent;
        if (!parent) {
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

    bool same_direction(const std::shared_ptr<Node>& node) const{
        auto p = node->get_parent();
        auto g = node->get_grandparent();

        if ((p->lc == node && g->lc == p) || (p->rc == node && g->rc == p)) {
            return true;
        }
        return false;
    }
    void splay(std::shared_ptr<Node> node) {
        while (auto p = node->get_parent()) {
            auto g = node->get_grandparent();
            if (!g) {                            // zig / zag
                if (p->lc == node) {
                    rotate_right(p);
                } else {
                    rotate_left(p);
                }
            } else if (same_direction(node)) {   // zig-zig / zag-zag
                if (p->lc == node) {
                    rotate_right(g);
                    rotate_right(p);
                } else {
                    rotate_left(g);
                    rotate_left(p);
                }
            } else {                        // zig-zag / zag-zig
                if (p->lc == node) {
                    rotate_right(p);
                    rotate_left(g);
                } else {
                    rotate_left(p);
                    rotate_right(g);
                }
            }
        }
    }

    std::shared_ptr<Node> insert_in(std::shared_ptr<Node>& node, std::shared_ptr<Node> p, T val) {
        if (!node) {
            node = std::make_shared<Node>(p,val);
            return node;
        }
        std::shared_ptr<Node> ret = nullptr;
        if (comparator_(node->data.value() , val))  ret = insert_in(node->rc, node, val);
        else if (comparator_(val, node->data.value())) ret = insert_in(node->lc, node, val);
        else {
            ++node->count;
            ret = node;
        }
        node->update();
        return ret;
    }

    std::shared_ptr<Node> remove_in(std::shared_ptr<Node>& node, std::shared_ptr<Node> p, T val) {
        std::shared_ptr<Node> ret = nullptr;

        if (!node) return p;
        else if (comparator_(node->data.value(), val)) {
            ret =  remove_in(node->rc, node, val);
        }else if (comparator_(val, node->data.value())){
            ret =  remove_in(node->lc,node,  val);
        }else {
            --node->count;
            if (node->count > 0) {
                ret = node;
            }else {
                if (node->rc == nullptr || node->lc == nullptr) {
                    node = std::move(node->rc ? node->rc : node->lc);
                    if (node) ret = node;
                    else ret = p;
                }else {
                    auto successor = node->rc;
                    while (successor->lc) successor = successor->lc;

                    node->data = successor->data;
                    node->count = successor->count;
                    successor->count = 1; // 强制设为1以便物理删除

                    ret = remove_in(node->rc, node, node->data.value());
                }
            }
        }

        if (node) {
            node->update();
        }
        return ret;
    }

    std::pair<std::shared_ptr<Node>, bool> search_in(std::shared_ptr<Node>& node, std::shared_ptr<Node> p,  T val) {
        if (!node) return {p, false};
        if (comparator_(node->data.value(), val)) return search_in(node->rc ,node, val);
        if (comparator_(val, node->data.value())) return search_in(node->lc, node, val);
        return {node, true};
    }

public:
    void insert(T val) {
        auto tar = insert_in(root, nullptr, val);
        splay(tar);
    }
    void remove(T val) {
        auto tar = remove_in(root, nullptr, val);
        splay(tar);
    }

    SPT(std::vector<T> &arr) {
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            this->insert_in(root, nullptr, *it);
        }
    }

    bool search(T val) {
        auto tar =  search_in(root,nullptr, val);
        splay(tar.first);
        return tar.second;
    }

    T get_root() {
        if (!root) {
            throw std::runtime_error("root is nullptr");
        }
        return root->data.value();
    }

};

#endif //TREE_SPLAYTREE_HPP
