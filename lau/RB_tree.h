// Lau CPP Library - to strengthen the ability of C++ STL
// Copyright (C) 2022 Lau Yee-Yu
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

/**
 * @file lau/RB_tree.h
 *
 * This is a external header file, including a red black tree class.
 */

#ifndef LAU_CPP_LIB_LAU_RB_TREE_H
#define LAU_CPP_LIB_LAU_RB_TREE_H

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include "exception.h"
#include "type_traits.h"
#include "utility.h"

namespace lau {

template<class T,
         class Compare = std::less<T>,
         class Allocator = std::allocator<T>>
class RBTree {
    friend class Iterator;
    friend class ConstIterator;

public:
    struct Node;
    class  Iterator;
    class  ConstIterator;

    enum Flag {red, black};

    using AllocatorType = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;

    struct Node {
        friend RBTree;

    public:
        Node() = default;

        explicit Node(const T& valueIn) : value(valueIn) {}
        explicit Node(T&& valueIn) : value(valueIn) {}

        Node(Node& obj) = default;
        Node(const Node& obj) = default;
        Node(Node&& obj) = default;

        template<class... Args>
        explicit Node(Args&&... args) : value(std::forward<Args>(args)...) {}

        Node& operator=(Node& obj) = default;
        Node& operator=(const Node& obj) = default;
        Node& operator=(Node&& obj) = default;

        ~Node() = default;

        Flag Colour() const noexcept { return colour; }

        Node* Parent() const noexcept { return parent; }
        Node* Left()   const noexcept { return left; }
        Node* Right()  const noexcept { return right; }

        T value;

    private:
        /**
         * Check whether the node is left node.  The node MUST have its parent
         * node.
         */
        bool IsLeftNode() noexcept { return (this->parent->left == this); }

        /**
         * Check whether the node is right node.  The node MUST have its parent
         * node.
         */
        bool IsRightNode() noexcept { return (this->parent->right == this); }

        /**
         * Get the brother node.  The node MUST have its parent node.
         * @param node
         * @return the pointer to the brother node
         */
        Node* Brother() noexcept {
            return (IsLeftNode()) ? this->parent->right : this->parent->left;
        }

        /**
         * Get the uncle node.  The node MUST have its grandparent node.
         * @param node
         * @return the pointer to the uncle node
         */
        Node* Uncle() noexcept { return this->parent->Brother(); }

        Flag  colour = red;
        Node* parent = nullptr;
        Node* left   = nullptr;
        Node* right  = nullptr;
    };

    class Iterator {
        friend RBTree;
        friend ConstIterator;

    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // For instance, for an iterator, iterator::value_type is the type that the
        // iterator points to.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using value_type        = Node;
        using pointer           = Node*;
        using reference         = Node&;
        using iterator_category = std::output_iterator_tag;

        Iterator() noexcept = default;
        Iterator(const Iterator& obj) noexcept = default;

        Iterator& operator=(const Iterator& obj) noexcept = default;

        ~Iterator() = default;

        operator Node*() const noexcept { return target_; }

        /**
         * Get the iterator next it.  For the last iterator, it will become
         * the end iterator.  Throw <code>lau::InvalidIterator</code> if it
         * is the end iterator.
         */
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        /**
         * Move the iterator to the next one.  For the last iterator, it will
         * become the end iterator.  Throw <code>lau::InvalidIterator</code>
         * if it is the end iterator.
         */
        Iterator& operator++() {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: using ++ on the end iterator");
            }
            if (target_->right == nullptr) {
                while (target_->parent != nullptr && target_->IsRightNode()) {
                    target_ = target_->parent;
                }
                target_ = target_->parent;
            } else {
                target_ = target_->right;
                while (target_->left != nullptr) target_ = target_->left;
            }
            return *this;
        }

        /**
         * Get the iterator that is the last one of it.  Throw
         * <code>lau::InvalidIterator</code> if it is the begin iterator.
         */
        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        /**
         * Move the iterator to the last one.  Throw
         * <code>lau::InvalidIterator</code> if it is the begin iterator.
         */
        Iterator& operator--() {
            if (target_ == nullptr) {
                if (tree_->size_ == 0) {
                    throw InvalidIterator("Invalid Iterator: using -- on an empty tree");
                }
                target_ = tree_->head_;
                while (target_->right != nullptr) target_ = target_->right;
                return *this;
            }

            if (target_->left == nullptr) {
                while (target_->parent != nullptr && target_->IsLeftNode()) {
                    target_ = target_->parent;
                }
                target_ = target_->parent;
            } else {
                target_ = target_->left;
                while (target_->right != nullptr) target_ = target_->right;
            }
            return *this;
        }

        bool operator==(const Iterator& rhs) const noexcept {
            return (this->tree_ == rhs.tree_ && this->target_ == rhs.target_);
        }

        bool operator==(const ConstIterator& rhs) const noexcept {
            return (this->tree_ == rhs.tree_ && this->target_ == rhs.target_);
        }

        bool operator!=(const Iterator& rhs) const noexcept {
            return (this->tree_ != rhs.tree_ || this->target_ != rhs.target_);
        }

        bool operator!=(const ConstIterator& rhs) const noexcept {
            return (this->tree_ != rhs.tree_ || this->target_ != rhs.target_);
        }

        Node& operator*() const {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: de-referencing the end iterator");
            }
            return *target_;
        }

        Node* operator->() const {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: de-referencing the end iterator");
            }
            return target_;
        }

    private:
        Iterator(Node* target, const RBTree* tree) : target_(target), tree_(tree) {}

        Node* target_       = nullptr;
        const RBTree* tree_ = nullptr;
    };

    class ConstIterator {
        friend RBTree;
        friend Iterator;

    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // For instance, for an iterator, iterator::value_type is the type that the
        // iterator points to.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
        using iterator_category = std::output_iterator_tag;

        ConstIterator() noexcept = default;
        ConstIterator(const ConstIterator& obj) noexcept = default;

        ConstIterator(const Iterator& iterator) noexcept
            : target_(iterator.target_),
              tree_(iterator.tree_) {}

        ConstIterator& operator=(const ConstIterator& obj) noexcept = default;

        ~ConstIterator()  = default;

        operator const Node*() const noexcept { return target_; }

        /**
         * Get the iterator next it.  For the last iterator, it will become
         * the end iterator.  Throw <code>lau::InvalidIterator</code> if it
         * is the end iterator.
         */
        ConstIterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        /**
         * Move the iterator to the next one.  For the last iterator, it will
         * become the end iterator.  Throw <code>lau::InvalidIterator</code>
         * if it is the end iterator.
         */
        ConstIterator& operator++() {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: using ++ on the end iterator");
            }
            if (target_->right == nullptr) {
                while (target_->parent != nullptr && target_->IsRightNode()) {
                    target_ = target_->parent;
                }
                target_ = target_->parent;
            } else {
                target_ = target_->right;
                while (target_->left != nullptr) target_ = target_->left;
            }
            return *this;
        }

        /**
         * Get the iterator that is the last one of it.  Throw
         * <code>lau::InvalidIterator</code> if it is the begin iterator.
         */
        ConstIterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        /**
         * Move the iterator to the last one.  Throw
         * <code>lau::InvalidIterator</code> if it is the begin iterator.
         */
        ConstIterator& operator--() {
            if (target_ == nullptr) {
                if (tree_->size_ == 0) {
                    throw InvalidIterator("Invalid Iterator: using -- on an empty tree");
                }
                target_ = tree_->head_;
                while (target_->right != nullptr) target_ = target_->right;
                return *this;
            }

            if (target_->left == nullptr) {
                while (target_->parent != nullptr && target_->IsLeftNode()) {
                    target_ = target_->parent;
                }
                target_ = target_->parent;
            } else {
                target_ = target_->left;
                while (target_->right != nullptr) target_ = target_->right;
            }
            return *this;
        }

        bool operator==(const Iterator& rhs) const noexcept {
            return (this->tree_ == rhs.tree_ && this->target_ == rhs.target_);
        }

        bool operator==(const ConstIterator& rhs) const noexcept {
            return (this->tree_ == rhs.tree_ && this->target_ == rhs.target_);
        }

        bool operator!=(const Iterator& rhs) const noexcept {
            return (this->tree_ != rhs.tree_ || this->target_ != rhs.target_);
        }

        bool operator!=(const ConstIterator& rhs) const noexcept {
            return (this->tree_ != rhs.tree_ || this->target_ != rhs.target_);
        }

        const Node& operator*() const {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: de-referencing the end iterator");
            }
            return *target_;
        }

        const Node* operator->() const {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: de-referencing the end iterator");
            }
            return target_;
        }

    private:
        ConstIterator(Node* target, const RBTree* tree) : target_(target), tree_(tree) {}

        Node* target_       = nullptr;
        const RBTree* tree_ = nullptr;
    };

    RBTree() : head_(nullptr),
               first_(nullptr),
               compare_(),
               size_(0),
               allocator_() {}

    explicit RBTree(const Compare& compare) : head_(nullptr),
                                              first_(nullptr),
                                              compare_(compare),
                                              size_(0),
                                              allocator_() {}

    explicit RBTree(const Allocator& allocator) : head_(nullptr),
                                                  first_(nullptr),
                                                  compare_(),
                                                  size_(0),
                                                  allocator_(allocator) {}

    RBTree(const Compare& compare,
           const Allocator& allocator) : head_(nullptr),
                                         first_(nullptr),
                                         compare_(compare),
                                         size_(0),
                                         allocator_(allocator) {}

    template<class InputIterator>
    RBTree(InputIterator begin, InputIterator end) : head_(nullptr),
                                                     first_(nullptr),
                                                     compare_(),
                                                     size_(0),
                                                     allocator_() {
        while (begin != end) {
            this->Insert(*begin);
            ++begin;
        }
    }

    template<class InputIterator>
    RBTree(InputIterator begin, InputIterator end,
           const Compare& compare) : head_(nullptr),
                                     first_(nullptr),
                                     compare_(compare),
                                     size_(0),
                                     allocator_() {
        while (begin != end) {
            this->Insert(*begin);
            ++begin;
        }
    }

    template<class InputIterator>
    RBTree(InputIterator begin, InputIterator end,
           const Allocator& allocator) : head_(nullptr),
                                         first_(nullptr),
                                         compare_(),
                                         size_(0),
                                         allocator_(allocator) {
        while (begin != end) {
            this->Insert(*begin);
            ++begin;
        }
    }

    template<class InputIterator>
    RBTree(InputIterator begin, InputIterator end,
           const Compare& compare,
           const Allocator& allocator) : head_(nullptr),
                                         first_(nullptr),
                                         compare_(compare),
                                         size_(0),
                                         allocator_(allocator) {
        while (begin != end) {
            this->Insert(*begin);
            ++begin;
        }
    }

    RBTree(const RBTree& obj) : size_(obj.size_),
                                compare_(obj.compare_),
                                allocator_(obj.allocator_) {
        head_ = CopyChildTree_(obj.head_);
        first_ = head_;
        if (head_ == nullptr) return;
        while (first_->left != nullptr) first_ = first_->left;
    }

    RBTree(const RBTree& obj, const Allocator& allocator)
        : size_(obj.size_),
          compare_(obj.compare_),
          allocator_(allocator_) {
        head_ = CopyChildTree_(obj.head_);
        first_ = head_;
        if (head_ == nullptr) return;
        while (first_->left != nullptr) first_ = first_->left;
    }

    RBTree(RBTree&& obj) noexcept : head_(obj.head_),
                                    compare_(std::move(obj.compare_)),
                                    first_(obj.first_),
                                    size_(obj.size_),
                                    allocator_(std::move(obj.allocator_)) {
        obj.head_ = nullptr;
        obj.first_ = nullptr;
        obj.size_ = 0;
    }

    RBTree(std::initializer_list<T> init) : head_(nullptr),
                                            first_(nullptr),
                                            compare_(),
                                            size_(0),
                                            allocator_() {
        for (const auto& element : init) {
            this->Insert(element);
        }
    }

    RBTree(std::initializer_list<T> init,
           const Compare& compare) : head_(nullptr),
                                     first_(nullptr),
                                     compare_(compare),
                                     size_(0),
                                     allocator_() {
        for (const auto& element : init) {
            this->Insert(element);
        }
    }

    RBTree(std::initializer_list<T> init,
           const Allocator& allocator) : head_(nullptr),
                                         first_(nullptr),
                                         compare_(),
                                         size_(0),
                                         allocator_(allocator) {
        for (const auto& element : init) {
            this->Insert(element);
        }
    }

    RBTree(std::initializer_list<T> init,
           const Compare& compare,
           const Allocator& allocator) : head_(nullptr),
                                         first_(nullptr),
                                         compare_(compare),
                                         size_(0),
                                         allocator_(allocator) {
        for (const auto& element : init) {
            this->Insert(element);
        }
    }


    RBTree& operator=(const RBTree& obj) {
        if (&obj == this) return *this;
        DeleteChildNode_(head_);
        head_ = CopyChildTree_(obj.head_);
        compare_ = obj.compare_;
        size_ = obj.size_;
        first_ = head_;
        if (head_ == nullptr) return *this;
        while (first_->left != nullptr) first_ = first_->left;
        return *this;
    }

    RBTree& operator=(RBTree&& obj) noexcept {
        DeleteChildNode_(head_);
        size_ = obj.size_;
        head_ = obj.head_;
        first_ = obj.first_;
        compare_ = std::move(obj.compare_);
        obj.head_ = nullptr;
        obj.first_ = nullptr;
        obj.size_ = 0;
        return *this;
    }

    ~RBTree() { DeleteChildNode_(head_); }

#ifdef LAU_TEST
    /**
     * Print the detail of the tree. The data is used for Graphviz. See
     * <a href="https://www.graphviz.org/">Graphviz Website</a> for more
     * detail.
     */
    void Print() {
        std::cout << "digraph {" << std::endl << "splines=false;" << std::endl
        << "node [style=filled,color=lightblue;];" << std::endl;
        PrintDetail_(head_);
        std::cout << "}" << std::endl;
    }
#endif

    /**
     * Insert a value into the tree.
     * @param value
     * @return a pair of Iterator and a bool.
     * <br><br>
     * If such node doesn't exist, the first one is the iterator of the new
     * node, and the second one is true;
     * <br>
     * if such node exists, the first one is the iterator of the node that
     * prevents this insertion, and the second one is false.
     */
    Pair<Iterator, bool> Insert(const T& value) {
        if (head_ == nullptr) {
            Node* newNode = allocator_.allocate(1);
            try {
                ::new(newNode) Node(value);
            } catch (...) {
                allocator_.deallocate(newNode, 1);
                throw;
            }
            newNode->colour = black;
            head_ = newNode;
            first_ = newNode;
            ++size_;
            return Pair<Iterator, bool>(Iterator(newNode, this), true);
        }

        // Find the place to insert
        Node* place = head_;
        Node* uncle;
        bool direction = true; // true for left and right for right
        bool min = true;
        Node* tmp;
        if (compare_(value, place->value)) {
            tmp = place->left;
        } else {
            min = false;
            tmp = place->right;
            direction = false;
        }

        if (!direction && !compare_(place->value, value)) { // the case of same value
            return Pair<Iterator, bool>(Iterator(place, this), false);
        }

        while (tmp != nullptr) {
            place = direction ? place->left : place->right;
            if (compare_(value, place->value)) {
                tmp = place->left;
                direction = true;
            } else {
                min = false;
                tmp = place->right;
                direction = false;
            }
            if (!direction && !compare_(place->value, value)) {
                return Pair<Iterator, bool>(Iterator(place, this), false);
            }
        }

        Node* newNode = allocator_.allocate(1);
        try {
            ::new(newNode) Node(value);
        } catch (...) {
            allocator_.deallocate(newNode, 1);
            throw;
        }
        ++size_;
        if (min) first_ = newNode;
        newNode->parent = place;
        if (direction) place->left = newNode;
        else place->right = newNode;

        if (place->colour == black) return Pair<Iterator, bool>(Iterator(newNode, this), true);

        place = newNode;

        while (place->parent != nullptr && place->parent->colour == red) {
            // Must have its grandparent, because the parent is red (red node
            // cannot be the head node.)
            uncle = place->Uncle();
            if (uncle == nullptr || uncle->colour == black) {
                if (place->IsLeftNode()) {
                    if (place->parent->IsLeftNode()) LLRotate_(place);
                    else RLRotate_(place);
                } else {
                    if (place->parent->IsLeftNode()) LRRotate_(place);
                    else RRRotate_(place);
                }
                break;
            } else {
                place = place->parent;
                place->colour = black;
                uncle->colour = black;
                place = place->parent;
                place->colour = red;
            }
        }
        head_->colour = black;
        return Pair<Iterator, bool>(Iterator(newNode, this), true);
    }

    /**
     * Add a value into the tree in place.  Please note that the node will be
     * constructed in the very beginning of this function.
     * @param args
     * @return a pair of Iterator and a bool.
     * <br><br>
     * If such node doesn't exist, the first one is the iterator of the new
     * node, and the second one is true;
     * <br>
     * if such node exists, the first one is the iterator of the node that
     * prevents this insertion, and the second one is false.
     */
    template<class... Args>
    Pair<Iterator, bool> Emplace(Args&&... args) {
        Node* newNode = allocator_.allocate(1);
        try {
            ::new(newNode) Node(std::forward<Args>(args)...);
        } catch (...) {
            allocator_.deallocate(newNode, 1);
            throw;
        }

        if (head_ == nullptr) {
            newNode->colour = black;
            head_ = newNode;
            first_ = newNode;
            ++size_;
            return Pair<Iterator, bool>(Iterator(newNode, this), true);
        }

        // Find the place to insert
        Node* place = head_;
        Node* uncle;
        bool direction = true; // true for left and right for right
        bool min = true;
        Node* tmp;
        if (compare_(newNode->value, place->value)) {
            tmp = place->left;
        } else {
            min = false;
            tmp = place->right;
            direction = false;
        }

        if (!direction && !compare_(place->value, newNode->value)) { // the case of same value
            newNode->~Node();
            allocator_.deallocate(newNode, 1);
            return Pair<Iterator, bool>(Iterator(place, this), false);
        }

        while (tmp != nullptr) {
            place = direction ? place->left : place->right;
            if (compare_(newNode->value, place->value)) {
                tmp = place->left;
                direction = true;
            } else {
                min = false;
                tmp = place->right;
                direction = false;
            }
            if (!direction && !compare_(place->value, newNode->value)) {
                newNode->~Node();
                allocator_.deallocate(newNode, 1);
                return Pair<Iterator, bool>(Iterator(place, this), false);
            }
        }

        ++size_;
        if (min) first_ = newNode;
        newNode->parent = place;
        if (direction) place->left = newNode;
        else place->right = newNode;

        if (place->colour == black) return Pair<Iterator, bool>(Iterator(newNode, this), true);

        place = newNode;

        while (place->parent != nullptr && place->parent->colour == red) {
            // Must have its grandparent, because the parent is red (red node
            // cannot be the head node.)
            uncle = place->Uncle();
            if (uncle == nullptr || uncle->colour == black) {
                if (place->IsLeftNode()) {
                    if (place->parent->IsLeftNode()) LLRotate_(place);
                    else RLRotate_(place);
                } else {
                    if (place->parent->IsLeftNode()) LRRotate_(place);
                    else RRRotate_(place);
                }
                break;
            } else {
                place = place->parent;
                place->colour = black;
                uncle->colour = black;
                place = place->parent;
                place->colour = red;
            }
        }
        head_->colour = black;
        return Pair<Iterator, bool>(Iterator(newNode, this), true);
    }

    /**
     * Insert a value into the tree.
     * @param value
     * @return a pair of Iterator and a bool.
     * <br><br>
     * If such node doesn't exist, the first one is the iterator of the new
     * node, and the second one is true;
     * <br>
     * if such node exists, the first one is the iterator of the node that
     * prevents this insertion, and the second one is false.
     */
    Pair<Iterator, bool> Insert(T&& value) {
        if (head_ == nullptr) {
            Node* newNode = allocator_.allocate(1);
            try {
                ::new(newNode) Node(value);
            } catch (...) {
                allocator_.deallocate(newNode, 1);
                throw;
            }
            newNode->colour = black;
            head_ = newNode;
            first_ = newNode;
            ++size_;
            return Pair<Iterator, bool>(Iterator(newNode, this), true);
        }

        // Find the place to insert
        Node* place = head_;
        Node* uncle;
        bool direction = true; // true for left and right for right
        bool min = true;
        Node* tmp;
        if (compare_(value, place->value)) {
            tmp = place->left;
        } else {
            min = false;
            tmp = place->right;
            direction = false;
        }

        if (!direction && !compare_(place->value, value)) { // the case of same value
            return Pair<Iterator, bool>(Iterator(place, this), false);
        }

        while (tmp != nullptr) {
            place = direction ? place->left : place->right;
            if (compare_(value, place->value)) {
                tmp = place->left;
                direction = true;
            } else {
                min = false;
                tmp = place->right;
                direction = false;
            }
            if (!direction && !compare_(place->value, value)) {
                return Pair<Iterator, bool>(Iterator(place, this), false);
            }
        }

        Node* newNode = allocator_.allocate(1);
        try {
            ::new(newNode) Node(value);
        } catch (...) {
            allocator_.deallocate(newNode, 1);
            throw;
        }
        ++size_;
        if (min) first_ = newNode;
        newNode->parent = place;
        if (direction) place->left = newNode;
        else place->right = newNode;

        if (place->colour == black) return Pair<Iterator, bool>(Iterator(newNode, this), true);

        place = newNode;

        while (place->parent != nullptr && place->parent->colour == red) {
            // Must have its grandparent, because the parent is red (red node
            // cannot be the head node.)
            uncle = place->Uncle();
            if (uncle == nullptr || uncle->colour == black) {
                if (place->IsLeftNode()) {
                    if (place->parent->IsLeftNode()) LLRotate_(place);
                    else RLRotate_(place);
                } else {
                    if (place->parent->IsLeftNode()) LRRotate_(place);
                    else RRRotate_(place);
                }
                break;
            } else {
                place = place->parent;
                place->colour = black;
                uncle->colour = black;
                place = place->parent;
                place->colour = red;
            }
        }
        head_->colour = black;
        return Pair<Iterator, bool>(Iterator(newNode, this), true);
    }

    /**
     * Erase the node at the position.  Throw <code>lau::InvalidIterator</code>
     * if the iterator doesn't belong to this tree or the iterator is the end
     * iterator.
     * @param position the iterator whose target is to be deleted
     * @return the reference to this class
     */
    RBTree& Erase(const Iterator& position) {
        if (position.tree_ != this) {
            throw InvalidIterator("Invalid Iterator: the iterator doesn't belong to this class");
        }
        if (position.target_ == nullptr) {
            throw InvalidIterator("Invalid Iterator: cannot erase the end iterator");
        }
        Erase_(position.target_);
        return *this;
    }

    /**
     * Erase the node with the same value.
     * @param value
     * @return the reference to this class
     */
    RBTree& Erase(const T& value) {
        Node* place = Find_(value);
        if (place == nullptr) {
            throw InvalidArgument("Invalid Argument: the class have no such value");
        }
        Erase_(place);
        return *this;
    }

    /**
     * Erase the node with the same value.
     * @param value
     * @return the reference to this class
     */
    template<class K>
    RBTree& Erase(const K& value) {
        Node* place = Find_(value);
        if (place == nullptr) {
            throw InvalidArgument("Invalid Argument: the class have no such value");
        }
        Erase_(place);
        return *this;
    }

    /**
     * Clear the class.
     * @return the reference to this class
     */
    RBTree& Clear() noexcept {
        DeleteChildNode_(head_);
        first_ = nullptr;
        head_ = nullptr;
        size_ = 0;
        return *this;
    }

    [[nodiscard]] SizeT Size()  const noexcept { return size_; }
    [[nodiscard]] bool  Empty() const noexcept { return size_ == 0; }

    /**
     * Get the maximum size of the class.
     * @return the maximum size of the class
     */
    [[nodiscard]] long MaxSize() const noexcept {
        return std::allocator_traits<Allocator>::max_size(allocator_);
    }

    /**
     * Get a copy of the allocator.
     * @return a copy of the allocator
     */
    [[nodiscard]] AllocatorType GetAllocator() const noexcept { return allocator_; }

    /**
     * Get a copy of the compare class.
     * @return a copy of the compare class
     */
    [[nodiscard]] Compare GetCompare() const { return compare_; }

    /**
     * Check whether a node exist or not.
     * @param value the value for searching
     * @return a boolean of whether the value exist or not
     */
    [[nodiscard]] bool Contains(const T& value) const { return Find_(value) != nullptr; }

    /**
     * Check whether a node exist or not.
     * @param value the value for searching
     * @return a boolean of whether the value exist or not
     */
    template<class K>
    [[nodiscard]] bool Contains(const K& value) const { return Find_(value) != nullptr; }

    /**
     * Find the place of the value.
     * @param value the value for searching
     * @return the iterator of the corresponding node;
     * <br>
     * if the there doesn't exist such node, an end pointer will be returned.
     */
    [[nodiscard]] Iterator Find(const T& value) { return Iterator(Find_(value), this); }

    /**
     * Find the place of the value.  Please note that the type K must have
     * valid <code>Compare::is_transparent</code>.
     * @param value the value for searching
     * @return the iterator of the corresponding node;
     * <br>
     * if the there doesn't exist such node, an end pointer will be returned.
     */
    template<class K>
    [[nodiscard]] Iterator Find(const K& value) { return Iterator(Find_(value), this); }


    /**
     * Find the place of the value.
     * @param value the value for searching
     * @return the const iterator of the corresponding node;
     * <br>
     * if the there doesn't exist such node, an const end pointer will be
     * returned.
     */
    [[nodiscard]] ConstIterator Find(const T& value) const { return ConstIterator(Find_(value), this); }

    /**
     * Find the place of the value.  Please note that the type K must have
     * valid <code>Compare::is_transparent</code>.
     * @param value the value for searching
     * @return the const iterator of the corresponding node;
     * <br>
     * if the there doesn't exist such node, an const end pointer will be
     * returned.
     */
    template<class K>
    [[nodiscard]] ConstIterator Find(const K& value) const { return ConstIterator(Find_(value), this); }

    /**
     * Find the first node that is no less that the value.  If there are no
     * nodes that is no less than the value, the end iterator will be returned.
     * @param value
     * @return the iterator of the first node that is no less that the value
     * <br>
     * If there are no nodes that is no less than the value, the end iterator
     * will be returned.
     */
    [[nodiscard]] Iterator LowerBound(const T& value) {
        Node* place = head_;
        Node* lowerBound = nullptr;

        while (place != nullptr) {
            if (compare_(place->value, value)) {
                place = place->right;
            } else {
                lowerBound = place;
                place = place->left;
            }
        }
        return Iterator(lowerBound, this);
    }

    /**
     * Find the first node that is no less that the value.  If there are no
     * nodes that is no less than the value, the const end iterator will be
     * returned.
     * @param value
     * @return the const iterator of the first node that is no less that the
     * value
     * <br>
     * If there are no nodes that is no less than the value, an const end
     * iterator will be returned.
     */
    [[nodiscard]] ConstIterator LowerBound(const T& value) const {
        Node* place = head_;
        Node* lowerBound = nullptr;

        while (place != nullptr) {
            if (compare_(place->value, value)) {
                place = place->right;
            } else {
                lowerBound = place;
                place = place->left;
            }
        }
        return ConstIterator(lowerBound, this);
    }

    /**
     * Find the first node that is no less that the value.  If there are no
     * nodes that is no less than the value, the end iterator will be
     * returned.  Please note that the type K must have valid
     * <code>Compare::is_transparent</code>.
     * @param value
     * @return the iterator of the first node that is no less that the value
     * <br>
     * If there are no nodes that is no less than the value, the end iterator
     * will be returned.
     */
    template<class K>
    [[nodiscard]] Iterator LowerBound(const K& value) {
        Node* place = head_;
        Node* lowerBound = nullptr;

        while (place != nullptr) {
            if (compare_(place->value, value)) {
                place = place->right;
            } else {
                lowerBound = place;
                place = place->left;
            }
        }
        return Iterator(lowerBound, this);
    }

    /**
     * Find the first node that is no less that the value.  If there are no
     * nodes that is no less than the value, the const end iterator will be
     * returned.  Please note that the type K must have valid
     * <code>Compare::is_transparent</code>.
     * @param value
     * @return the const iterator of the first node that is no less that the
     * value
     * <br>
     * If there are no nodes that is no less than the value, an const end
     * iterator will be returned.
     */
    template<class K>
    [[nodiscard]] ConstIterator LowerBound(const K& value) const {
        Node* place = head_;
        Node* lowerBound = nullptr;

        while (place != nullptr) {
            if (compare_(place->value, value)) {
                place = place->right;
            } else {
                lowerBound = place;
                place = place->left;
            }
        }
        return ConstIterator(lowerBound, this);
    }

    /**
     * Find the first node that is greater that the value.  If there are no
     * nodes that is greater than the value, the end iterator will be returned.
     * @param value
     * @return the iterator of the first node that is greater that the value
     * <br>
     * If there are no nodes that is greater than the value, the end iterator
     * will be returned.
     */
    [[nodiscard]] Iterator UpperBound(const T& value) {
        Node* place = head_;
        Node* upperBound = nullptr;

        while (place != nullptr) {
            if (compare_(value, place->value)) {
                upperBound = place;
                place = place->left;
            } else {
                place = place->right;
            }
        }
        return Iterator(upperBound, this);
    }

    /**
     * Find the first node that is greater that the value.  If there are no
     * nodes that is greater than the value, the const end iterator will be
     * returned.
     * @param value
     * @return the const iterator of the first node that is greater that the
     * value
     * <br>
     * If there are no nodes that is greater than the value, an const end
     * iterator will be returned.
     */
    [[nodiscard]] ConstIterator UpperBound(const T& value) const {
        Node* place = head_;
        Node* upperBound = nullptr;

        while (place != nullptr) {
            if (compare_(value, place->value)) {
                upperBound = place;
                place = place->left;
            } else {
                place = place->right;
            }
        }
        return ConstIterator(upperBound, this);
    }

    /**
     * Find the first node that is greater that the value.  If there are no
     * nodes that is greater than the value, the end iterator will be
     * returned.  Please note that the type K must have valid
     * <code>Compare::is_transparent</code>.
     * @param value
     * @return the iterator of the first node that is greater that the value
     * <br>
     * If there are no nodes that is greater than the value, the end iterator
     * will be returned.
     */
    template<class K>
    [[nodiscard]] Iterator UpperBound(const K& value) {
        Node* place = head_;
        Node* upperBound = nullptr;

        while (place != nullptr) {
            if (compare_(value, place->value)) {
                upperBound = place;
                place = place->left;
            } else {
                place = place->right;
            }
        }
        return Iterator(upperBound, this);
    }

    /**
     * Find the first node that is greater that the value.  If there are no
     * nodes that is greater than the value, the const end iterator will be
     * returned.  Please note that the type K must have valid
     * <code>Compare::is_transparent</code>.
     * @param value
     * @return the const iterator of the first node that is greater that the
     * value
     * <br>
     * If there are no nodes that is greater than the value, an const end
     * iterator will be returned.
     */
    template<class K>
    [[nodiscard]] ConstIterator UpperBound(const K& value) const {
        Node* place = head_;
        Node* upperBound = nullptr;

        while (place != nullptr) {
            if (compare_(value, place->value)) {
                upperBound = place;
                place = place->left;
            } else {
                place = place->right;
            }
        }
        return ConstIterator(upperBound, this);
    }

    [[nodiscard]] Iterator Begin() noexcept { return Iterator(first_, this); }
    [[nodiscard]] ConstIterator Begin() const noexcept { return ConstIterator(first_, this); }

    [[nodiscard]] Iterator begin() { return Begin(); }
    [[nodiscard]] ConstIterator begin() const noexcept { return ConstIterator(first_, this); }

    [[nodiscard]] ConstIterator ConstBegin() const noexcept { return ConstIterator(first_, this); }

    [[nodiscard]] Iterator End() noexcept { return Iterator(nullptr, this); }
    [[nodiscard]] ConstIterator End() const noexcept { return ConstIterator(nullptr, this); }

    [[nodiscard]] Iterator end() { return End(); }
    [[nodiscard]] ConstIterator end() const noexcept { return ConstIterator(nullptr, this); }

    [[nodiscard]] ConstIterator ConstEnd() const { return ConstIterator(nullptr, this); }

    /**
     * Swap the contents of two red black tree.
     * @param other
     * @return the reference to this class
     */
    RBTree& Swap(RBTree& other) noexcept {
        Node* tmpNode = other.head_;
        other.head_ = this->head_;
        this->head_ = tmpNode;

        tmpNode = other.first_;
        other.first_ = this->first_;
        this->first_ = tmpNode;

        Compare tmpCompare = std::move(other.compare_);
        other.compare_ = std::move(this->compare_);
        this->compare_ = std::move(tmpCompare);

        SizeT tmpSize = other.size_;
        other.size_ = this->size_;
        this->size_ = tmpSize;

        AllocatorType tmpAllocator = std::move(other.allocator_);
        other.allocator_ = std::move(this->allocator_);
        this->allocator_ = std::move(tmpAllocator);
    }

private:
    /**
     * The rotation when the cause of rotation is the left node of a left node.
     * @param cause the cause of rotation whose uncle is black
     */
    void LLRotate_(Node* cause) noexcept {
        Node* parent = cause->parent;
        Node* grandparent = parent->parent;

        if (grandparent->parent != nullptr) {
            if (grandparent->IsLeftNode()) grandparent->parent->left = parent;
            else grandparent->parent->right = parent;
        } else {
            head_ = parent;
        }
        parent->parent = grandparent->parent;

        grandparent->left = parent->right;
        if (parent->right != nullptr) parent->right->parent = grandparent;

        parent->right = grandparent;
        grandparent->parent = parent;

        parent->colour = black;
        grandparent->colour = red;
    }

    /**
     * The rotation when the cause of rotation is the right node of a right
     * node.
     * @param cause the cause of rotation whose uncle is black
     */
    void RRRotate_(Node* cause) noexcept {
        Node* parent = cause->parent;
        Node* grandparent = parent->parent;

        if (grandparent->parent != nullptr) {
            if (grandparent->IsLeftNode()) grandparent->parent->left = parent;
            else grandparent->parent->right = parent;
        } else {
            head_ = parent;
        }
        parent->parent = grandparent->parent;

        grandparent->right = parent->left;
        if (parent->left != nullptr) parent->left->parent = grandparent;

        parent->left = grandparent;
        grandparent->parent = parent;

        parent->colour = black;
        grandparent->colour = red;
    }

    /**
     * The rotation when the cause of rotation is the right node of a left
     * node.
     * @param cause the cause of rotation whose uncle is black
     */
    void LRRotate_(Node* cause) noexcept {
        Node* parent = cause->parent;
        Node* grandparent = parent->parent;

        if (grandparent->parent != nullptr) {
            if (grandparent->IsLeftNode()) grandparent->parent->left = cause;
            else grandparent->parent->right = cause;
        } else {
            head_ = cause;
        }
        cause->parent = grandparent->parent;

        parent->right = cause->left;
        if (cause->left != nullptr) cause->left->parent = parent;

        grandparent->left = cause->right;
        if (cause->right != nullptr) cause->right->parent = grandparent;

        grandparent->parent = cause;
        parent->parent = cause;
        cause->right = grandparent;
        cause->left = parent;

        grandparent->colour = red;
        cause->colour = black;
    }

    /**
     * The rotation when the cause of rotation is the left node of a right
     * node.
     * @param cause the cause of rotation whose uncle is black
     */
    void RLRotate_(Node* cause) noexcept {
        Node* parent = cause->parent;
        Node* grandparent = parent->parent;

        if (grandparent->parent != nullptr) {
            if (grandparent->IsLeftNode()) grandparent->parent->left = cause;
            else grandparent->parent->right = cause;
        } else {
            head_ = cause;
        }
        cause->parent = grandparent->parent;

        parent->left = cause->right;
        if (cause->right != nullptr) cause->right->parent = parent;

        grandparent->right = cause->left;
        if (cause->left != nullptr) cause->left->parent = grandparent;


        grandparent->parent = cause;
        parent->parent = cause;
        cause->left = grandparent;
        cause->right = parent;

        grandparent->colour = red;
        cause->colour = black;
    }

    /**
     * Find the node that is equal to value.  Return nullptr if there doesn't
     * exist such node.
     * @param value
     * @return the pointer to the node; will be nullptr if there doesn't exist
     * such node.
     */
    Node* Find_(const T& value) const noexcept {
        Node* position = head_;
        while (position != nullptr) {
            if (compare_(value, position->value)) {
                position = position->left;
            } else if (compare_(position->value, value)) {
                position = position->right;
            } else {
                return position;
            }
        }
        return nullptr;
    }

    template<class K>
    Node* Find_(const K& value) const noexcept {
        Node* position = head_;
        while (position != nullptr) {
            if (compare_(value, position->value)) {
                position = position->left;
            } else if (compare_(position->value, value)) {
                position = position->right;
            } else {
                return position;
            }
        }
        return nullptr;
    }

    /**
     * Erase the node position is pointing.  The position must be valid.
     * @param position
     */
    void Erase_(Node* position) noexcept {
        --size_;

        // Handling the first pointer
        if (position == first_) {
            if (size_ == 0) {
                head_ = nullptr;
                first_ = nullptr;
                position->~Node();
                allocator_.deallocate(position, 1);
                return;
            }
            if (position->right != nullptr) first_ = first_->right;
            else first_ = first_->parent;
        }

        if (position->left == nullptr) {
            if (position->right == nullptr) {
                if (position->colour == red) {
                    // The parent must exist, since the node is a red node
                    if (position->IsLeftNode()) position->parent->left = nullptr;
                    else position->parent->right = nullptr;
                    position->~Node();
                    allocator_.deallocate(position, 1);
                    return;
                }

                // position->colour == black
                if (position->IsLeftNode()) {
                    position->parent->left = nullptr;
                    SolveBalancing_(position->parent, true);
                } else {
                    position->parent->right = nullptr;
                    SolveBalancing_(position->parent, false);
                }

                position->~Node();
                allocator_.deallocate(position, 1);
                return;
            }

            // right side is not empty. According to the rule, the right side
            // must be a single red node.
            if (position->parent == nullptr) {
                head_ = position->right;
            } else if (position->IsLeftNode()) {
                position->parent->left = position->right;
            } else {
                position->parent->right = position->right;
            }
            position->right->colour = black;
            position->right->parent = position->parent;

            position->~Node();
            allocator_.deallocate(position, 1);
            return;

        } else if (position->right == nullptr) {
            // Only left side is not empty.  According to the rule, the left
            // side must be a single red node.
            if (position->parent == nullptr) {
                head_ = position->left;
            } else if (position->IsLeftNode()) {
                position->parent->left = position->left;
            } else {
                position->parent->right = position->left;
            }
            position->left->colour = black;
            position->left->parent = position->parent;

            position->~Node();
            allocator_.deallocate(position, 1);
            return;

        } else {
            Node* alternative = position->left;
            // indicating that the alternative is the left node of the position
            bool firstLeft = (alternative->right == nullptr);
            while (alternative->right != nullptr) alternative = alternative->right;
            if (alternative->colour == red) { // no child at all
                alternative->colour = position->colour;

                if (!firstLeft) alternative->parent->right = nullptr;
                alternative->parent = position->parent;

                if (position->parent == nullptr) {
                    head_ = alternative;
                } else if (position->IsLeftNode()) {
                    position->parent->left = alternative;
                } else {
                    position->parent->right = alternative;
                }

                if (!firstLeft) {
                    alternative->left = position->left;
                    position->left->parent = alternative;
                }

                alternative->right = position->right;
                position->right->parent = alternative;

                position->~Node();
                allocator_.deallocate(position, 1);
                return;
            }

            // the node is black
            if (alternative->left != nullptr) {
                if (firstLeft) {
                    alternative->colour = position->colour;
                    alternative->left->colour = black;
                    position->right->parent = alternative;
                    alternative->right = position->right;

                    alternative->parent = position->parent;
                    if (position->parent == nullptr) {
                        head_ = alternative;
                    } else if (position->IsLeftNode()) {
                        position->parent->left = alternative;
                    } else {
                        position->parent->right = alternative;
                    }

                    position->~Node();
                    allocator_.deallocate(position, 1);
                    return;
                }

                Node* childOfAlternative = alternative->left; // must be red
                childOfAlternative->colour = black;
                childOfAlternative->parent = alternative->parent;
                alternative->parent->right = childOfAlternative;

                alternative->colour = position->colour;

                alternative->parent = position->parent;
                if (position->parent == nullptr) {
                    head_ = alternative;
                } else if (position->IsLeftNode()) {
                    position->parent->left = alternative;
                } else {
                    position->parent->right = alternative;
                }

                alternative->left = position->left;
                position->left->parent = alternative;

                alternative->right = position->right;
                position->right->parent = alternative;

                position->~Node();
                allocator_.deallocate(position, 1);
                return;
            }

            // For a black node without a child
            alternative->colour = position->colour;

            Node* parentOfAlternative = alternative->parent;

            if (!firstLeft) parentOfAlternative->right = nullptr;
            else parentOfAlternative = alternative;
            alternative->parent = position->parent;
            if (position->parent == nullptr) {
                head_ = alternative;
            } else if (position->IsLeftNode()) {
                position->parent->left = alternative;
            } else {
                position->parent->right = alternative;
            }

            alternative->right = position->right;
            position->right->parent = alternative;

            if (!firstLeft) {
                alternative->left = position->left;
                position->left->parent = alternative;
            }

            SolveBalancing_(parentOfAlternative, firstLeft);

            position->~Node();
            allocator_.deallocate(position, 1);
            return;
        }
    }

    /**
     * Solve the balancing problem of lacking a black caused by erasing
     * operation.
     * @param cause
     * @param direction true for lacking at left, false for lacking at right
     */
    void SolveBalancing_(Node* cause, bool direction) noexcept {
        Node* brother;
        while (cause != nullptr) {
            if (direction) {
                brother = cause->right;
                if (brother->colour == red) {
                    brother->colour = black;
                    cause->colour = red;
                    // no need to check, brother must have a black left child
                    brother->left->parent = cause;
                    cause->right = brother->left;

                    brother->parent = cause->parent;
                    if (cause->parent == nullptr) {
                        head_ = brother;
                    } else if (cause->IsLeftNode()) {
                        cause->parent->left = brother;
                    } else {
                        cause->parent->right = brother;
                    }

                    brother->left = cause;
                    cause->parent = brother;

                    brother = cause->right;
                }

                if (brother->left != nullptr && brother->left->colour == red) {
                    Node* leftChild = brother->left;

                    leftChild->colour = cause->colour;
                    cause->colour= black;

                    leftChild->parent = cause->parent;
                    if (cause->parent == nullptr) {
                        head_ = leftChild;
                    } else if (cause->IsLeftNode()) {
                        cause->parent->left = leftChild;
                    } else {
                        cause->parent->right = leftChild;
                    }

                    brother->left = leftChild->right;
                    if (leftChild->right != nullptr) leftChild->right->parent = brother;

                    cause->right = leftChild->left;
                    if (leftChild->left != nullptr) leftChild->left->parent = cause;

                    leftChild->right = brother;
                    leftChild->left = cause;

                    brother->parent = leftChild;
                    cause->parent = leftChild;
                    return;

                } else if (brother->right != nullptr && brother->right->colour == red) {
                    brother->right->colour = black;
                    brother->colour = cause->colour;
                    cause->colour = black;

                    brother->parent = cause->parent;
                    if (cause->parent == nullptr) {
                        head_ = brother;
                    } else if (cause->IsLeftNode()) {
                        cause->parent->left = brother;
                    } else {
                        cause->parent->right = brother;
                    }

                    cause->right = brother->left;
                    if (brother->left != nullptr) brother->left->parent = cause;

                    brother->left = cause;
                    cause->parent = brother;
                    return;

                } else {
                    brother->colour = red;
                    if (cause->colour == red) {
                        cause->colour = black;
                        return;
                    }
                    if (cause->parent == nullptr) {
                        return;
                    }
                    direction = cause->IsLeftNode();
                    cause = cause->parent;
                }

            } else {
                brother = cause->left;

                // Change the colour of brother node to be black
                if (brother->colour == red) {
                    brother->colour = black;
                    cause->colour = red;
                    // no need to check, brother must have a black right child
                    brother->right->parent = cause;
                    cause->left = brother->right;

                    brother->parent = cause->parent;
                    if (cause->parent == nullptr) {
                        head_ = brother;
                    } else if (cause->IsLeftNode()) {
                        cause->parent->left = brother;
                    } else {
                        cause->parent->right = brother;
                    }

                    brother->right = cause;
                    cause->parent = brother;

                    brother = cause->left;
                }

                if (brother->right != nullptr && brother->right->colour == red) {
                    Node* rightChild = brother->right;

                    rightChild->colour = cause->colour;
                    cause->colour= black;

                    rightChild->parent = cause->parent;
                    if (cause->parent == nullptr) {
                        head_ = rightChild;
                    } else if (cause->IsLeftNode()) {
                        cause->parent->left = rightChild;
                    } else {
                        cause->parent->right = rightChild;
                    }

                    brother->right = rightChild->left;
                    if (rightChild->left != nullptr) rightChild->left->parent = brother;

                    cause->left = rightChild->right;
                    if (rightChild->right != nullptr) rightChild->right->parent = cause;

                    rightChild->left = brother;
                    rightChild->right = cause;

                    brother->parent = rightChild;
                    cause->parent = rightChild;
                    return;

                } else if (brother->left != nullptr && brother->left->colour == red) {
                    brother->left->colour = black;
                    brother->colour = cause->colour;
                    cause->colour = black;

                    brother->parent = cause->parent;
                    if (cause->parent == nullptr) {
                        head_ = brother;
                    } else if (cause->IsLeftNode()) {
                        cause->parent->left = brother;
                    } else {
                        cause->parent->right = brother;
                    }

                    cause->left = brother->right;
                    if (brother->right != nullptr) brother->right->parent = cause;

                    brother->right = cause;
                    cause->parent = brother;
                    return;

                } else {
                    brother->colour = red;
                    if (cause->colour == red) {
                        cause->colour = black;
                        return;
                    }
                    if (cause->parent == nullptr) {
                        return;
                    }
                    direction = cause->IsLeftNode();
                    cause = cause->parent;
                }
            }
        }
    }

    /**
     * Delete all the child node(s) of the node.
     * @param node the head node of the tree
     */
    void DeleteChildNode_(Node* node) noexcept {
        if (node == nullptr) return;
        DeleteChildNode_(node->left);
        DeleteChildNode_(node->right);
        node->~Node();
        allocator_.deallocate(node, 1);
    }

    /**
     * Copy all the child node(s) of the node.
     * @param node the source head node
     * @return the pointer to the head of the copied tree
     */
    Node* CopyChildTree_(Node* node) {
        if (node == nullptr) return nullptr;
        Node* newNode = allocator_.allocate(1);
        try {
            ::new(newNode) Node(*node);
        } catch (...) {
            allocator_.deallocate(newNode, 1);
            throw;
        }

        try {
            newNode->left = CopyChildTree_(node->left);
            newNode->right = CopyChildTree_(node->right);
        } catch (...) {
            DeleteChildNode_(newNode);
            throw;
        }

        if (newNode->left != nullptr) newNode->left->parent = newNode;
        if (newNode->right != nullptr) newNode->right->parent = newNode;
        return newNode;
    }

    /**
     * Print all the detail of the node and its child node.
     * @param node the head node to be printed
     */
#ifdef LAU_TEST
    void PrintDetail_(Node* node) {
        std::cout << "node" << node << " [label=\"" << node->value << "\",color=";
        if (node->colour == red) std::cout << "red,fontcolor=white]" << std::endl;
        else std::cout << "black,fontcolor=white]" << std::endl;

        if (node->parent != nullptr) {
            std::cout << "node" << node << " -> node" << node->parent << std::endl;
        }
        if (node->left != nullptr) {
            std::cout << "node" << node << " -> node" << node->left << std::endl;
            P(node->left);
        }
        if (node->right != nullptr) {
            std::cout << "node" << node << " -> node" << node->right << std::endl;
            P(node->right);
        }
    }
#endif

    Node*         head_  = nullptr;
    Node*         first_ = nullptr;
    SizeT         size_  = 0;
    Compare       compare_;
    AllocatorType allocator_;
};

template <class T, class Compare, class Allocator>
void Swap(RBTree<T, Compare, Allocator>& lhs,
          RBTree<T, Compare, Allocator>& rhs) noexcept {
    lhs.Swap(rhs);
}

} // namespace lau

#endif // LAU_CPP_LIB_LAU_RB_TREE_H
