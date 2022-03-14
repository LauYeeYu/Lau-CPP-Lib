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
 * @file lau/priority_queue.h
 *
 * This is a external header file, including stuff about <code>lau::PriorityQueue</code>
 */

#ifndef LAU_CPP_LIB_LAU_PRIORITY_QUEUE_H
#define LAU_CPP_LIB_LAU_PRIORITY_QUEUE_H

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

#include "exception.h"
#include "type_trait.h"

namespace lau {

/**
 * A priority queue that uses the data structure leftist heap.
 * @tparam T the type of elements
 * @tparam Compare the compare of elements (the priority is the last to be
 * popped)
 * @tparam Allocator the allocator T
 */
template<class T, class Compare = std::less<T>, class Allocator = std::allocator<T>>
class PriorityQueue {
public:
    PriorityQueue() : PriorityQueue(Compare()) {}

    explicit PriorityQueue(const Compare& compare) : compare_(compare), nodeAllocator_() {}

    explicit PriorityQueue(const Allocator& allocator) : PriorityQueue(Compare(), allocator) {}

    PriorityQueue(const Compare& compare, const Allocator& allocator)
        : compare_(compare_), nodeAllocator_(allocator) {}

    PriorityQueue(const PriorityQueue& obj) : size_(obj.size_),
                                              compare_(obj.compare_),
                                              nodeAllocator_(obj.nodeAllocator_) {
        if (this == &obj) return;
        data_ = CopyNode_(obj.data_);
    }

    PriorityQueue(PriorityQueue&& obj) noexcept : compare_(obj.compare_),
                                                  data_(obj.data_),
                                                  size_(obj.size()),
                                                  nodeAllocator_(obj.nodeAllocator_) {
        obj.data_ = nullptr;
        obj.size_ = 0;
    }

    template<class Iterator>
    PriorityQueue(Iterator first, Iterator last,
                  const Compare& compare = Compare()) : compare_(compare), nodeAllocator_() {
        while (first != last) {
            this->Push(*first);
            ++first;
        }
    }

    template<class Iterator>
    PriorityQueue(Iterator first, Iterator last,
                  const Allocator& allocator) : compare_(), nodeAllocator_(allocator) {
        while (first != last) {
            this->Push(*first);
            ++first;
        }
    }

    template<class Iterator>
    PriorityQueue(Iterator first, Iterator last,
                  const Compare& compare,
                  const Allocator& allocator) : compare_(compare_), nodeAllocator_(allocator) {
        while (first != last) {
            this->Push(*first);
            ++first;
        }
    }

    ~PriorityQueue() {
        DeleteAllChildNode(data_);
    }

    PriorityQueue& operator=(const PriorityQueue& obj) {
        if (this == &obj) return *this;
        DeleteAllChildNode(data_);
        compare_ = obj.compare_;
        nodeAllocator_ = obj.nodeAllocator_;
        data_ = CopyNode_(obj.data_);
        size_ = obj.size_;
        return *this;
    }

    PriorityQueue& operator=(PriorityQueue&& obj) noexcept {
        DeleteAllChildNode(data_);
        compare_ = std::move(obj.compare_);
        nodeAllocator_ = std::move(obj.nodeAllocator_);
        data_ = obj.data_;
        size_ = obj.size_;
        obj.data_ = nullptr;
        obj.size_ = 0;
        return *this;
    }

    /**
     * Get the top of the queue.  Throw EmptyContainer if the queue is empty.
     * @return a reference of the top element.
     */
    const T& Top() const {
        if (size_ == 0) throw EmptyContainer("Empty Priority Queue");
        return data_->value;
    }

    /**
     * Push new element to the priority queue.
     * @param value
     * @return the reference to the current class
     */
    PriorityQueue& Push(const T& value) {
        Node_* tmp = nodeAllocator_.allocate(1);
        ::new(tmp) Node_(value, nullptr, nullptr, 0);
        data_ = MergeNode_(data_, tmp);
        ++size_;
        return *this;
    }

    /**
     * Delete the top element.  Throw EmptyContainer if the queue is empty.
     * @return the reference to the current class
     */
    PriorityQueue& Pop() {
        if (size_ == 0) throw EmptyContainer("Empty Priority Queue");
        Node_* top = data_;
        data_ = MergeNode_(data_->left, data_->right);
        top->~Node_();
        nodeAllocator_.deallocate(top, 1);
        --size_;
        return *this;
    }

    /**
     * Get the number of elements in this queue.
     * @return the number of the elements
     */
    [[nodiscard]] SizeT Size() const {
        return size_;
    }

    /**
     * Check if the container has at least an element.
     * @return true if it is empty, false if it has at least an element.
     */
    [[nodiscard]] bool Empty() const {
        return size_ == 0;
    }

    /**
     * Get a copy of the compare.
     * @return a copy of the compare
     */
    [[nodiscard]] Compare GetCompare() const {
        return compare_;
    }

    /**
     * Merge two PriorityQueues with complexity of O(log n).  Clear the other
     * priority queue.
     * @return the reference to the current class
     */
    PriorityQueue& Merge(PriorityQueue& other) {
        this->data_ = MergeNode_(this->data_, other.data_);
        other.data_ = nullptr;
        this->size_ += other.size_;
        other.size_ = 0;
        return *this;
    }

private:
    struct Node_ {
        T value;
        Node_* left = nullptr;
        Node_* right = nullptr;
        SizeT distance = 0;

        explicit Node_(const T& value,
                       Node_* left = nullptr,
                       Node_* right = nullptr,
                       SizeT distance = 0)
            : left(left),
              right(right),
              value(value),
              distance(distance) {}

        Node_(const Node_& obj) : left(obj.left), right(obj.right), value(obj.value), distance(obj.distance) {}

        ~Node_() = default;
    };

    using NodeAllocatorType = typename std::allocator_traits<Allocator>::template rebind_alloc<Node_>;

    Node_* data_ = nullptr;
    SizeT size_ = 0;
    Compare compare_;
    NodeAllocatorType nodeAllocator_;

    Node_* MergeNode_(Node_* node1, Node_* node2) {
        if (node1 == nullptr) return node2;
        if (node2 == nullptr) return node1;
        if (compare_(node1->value, node2->value)) { // merge the node1 into node2
            node2->right = MergeNode_(node1, node2->right);
            FlushNode(node2);
            return node2;
        } else {
            node1->right = MergeNode_(node1->right, node2);
            FlushNode(node1);
            return node1;
        }
    }

    void FlushNode(Node_* node) {
        if (node->right == nullptr) {
            node->distance = 0;
            return;
        }

        if (node->left == nullptr) {
            node->left = node->right;
            node->right = nullptr;
            node->distance = 0;
            return;
        }

        if (node->left->distance < node->right->distance) {
            Node_* tmp = node->left;
            node->left = node->right;
            node->right = tmp;
        }
        node->distance = node->right->distance + 1;
    }

    void Clear() {
        DeleteAllChildNode(data_);
        size_ = 0;
    }

    void DeleteAllChildNode(Node_* node) {
        if (node == nullptr) return;
        DeleteAllChildNode(node->left);
        DeleteAllChildNode(node->right);
        node->~Node_();
        nodeAllocator_.deallocate(node, 1);
    }

    Node_* CopyNode_(Node_* node) {
        Node_* newNode = nodeAllocator_.allocate(1);
        ::new(newNode) Node_(node->value, nullptr, nullptr, node->distance);
        if (node->left != nullptr) newNode->left = CopyNode_(node->left);
        if (node->right != nullptr) newNode->right = CopyNode_(node->right);
        return newNode;
    }
};

} // namespace lau

#endif // LAU_CPP_LIB_LAU_PRIORITY_QUEUE_H
