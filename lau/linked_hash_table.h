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
 * @file lau/linked_hash_table.h
 *
 * This is a external header file, including a linked hash table class.
 */

#ifndef LAU_CPP_LIB_LAU_LINKED_HASH_TABLE_H
#define LAU_CPP_LIB_LAU_LINKED_HASH_TABLE_H

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include "exception.h"
#include "type_traits.h"
#include "rehash_policy.h"
#include "utility.h"

namespace lau {

/**
 * @class LinkedHashTable
 *
 * A hash table whose elements are linked together.
 * @tparam T
 * @tparam Hash
 * @tparam Equal
 * @tparam Allocator
 */
template <class T,
          class Hash = std::hash<T>,
          class Equal = std::equal_to<T>,
          class Allocator = std::allocator<T>>
class LinkedHashTable {
public:
    struct Node;
    class Iterator;
    class ConstIterator;
    class BucketIterator;
    class ConstBucketIterator;

    using NodeAllocatorType   = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using BucketAllocatorType = typename std::allocator_traits<Allocator>::template rebind_alloc<Node*>;

    struct Node {
        friend class LinkedHashTable;
    public:
        Node() = default;
        Node(Node&) = default;
        Node(const Node&) = default;
        Node(Node&&) = default;

        Node(std::size_t hashIn, const T& valueIn) : value(valueIn), hash(hashIn) {}
        Node(std::size_t hashIn, T&& valueIn) : value(std::move(valueIn)), hash(hashIn) {}
        /*explicit Node(const T& valueIn) : value(valueIn), hash(Hash()(value)) {}
        explicit Node(T&& valueIn) : value(std::move(valueIn)), hash(Hash()(value)) {}*/

        template<class... Args>
        explicit Node(std::size_t hashIn, Args&&... args) : value(std::forward<Args>(args)...), hash(hashIn) {}

        Node& operator=(Node&) = default;
        Node& operator=(const Node&) = default;
        Node& operator=(Node&&) = default;

        ~Node() = default;

        Node* BucketNext() const noexcept { return next; }
        Node* LinkedNext() const noexcept { return linkedNext; }
        Node* LinkedPrevious() const noexcept { return linkedPrevious; }

        // Every time you change the value, you should also change the hash
        T value;
        std::size_t hash = 0;

    private:
        Node* next = nullptr;
        Node* linkedNext = nullptr;
        Node* linkedPrevious = nullptr;
    };

    class Iterator {
        friend class LinkedHashTable;
        friend class ConstIterator;
        friend class BucketIterator;
        friend class ConstBucketIterator;

    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::output_iterator_tag;

        Iterator() noexcept = default;
        Iterator(const Iterator&) noexcept = default;

        Iterator& operator=(const Iterator&) noexcept = default;

        ~Iterator() = default;

        explicit operator Node*() const noexcept { return target_; }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator& operator++() {
            if (target_ == nullptr) throw InvalidIterator("Invalid Iterator: using ++ on the end iterator");
            target_ = target_->linkedNext;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        Iterator& operator--() {
            if (target_ == nullptr) {
                target_ = table_->tail_;
                if (target_ == nullptr) {
                    throw InvalidIterator("Invalid Iterator: using -- on an empty container");
                }
                return *this;
            }
            if (target_->linkedPrevious == nullptr) {
                throw InvalidIterator("Invalid Iterator: using -- on the end iterator");
            }
            target_ = target_->linkedPrevious;
            return *this;
        }

        bool operator==(const Iterator& rhs) const noexcept {
            return ((this->table_ == rhs.table_) && (this->target_ == rhs.target_));
        }

        bool operator==(const ConstIterator& rhs) const noexcept {
            return ((this->table_ == rhs.table_) && (this->target_ == rhs.target_));
        }

        bool operator!=(const Iterator& rhs) const noexcept {
            return ((this->table_ != rhs.table_) || (this->target_ != rhs.target_));
        }

        bool operator!=(const ConstIterator& rhs) const noexcept {
            return ((this->table_ != rhs.table_) || (this->target_ != rhs.target_));
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
        Iterator(Node* target, const LinkedHashTable* table) noexcept : target_(target), table_(table) {}

        Node* target_ = nullptr;
        const LinkedHashTable* table_ = nullptr;
    };

    class ConstIterator {
        friend class LinkedHashTable;
        friend class ConstBucketIterator;

    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::output_iterator_tag;

        ConstIterator() noexcept = default;
        ConstIterator(const ConstIterator&) noexcept = default;
        ConstIterator(const Iterator& obj) noexcept : target_(obj.target_), table_(obj.table_) {}

        ConstIterator& operator=(const ConstIterator&) noexcept = default;

        ~ConstIterator() = default;

        explicit operator const Node*() const noexcept { return target_; }

        ConstIterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        ConstIterator& operator++() {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: using ++ on the end iterator");
            }
            target_ = target_->linkedNext;
            return *this;
        }

        ConstIterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        ConstIterator& operator--() {
            if (target_ == nullptr) {
                target_ = table_->tail_;
                if (target_ == nullptr) {
                    throw InvalidIterator("Invalid Iterator: using -- on an empty container");
                }
                return *this;
            }
            if (target_->linkedPrevious == nullptr) {
                throw InvalidIterator("Invalid Iterator: using -- on the beginning iterator");
            }
            target_ = target_->linkedPrevious;
            return *this;
        }

        bool operator==(const Iterator& rhs) const noexcept {
            return ((this->table_ == rhs.table_) && (this->target_ == rhs.target_));
        }

        bool operator==(const ConstIterator& rhs) const noexcept {
            return ((this->table_ == rhs.table_) && (this->target_ == rhs.target_));
        }

        bool operator!=(const Iterator& rhs) const noexcept {
            return ((this->table_ != rhs.table_) || (this->target_ != rhs.target_));
        }

        bool operator!=(const ConstIterator& rhs) const noexcept {
            return ((this->table_ != rhs.table_) || (this->target_ != rhs.target_));
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
        ConstIterator(const Node* target, const LinkedHashTable* table) noexcept : target_(target), table_(table) {}

        const Node* target_ = nullptr;
        const LinkedHashTable* table_ = nullptr;
    };

    class BucketIterator {
    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::output_iterator_tag;

        BucketIterator() noexcept = default;
        BucketIterator(const BucketIterator&) noexcept = default;
        explicit BucketIterator(const Iterator& obj) noexcept
            : target_(obj.target_),
              bucket_(obj.table_->bucket_ + target_->hash % obj.table_->bucketSize_) {}

        BucketIterator& operator=(const BucketIterator&) noexcept = default;

        ~BucketIterator() = default;

        explicit operator Node*() const noexcept { return target_; }

        BucketIterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        BucketIterator& operator++() {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: using ++ on the end iterator");
            }
            target_ = target_->next;
            return *this;
        }

        bool operator==(const BucketIterator& rhs) const noexcept {
            return ((this->target_ == rhs.target_) && (this->bucket_ == rhs.bucket_));
        }

        bool operator==(const ConstBucketIterator& rhs) const noexcept {
            return ((this->target_ == rhs.target_) && (this->bucket_ == rhs.bucket_));
        }

        bool operator!=(const BucketIterator& rhs) const noexcept {
            return ((this->target_ != rhs.target_) || (this->bucket_ != rhs.bucket_));
        }

        bool operator!=(const ConstBucketIterator& rhs) const noexcept {
            return ((this->target_ != rhs.target_) || (this->bucket_ != rhs.bucket_));
        }

        Node& operator*() const {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: de-referencing an end iterator");
            }
            return *target_;
        }

        Node* operator->() const {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: de-referencing an end iterator");
            }
            return target_;
        }

    private:
        BucketIterator(Node* target, Node const* const* bucket) noexcept : target_(target), bucket_(bucket) {}

        Node* target_ = nullptr;
        Node const* const* bucket_ = nullptr;
    };

    class ConstBucketIterator {
    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::output_iterator_tag;

        ConstBucketIterator() noexcept = default;
        ConstBucketIterator(const ConstBucketIterator&) noexcept = default;
        ConstBucketIterator(const BucketIterator& obj) noexcept : target_(obj.target_), bucket_(obj.bucket_) {}

        explicit ConstBucketIterator(const Iterator& obj) noexcept
            : target_(obj.target_),
              bucket_(obj.table_->bucket_ + target_->hash % obj.table_->bucketSize_) {}

        explicit ConstBucketIterator(const ConstIterator& obj) noexcept
            : target_(obj.target_),
              bucket_(obj.table_->bucket_ + target_->hash % obj.table_->bucketSize_) {}

        ConstBucketIterator& operator=(const ConstBucketIterator&) noexcept = default;

        ~ConstBucketIterator() = default;

        explicit operator const Node*() const noexcept { return target_; }

        ConstBucketIterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        ConstBucketIterator& operator++() {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: using ++ on the end iterator");
            }
            target_ = target_->next;
            return *this;
        }

        bool operator==(const BucketIterator& rhs) const noexcept {
            return ((this->target_ == rhs.target_) && (this->bucket_ == rhs.bucket_));
        }

        bool operator==(const ConstBucketIterator& rhs) const noexcept {
            return ((this->target_ == rhs.target_) && (this->bucket_ == rhs.bucket_));
        }

        bool operator!=(const BucketIterator& rhs) const noexcept {
            return ((this->target_ != rhs.target_) || (this->bucket_ != rhs.bucket_));
        }

        bool operator!=(const ConstBucketIterator& rhs) const noexcept {
            return ((this->target_ != rhs.target_) || (this->bucket_ != rhs.bucket_));
        }

        const Node& operator*() const {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: de-referencing an end iterator");
            }
            return *target_;
        }

        const Node* operator->() const {
            if (target_ == nullptr) {
                throw InvalidIterator("Invalid Iterator: de-referencing an end iterator");
            }
            return target_;
        }

    private:
        ConstBucketIterator(Node* target, Node const* const* bucket) noexcept : target_(target), bucket_(bucket) {}

        Node* target_ = nullptr;
        Node const* const* bucket_ = nullptr;
    };

    explicit LinkedHashTable(const Allocator& allocator = Allocator())
        : hash_(), equal_(), rehashPolicy_(),
          nodeAllocator_(allocator), bucketAllocator_(allocator),
          head_(nullptr), tail_(nullptr), bucket_(nullptr),
          size_(0), bucketSize_(0) {}

    explicit LinkedHashTable(const Hash& hash,
                             const Equal& equal = Equal(),
                             const Allocator& allocator = Allocator())
        : hash_(hash), equal_(equal), rehashPolicy_(),
          nodeAllocator_(allocator), bucketAllocator_(allocator),
          head_(nullptr), tail_(nullptr), bucket_(nullptr),
          size_(0), bucketSize_(0) {}

    explicit LinkedHashTable(SizeT minimumBucketSize,
                             const Hash& hash = Hash(),
                             const Equal& equal = Equal(),
                             const Allocator& allocator = Allocator())
        : hash_(hash), equal_(equal), rehashPolicy_(minimumBucketSize),
          nodeAllocator_(allocator), bucketAllocator_(allocator),
          head_(nullptr), tail_(nullptr), bucket_(bucketAllocator_.allocate(rehashPolicy_.GetSize())),
          size_(0), bucketSize_(rehashPolicy_.GetSize()) {}

    template<class InputIt>
    LinkedHashTable(InputIt first, InputIt last,
                    const Hash& hash = Hash(),
                    const Equal& equal = Equal(),
                    const Allocator& allocator = Allocator())
        : hash_(hash), equal_(equal), rehashPolicy_(),
          nodeAllocator_(allocator), bucketAllocator_(allocator),
          head_(nullptr), tail_(nullptr), bucket_(nullptr),
          size_(0), bucketSize_(0) {
        try {
            while (first != last) {
                this->Insert(*first);
                ++first;
            }
        } catch (...) {
            this->Clear();
            throw;
        }
    }

    template<class InputIt>
    LinkedHashTable(InputIt first, InputIt last,
                    SizeT minimumBucketSize,
                    const Hash& hash = Hash(),
                    const Equal& equal = Equal(),
                    const Allocator& allocator = Allocator())
        : hash_(hash), equal_(equal), rehashPolicy_(minimumBucketSize),
          nodeAllocator_(allocator), bucketAllocator_(allocator),
          head_(nullptr), tail_(nullptr), bucket_(bucketAllocator_.allocate(rehashPolicy_.GetSize())),
          size_(0), bucketSize_(rehashPolicy_.GetSize()) {
        try {
            while (first != last) {
                this->Insert(*first);
                ++first;
            }
        } catch (...) {
            this->Clear();
            throw;
        }
    }

    LinkedHashTable(const LinkedHashTable& obj) : hash_(obj.hash_),
                                                  equal_(obj.equal_),
                                                  rehashPolicy_(obj.rehashPolicy_),
                                                  nodeAllocator_(obj.nodeAllocator_),
                                                  bucketAllocator_(obj.bucketAllocator_),
                                                  head_(nullptr),
                                                  tail_(nullptr),
                                                  bucket_(nullptr),
                                                  size_(obj.size_),
                                                  bucketSize_(obj.bucketSize_) {
        bucket_ = bucketAllocator_.allocate(bucketSize_);
        for (int i = 0; i < bucketSize_; ++i) {
            bucket_[i] = nullptr;
        }

        for (Node* node = obj.head_; node != nullptr; node = node->linkedNext) {
            Node* newNode;
            try {
                newNode = nodeAllocator_.allocate(1);
                try {
                ::new(newNode) Node(node->hash, node->value);
                } catch (...) {
                    nodeAllocator_.deallocate(newNode, 1);
                    throw;
                }
            } catch (...) {
                Node* toDelete = head_;
                while (toDelete != nullptr) {
                    Node* next = toDelete->linkedNext;
                    toDelete->~Node();
                    nodeAllocator_.deallocate(toDelete, 1);
                    toDelete = next;
                }
                bucketAllocator_.deallocate(bucket_, bucketSize_);
                throw;
            }
            this->Insert_(newNode);
        }
    }

    LinkedHashTable(const LinkedHashTable& obj,
                    const Allocator& allocator) : hash_(obj.hash_),
                                                  equal_(obj.equal_),
                                                  rehashPolicy_(obj.rehashPolicy_),
                                                  nodeAllocator_(allocator),
                                                  bucketAllocator_(allocator),
                                                  head_(nullptr),
                                                  tail_(nullptr),
                                                  bucket_(nullptr),
                                                  size_(obj.size_),
                                                  bucketSize_(obj.bucketSize_) {
        bucket_ = bucketAllocator_.allocate(bucketSize_);
        for (int i = 0; i < bucketSize_; ++i) {
            bucket_[i] = nullptr;
        }

        for (Node* node = obj.head_; node != nullptr; node = node->linkedNext) {
            Node* newNode;
            try {
                newNode = nodeAllocator_.allocate(1);
                try {
                    ::new(newNode) Node(node->hash, node->value);
                } catch (...) {
                    nodeAllocator_.deallocate(newNode, 1);
                    throw;
                }
            } catch (...) {
                Node* toDelete = head_;
                while (toDelete != nullptr) {
                    Node* next = toDelete->linkedNext;
                    toDelete->~Node();
                    nodeAllocator_.deallocate(toDelete, 1);
                    toDelete = next;
                }
                bucketAllocator_.deallocate(bucket_, bucketSize_);
                throw;
            }
            this->Insert_(newNode);
        }
    }

    LinkedHashTable(LinkedHashTable&& obj) noexcept
        : hash_(std::move(obj.hash_)),
          equal_(std::move(obj.equal_)),
          rehashPolicy_(obj.rehashPolicy_),
          nodeAllocator_(std::move(obj.nodeAllocator_)),
          bucketAllocator_(std::move(obj.bucketAllocator_)),
          head_(obj.head_),
          tail_(obj.tail_),
          bucket_(obj.bucket_),
          size_(obj.size_),
          bucketSize_(obj.bucketSize_) {
        obj.head_ = nullptr;
        obj.tail_ = nullptr;
        obj.bucket_ = nullptr;
        obj.size_ = 0;
        obj.bucketSize_ = 0;
    }

    LinkedHashTable(std::initializer_list<T> init,
                    const Hash& hash,
                    const Equal& equal = Equal(),
                    const Allocator& allocator = Allocator())
        : hash_(hash), equal_(equal), rehashPolicy_(init.size()),
          nodeAllocator_(allocator), bucketAllocator_(allocator),
          head_(nullptr), tail_(nullptr), bucket_(bucketAllocator_.allocate(rehashPolicy_.GetSize())),
          size_(0), bucketSize_(rehashPolicy_.GetSize()) {
        try {
            for (auto& value: init) {
                this->Insert_(value);
            }
        } catch (...) {
            this->Clear();
            throw;
        }
    }

    LinkedHashTable(std::initializer_list<T> init,
                    const Allocator& allocator = Allocator())
        : hash_(), equal_(), rehashPolicy_(init.size()),
          nodeAllocator_(allocator), bucketAllocator_(allocator),
          head_(nullptr), tail_(nullptr), bucket_(bucketAllocator_.allocate(rehashPolicy_.GetSize())),
          size_(0), bucketSize_(rehashPolicy_.GetSize()) {
        try {
            for (auto& value: init) {
                this->Insert_(value);
            }
        } catch (...) {
            this->Clear();
            throw;
        }
    }

    LinkedHashTable& operator=(const LinkedHashTable& obj) {
        if (this == &obj) {
            return *this;
        }

        NodeAllocatorType newNodeAllocator(obj.nodeAllocator_);
        BucketAllocatorType newBucketAllocator(obj.bucketAllocator_);
        Node** newBucket = newBucketAllocator.allocate(obj.bucketSize_);
        Node* newHead = nullptr;
        Node* newTail = nullptr;

        for (int i = 0; i < obj.bucketSize_; ++i) {
            newBucket[i] = nullptr;
        }

        for (Node* node = obj.head_; node != nullptr; node = node->linkedNext) {
            Node* newNode;
            try {
                newNode = newNodeAllocator.allocate(1);
                try {
                    ::new(newNode) Node(node->hash, node->value);
                } catch (...) {
                    nodeAllocator_.deallocate(newNode, 1);
                    throw;
                }
            } catch (...) {
                Node* toDelete = newHead;
                while (toDelete != nullptr) {
                    Node* next = toDelete->linkedNext;
                    toDelete->~Node();
                    nodeAllocator_.deallocate(toDelete, 1);
                    toDelete = next;
                }
                newBucketAllocator.deallocate(newBucket, obj.bucketSize_);
                throw;
            }
            // Handling the linked list
            newNode->linkedPrevious = newTail;
            if (newHead == nullptr) newHead = newNode;
            else newTail->linkedNext = newNode;
            newTail = newNode;

            // Handling the bucket
            std::size_t index = (newNode->hash) % obj.bucketSize_;
            newNode->next = newBucket[index];
            newBucket[index] = newNode;
        }

        this->Clear();
        this->hash_ = obj.hash_;
        this->equal_ = obj.equal_;
        this->rehashPolicy_ = obj.rehashPolicy_;
        this->size_ = obj.size_;
        this->bucketSize_ = obj.bucketSize_;
        this->bucket_ = newBucket;
        this->head_ = newHead;
        this->tail_ = newTail;
        this->nodeAllocator_ = std::move(newNodeAllocator);
        this->bucketAllocator_ = std::move(newBucketAllocator);

        return *this;
    }

    LinkedHashTable& operator=(LinkedHashTable&& obj) noexcept {
        this->Clear();
        hash_ = std::move(obj.hash_);
        equal_ = std::move(obj.equal_);
        rehashPolicy_ = obj.rehashPolicy_;
        nodeAllocator_ = std::move(obj.nodeAllocator_);
        bucketAllocator_ = std::move(obj.bucketAllocator_);
        head_ = obj.head_;
        tail_ = obj.tail_;
        bucket_ = obj.bucket_;
        size_ = obj.size_;
        bucketSize_ = obj.bucketSize_;
        obj.head_ = nullptr;
        obj.tail_ = nullptr;
        obj.bucket_ = nullptr;
        obj.size_ = 0;
        obj.bucketSize_ = 0;
        return *this;
    }

    ~LinkedHashTable() { this->Clear(); }

    /**
     * Delete all the contents of the hash table.  This will also free the
     * memory of the bucket table.
     * @return the reference to the hash table
     */
    LinkedHashTable& Clear() noexcept {
        Node* toDelete = head_;
        while (toDelete != nullptr) {
            Node* next = toDelete->linkedNext;
            toDelete->~Node();
            nodeAllocator_.deallocate(toDelete, 1);
            toDelete = next;
        }
        bucketAllocator_.deallocate(bucket_, bucketSize_);
        bucket_ = nullptr;
        bucketSize_ = 0;
        rehashPolicy_.SetSize(0);
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
        return *this;
    }

    /**
     * Try inserting the value into the table.  If the value has not already
     * been contained in the table, the value will be inserted and a pair of
     * the iterator pointing to the inserted element and a bool set to true
     * will be returned.  If not, a pair of the iterator pointing to the
     * existing element that is equal to the value and a bool set to false
     * will be returned.
     * @param value
     * @return the iterator pointing to the inserted element or the existing
     * element that is equal to the value and a bool indicating whether the
     * operation is successful or not
     */
    Pair<Iterator, bool> Insert(const T& value) {
        if (NeedRehash_()) Rehash_();
        std::size_t hash = hash_(value);
        std::size_t bucketIndex = hash % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, value)) {
                return Pair<Iterator, bool>(Iterator(tmpNode, this), false);
            }
            tmpNode = tmpNode->next;
        }
        Node* newNode = nodeAllocator_.allocate(1);
        try {
            ::new(newNode) Node(hash, value);
        } catch (...) {
            nodeAllocator_.deallocate(newNode, 1);
            throw;
        }
        newNode->next = bucket_[bucketIndex];
        bucket_[bucketIndex] = newNode;
        newNode->linkedPrevious = tail_;

        if (size_ == 0) head_ = newNode;
        else tail_->linkedNext = newNode;
        tail_ = newNode;

        ++size_;
        return Pair<Iterator, bool>(Iterator(newNode, this), true);
    }

    /**
     * Try inserting the value into the table.  If the value has not already
     * been contained in the table, the value will be inserted and a pair of
     * the iterator pointing to the inserted element and a bool set to true
     * will be returned.  If not, a pair of the iterator pointing to the
     * existing element that is equal to the value and a bool set to false
     * will be returned.
     * @param value
     * @return the iterator pointing to the inserted element or the existing
     * element that is equal to the value and a bool indicating whether the
     * operation is successful or not
     */
    Pair<Iterator, bool> Insert(T&& value) {
        if (NeedRehash_()) Rehash_();
        std::size_t hash = hash_(value);
        std::size_t bucketIndex = hash % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, value)) {
                return Pair<Iterator, bool>(Iterator(tmpNode, this), false);
            }
            tmpNode = tmpNode->next;
        }
        Node* newNode = nodeAllocator_.allocate(1);
        try {
            ::new(newNode) Node(hash, value);
        } catch (...) {
            nodeAllocator_.deallocate(newNode, 1);
            throw;
        }
        newNode->next = bucket_[bucketIndex];
        bucket_[bucketIndex] = newNode;
        newNode->linkedPrevious = tail_;

        if (size_ == 0) head_ = newNode;
        else tail_->linkedNext = newNode;
        tail_ = newNode;

        ++size_;
        return Pair<Iterator, bool>(Iterator(newNode, this), true);
    }

    /**
     * Insert or assign the value into the table.  If the value has not already
     * been contained in the table, the value will be inserted and a pair of
     * the iterator pointing to the inserted element and a bool set to true
     * will be returned.  If not, the element will be set to the input value
     * and a pair of the iterator pointing to that existing element and a bool
     * set to false will be returned.  Note that the order of the linked list
     * is not changed in this progress.
     * @param value
     * @return the iterator pointing to the inserted element or the existing
     * element that is equal to the value and a bool indicating whether the
     * operation is successful or not
     */
    Pair<Iterator, bool> InsertOrAssign(const T& value) {
        if (NeedRehash_()) Rehash_();
        std::size_t hash = hash_(value);
        std::size_t bucketIndex = hash % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, value)) {
                tmpNode->value = value;
                tmpNode->hash = hash;
                return Pair<Iterator, bool>(Iterator(tmpNode, this), false);
            }
            tmpNode = tmpNode->next;
        }
        Node* newNode = nodeAllocator_.allocate(1);
        try {
            ::new(newNode) Node(hash, value);
        } catch (...) {
            nodeAllocator_.deallocate(newNode, 1);
            throw;
        }
        newNode->next = bucket_[bucketIndex];
        bucket_[bucketIndex] = newNode;
        newNode->linkedPrevious = tail_;

        if (size_ == 0) head_ = newNode;
        else tail_->linkedNext = newNode;
        tail_ = newNode;

        ++size_;
        return Pair<Iterator, bool>(Iterator(newNode, this), true);
    }

    /**
     * Insert or assign the value into the table.  If the value has not already
     * been contained in the table, the value will be inserted and a pair of
     * the iterator pointing to the inserted element and a bool set to true
     * will be returned.  If not, the element will be set to the input value
     * and a pair of the iterator pointing to that existing element and a bool
     * set to false will be returned.  Note that the order of the linked list
     * is not changed in this progress.
     * @param value
     * @return the iterator pointing to the inserted element or the existing
     * element that is equal to the value and a bool indicating whether the
     * operation is successful or not
     */
    Pair<Iterator, bool> InsertOrAssign(T&& value) {
        if (NeedRehash_()) Rehash_();
        std::size_t hash = hash_(value);
        std::size_t bucketIndex = hash % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, value)) {
                tmpNode->value = value;
                tmpNode->hash = hash;
                return Pair<Iterator, bool>(Iterator(tmpNode, this), false);
            }
            tmpNode = tmpNode->next;
        }
        Node* newNode = nodeAllocator_.allocate(1);
        try {
            ::new(newNode) Node(hash, value);
        } catch (...) {
            nodeAllocator_.deallocate(newNode, 1);
            throw;
        }
        newNode->next = bucket_[bucketIndex];
        bucket_[bucketIndex] = newNode;
        newNode->linkedPrevious = tail_;

        if (size_ == 0) head_ = newNode;
        else tail_->linkedNext = newNode;
        tail_ = newNode;

        ++size_;
        return Pair<Iterator, bool>(Iterator(newNode, this), true);
    }

    /**
     * Try inserting the value into the hash using a in-place construction of
     * the contained class.  The function will construct the contained class
     * even if there is an element equal to it.  If the value has not already
     * been contained in the table, the value will be inserted and a pair of
     * the iterator pointing to the inserted element and a bool set to true
     * will be returned.  If not, a pair of the iterator pointing to the
     * existing element that is equal to the value and a bool set to false
     * will be returned.
     * @tparam Args...
     * @param args... the argument(s) to construct the contained class
     * @return the iterator pointing to the inserted element or the existing
     * element that is equal to the value and a bool indicating whether the
     * operation is successful or not
     */
    template<class... Args>
    Pair<Iterator, bool> Emplace(Args&&... args) {
        if (NeedRehash_()) Rehash_();

        Node* newNode = nodeAllocator_.allocate(1);
        try {
            ::new(newNode) Node(0, std::forward<Args>(args)...);
        } catch (...) {
            nodeAllocator_.deallocate(newNode, 1);
            throw;
        }

        try {
            newNode->hash = hash_(newNode->value);
        } catch (...) {
            newNode->~Node();
            nodeAllocator_.deallocate(newNode, 1);
            throw;
        }
        std::size_t hash = newNode->hash;
        std::size_t bucketIndex = hash % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];
        try {
            while (tmpNode != nullptr) {
                if (equal_(tmpNode->value, newNode->value)) {
                    newNode->~Node();
                    nodeAllocator_.deallocate(newNode, 1);
                    return Pair<Iterator, bool>(Iterator(tmpNode, this), false);
                }
                tmpNode = tmpNode->next;
            }
        } catch (...) {
            newNode->~Node();
            nodeAllocator_.deallocate(newNode, 1);
            throw;
        }

        newNode->next = bucket_[bucketIndex];
        bucket_[bucketIndex] = newNode;
        newNode->linkedPrevious = tail_;

        if (size_ == 0) head_ = newNode;
        else tail_->linkedNext = newNode;
        tail_ = newNode;

        ++size_;
        return Pair<Iterator, bool>(Iterator(newNode, this), true);
    }

    /**
     * Insert ot assign the value into the hash using a in-place construction
     * of the contained class.  The function will construct the contained class
     * even if there is an element equal to it.  If the value has not already
     * been contained in the table, the value will be inserted and a pair of
     * the iterator pointing to the inserted element and a bool set to true
     * will be returned.  If not, the element will be set to the input value
     * and a pair of the iterator pointing to that existing element and a bool
     * set to false will be returned.  Note that the order of the linked
     * list is not changed in this progress.
     * @tparam Args...
     * @param args... the argument(s) to construct the contained class
     * @return the iterator pointing to the inserted element or the existing
     * element that is equal to the value and a bool indicating whether the
     * operation is successful or not
     */
    template<class... Args>
    Pair<Iterator, bool> EmplaceOrAssign(Args&&... args) {
        if (NeedRehash_()) Rehash_();

        T value(std::forward<Args>(args)...);
        std::size_t hash = hash_(value);
        std::size_t bucketIndex = hash % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, value)) {
                tmpNode->value = std::move(value);
                tmpNode->hash = hash;
                return Pair<Iterator, bool>(Iterator(tmpNode, this), false);
            }
            tmpNode = tmpNode->next;
        }

        Node* newNode = nodeAllocator_.allocate(1);
        try {
            ::new(newNode) Node(hash, std::move(value));
        } catch (...) {
            nodeAllocator_.deallocate(newNode, 1);
            throw;
        }

        newNode->next = bucket_[bucketIndex];
        bucket_[bucketIndex] = newNode;
        newNode->linkedPrevious = tail_;

        if (size_ == 0) head_ = newNode;
        else tail_->linkedNext = newNode;
        tail_ = newNode;

        ++size_;
        return Pair<Iterator, bool>(Iterator(newNode, this), true);
    }

    /**
     * Erasing the value from the hash table.  If the position iterator is not
     * pointing to this table or is the end iterator, a
     * <code>lau::InvalidIterator</code> will be thrown.
     * @param position the iterator pointing to the element to be erased
     * @return the reference to the table
     */
    LinkedHashTable& Erase(Iterator position) {
        if (position.table_ != this) {
            throw InvalidIterator("Invalid Iterator: using an iterator that does not belong to the hash table");
        }
        if (position.target_ == nullptr) {
            throw InvalidIterator("Invalid Iterator: erasing with the end iterator");
        }
        Erase_(position.target_);
        return *this;
    }

    /**
     * Erasing the value from the hash table.  If the container is empty, a
     * <code>lau::EmptyContainer</code> will be thrown.  If the value is not
     * contained in the table, a <code>lau::InvalidArgument</code> will be
     * thrown.
     * @param value the value to be deleted
     * @return the reference to the table
     */
    LinkedHashTable& Erase(const T& value) {
        if (size_ == 0) {
            throw EmptyContainer("Empty Container: using erasing function on an empty container");
        }
        std::size_t bucketIndex = hash_(value) % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];
        if (tmpNode == nullptr) {
            throw InvalidArgument("Invalid Argument: no such element to be erased");
        }

        if (equal_(tmpNode->value, value)) {
            bucket_[bucketIndex] = tmpNode->next;
            if (tmpNode == head_) head_ = tmpNode->linkedNext;
            if (tmpNode == tail_) tail_ = tmpNode->linkedPrevious;
            if (tmpNode->linkedNext != nullptr) {
                tmpNode->linkedNext->linkedPrevious = tmpNode->linkedPrevious;
            }
            if (tmpNode->linkedPrevious != nullptr) {
                tmpNode->linkedPrevious->linkedNext = tmpNode->linkedNext;
            }
            tmpNode->~Node();
            nodeAllocator_.deallocate(tmpNode, 1);
            --size_;
            return *this;
        }

        Node* previousNode = tmpNode;
        tmpNode = tmpNode->next;
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, value)) {
                if (tmpNode == head_) head_ = tmpNode->linkedNext;
                if (tmpNode == tail_) tail_ = tmpNode->linkedPrevious;
                if (tmpNode->linkedNext != nullptr) {
                    tmpNode->linkedNext->linkedPrevious = tmpNode->linkedPrevious;
                }
                if (tmpNode->linkedPrevious != nullptr) {
                    tmpNode->linkedPrevious->linkedNext = tmpNode->linkedNext;
                }
                previousNode->next = tmpNode->next;
                tmpNode->~Node();
                nodeAllocator_.deallocate(tmpNode, 1);
                --size_;
                return *this;
            }
            previousNode = tmpNode;
            tmpNode = tmpNode->next;
        }
        throw InvalidArgument("Invalid Argument: no such element to be erased");
    }

    /**
     * Erasing the value from the hash table.  If the container is empty, a
     * <code>lau::EmptyContainer</code> will be thrown.  If the value is not
     * contained in the table, a <code>lau::InvalidArgument</code> will be
     * thrown.  Note that the <code>Hash::is_transparent</code> and
     * <code>KeyEqual::is_transparent</code> must be valid and each denotes a
     * type.
     * @tparam K the type of value
     * @param value
     * @return the reference to the table
     */
    template<class K>
    LinkedHashTable& Erase(const K& value) {
        if (size_ == 0) {
            throw EmptyContainer("Empty Container: using erasing function on an empty container");
        }
        std::size_t bucketIndex = hash_(value) % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];
        if (tmpNode == nullptr) {
            throw InvalidArgument("Invalid Argument: no such element to be erased");
        }

        if (equal_(tmpNode->value, value)) {
            bucket_[bucketIndex] = tmpNode->next;
            if (tmpNode == head_) head_ = tmpNode->linkedNext;
            if (tmpNode == tail_) tail_ = tmpNode->linkedPrevious;
            if (tmpNode->linkedNext != nullptr) {
                tmpNode->linkedNext->linkedPrevious = tmpNode->linkedPrevious;
            }
            if (tmpNode->linkedPrevious != nullptr) {
                tmpNode->linkedPrevious->linkedNext = tmpNode->linkedNext;
            }
            tmpNode->~Node();
            nodeAllocator_.deallocate(tmpNode, 1);
            --size_;
            return *this;
        }

        Node* previousNode = tmpNode;
        tmpNode = tmpNode->next;
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, value)) {
                if (tmpNode == head_) head_ = tmpNode->linkedNext;
                if (tmpNode == tail_) tail_ = tmpNode->linkedPrevious;
                if (tmpNode->linkedNext != nullptr) {
                    tmpNode->linkedNext->linkedPrevious = tmpNode->linkedPrevious;
                }
                if (tmpNode->linkedPrevious != nullptr) {
                    tmpNode->linkedPrevious->linkedNext = tmpNode->linkedNext;
                }
                previousNode->next = tmpNode->next;
                tmpNode->~Node();
                nodeAllocator_.deallocate(tmpNode, 1);
                --size_;
                return *this;
            }
            previousNode = tmpNode;
            tmpNode = tmpNode->next;
        }
        throw InvalidArgument("Invalid Argument: no such element to be erased");
    }

    /**
     * Merge other linked hash table into this linked hash table.  The function
     * will merge the elements of the other linked hash table into this linked
     * hash table.  If the key of the other linked hash table is already contained
     * in this linked hash table, the value of the other linked hash table will be
     * replaced by the value of this linked hash table.  If the key of the other
     * linked hash table is not contained in this linked hash table, the key-value
     * pair of the other linked hash table will be inserted into this linked
     * hash table.  After this operation, the other table will be cleared.
     * @param other
     * @return the reference to the table
     */
    LinkedHashTable& Merge(LinkedHashTable& other) {
        if (NeedRehash_()) ReserveAtLeast(size_);
        for (Node* node = other.head_; node != nullptr; node = node->linkedNext) {
            if (Find_(node->value, node->hash) == nullptr) {
                this->Insert_(node);
                ++size_;
            }
        }

        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;

        for (int i = 0; i < other.bucket_.size(); ++i) {
            other.bucket_[i] = nullptr;
        }
        return *this;
    }

    /**
     * Swap the content of two hash tables.
     * @param other
     * @return the reference to the table
     */
    LinkedHashTable& Swap(LinkedHashTable& other) noexcept {
        SizeT tmpSize = this->size_;
        this->size_ = other.size_;
        other.size_ = tmpSize;

        Node** tmpBucket = this->bucket_;
        this->bucket_ = other.bucket_;
        other.bucket_ = tmpBucket;

        Node* tmpHead = this->head_;
        this->head_ = other.head_;
        other.head_ = tmpHead;

        Node* tmpTail = this->tail_;
        this->tail_ = other.tail_;
        other.tail_ = tmpTail;

        SizeT tmpBucketSize = this->bucketSize_;
        this->bucketSize_ = other.bucketSize_;
        other.bucketSize_ = tmpBucketSize;

        Hash tmpHash = std::move(this->hash_);
        this->hash_ = std::move(other.hash_);
        other.hash_ = std::move(tmpHash);

        Equal tmpEqual = std::move(this->equal_);
        this->equal_ = std::move(other.equal_);
        other.equal_ = std::move(tmpEqual);

        NodeAllocatorType tmpNodeAllocator = std::move(this->nodeAllocator_);
        this->nodeAllocator_ = std::move(other.nodeAllocator_);
        other.nodeAllocator_ = std::move(tmpNodeAllocator);

        BucketAllocatorType tmpBucketAllocator = std::move(this->bucketAllocator_);
        this->bucketAllocator_ = std::move(other.bucketAllocator_);
        other.bucketAllocator_ = std::move(tmpBucketAllocator);

        RehashPolicy tmpRehashPolicy = this->rehashPolicy_;
        this->rehashPolicy_ = other.rehashPolicy_;
        other.rehashPolicy_ = tmpRehashPolicy;

        return *this;
    }

    /**
     * Reserve the bucket size with at lest <code>minimumSize</code> elements.
     * If the size reaches the maximum size, the bucket size will be the
     * maximum size.
     * @param minimumSize
     * @return the reference to the table
     */
    LinkedHashTable& ReserveAtLeast(SizeT minimumSize) noexcept {
        if (minimumSize <= bucketSize_ ||
            minimumSize >= rehashPolicy_.NextSize()) {
            return *this;
        }
        SizeT newSize = rehashPolicy_.ReserveAtLeast(minimumSize);
        Node** newBucket;
        try {
            newBucket = bucketAllocator_.allocate(newSize);
        } catch (...) {
            rehashPolicy_.SetSize(bucketSize_);
            throw;
        }
        bucketAllocator_.deallocate(bucket_, bucketSize_);
        bucket_ = newBucket;
        bucketSize_ = newSize;
        for (SizeT i = 0; i < newSize; ++i) {
            newBucket[i] = nullptr;
        }
        for (Node* node = head_; node != nullptr; node = node->next) {
            Reinsert_(node);
        }
        return *this;
    }

    [[nodiscard]] Iterator Begin() noexcept { return Iterator(head_, this); }
    [[nodiscard]] ConstIterator Begin() const noexcept { return ConstIterator(head_, this); }
    [[nodiscard]] Iterator begin() noexcept { return Iterator(head_, this); }
    [[nodiscard]] ConstIterator begin() const noexcept { return ConstIterator(head_, this); }

    [[nodiscard]] BucketIterator Begin(SizeT bucketIndex) {
        if (bucketIndex >= bucketSize_) {
            throw InvalidArgument("Invalid Argument: bucket index out of range");
        }
        return BucketIterator(bucket_[bucketIndex], bucket_ + bucketIndex);
    }

    [[nodiscard]] ConstBucketIterator Begin(SizeT bucketIndex) const {
        if (bucketIndex >= bucketSize_) {
            throw InvalidArgument("Invalid Argument: bucket index out of range");
        }
        return ConstBucketIterator(bucket_[bucketIndex], bucket_ + bucketIndex);
    }

    [[nodiscard]] ConstIterator ConstBegin() const noexcept { return ConstIterator(head_, this); }

    [[nodiscard]] ConstBucketIterator ConstBegin(SizeT bucketIndex) const {
        if (bucketIndex >= bucketSize_) {
            throw InvalidArgument("Invalid Argument: bucket index out of range");
        }
        return ConstBucketIterator(bucket_[bucketIndex], bucket_ + bucketIndex);
    }

    [[nodiscard]] Iterator End() noexcept { return Iterator(nullptr, this); }
    [[nodiscard]] ConstIterator End() const noexcept { return ConstIterator(nullptr, this); }
    [[nodiscard]] Iterator end() noexcept { return Iterator(nullptr, this); }
    [[nodiscard]] ConstIterator end() const noexcept { return ConstIterator(nullptr, this); }

    [[nodiscard]] BucketIterator End(SizeT bucketIndex) {
        if (bucketIndex >= bucketSize_) {
            throw InvalidArgument("Invalid Argument: bucket index out of range");
        }
        return BucketIterator(nullptr, bucket_ + bucketIndex);
    }

    [[nodiscard]] ConstBucketIterator End(SizeT bucketIndex) const {
        if (bucketIndex >= bucketSize_) {
            throw InvalidArgument("Invalid Argument: bucket index out of range");
        }
        return ConstBucketIterator(nullptr, bucket_ + bucketIndex);
    }

    [[nodiscard]] ConstIterator ConstEnd() const noexcept { return ConstIterator(nullptr, this); }

    [[nodiscard]] ConstBucketIterator ConstEnd(SizeT bucketIndex) const {
        if (bucketIndex >= bucketSize_) {
            throw InvalidArgument("Invalid Argument: bucket index out of range");
        }
        return ConstBucketIterator(nullptr, bucket_ + bucketIndex);
    }

    [[nodiscard]] Iterator Find(const T& value) { return Iterator(Find_(value), this); }

    template<class K>
    [[nodiscard]] Iterator Find(const K& value) { return Iterator(Find_(value), this); }

    [[nodiscard]] ConstIterator Find(const T& value) const { return ConstIterator(Find_(value), this); }

    template<class K>
    [[nodiscard]] ConstIterator Find(const K& value) const { return ConstIterator(Find_(value), this); }

    [[nodiscard]] SizeT Count(const T& value) const { return Find_(value) != nullptr ? 1 : 0; }

    template<class K>
    [[nodiscard]] SizeT Count(const K& value) const { return Find_(value) != nullptr ? 1 : 0; }

    [[nodiscard]] bool Contains(const T& value) const { return Find_(value) != nullptr; }

    template<class K>
    [[nodiscard]] bool Contains(const K& value) const { return Find_(value) != nullptr; }

    [[nodiscard]] SizeT Size() const noexcept { return size_; }
    [[nodiscard]] bool Empty() const noexcept { return size_ == 0; }
    [[nodiscard]] SizeT BucketCount() const noexcept { return bucketSize_; }

    /**
     * Get a copy of the hash class.
     * @return a copy of the hash class
     */
    [[nodiscard]] Hash GetHash() const { return hash_; }

    /**
     * Get a copy of the equal class.
     * @return a copy of the equal class
     */
    [[nodiscard]] Equal GetEqual() const { return equal_; }

    /**
     * Get a copy of the node allocator class.
     * @return a copy of the node allocator class
     */
    [[nodiscard]] NodeAllocatorType GetNodeAllocator() const { return nodeAllocator_; }

    /**
     * Get a copy of the bucket allocator class.
     * @return a copy of the bucket allocator class
     */
    [[nodiscard]] BucketAllocatorType GetAllocator() const { return bucketAllocator_; }

private:
    /**
     * Check whether the hash table needs to be rehashed.
     * @return
     */
    [[nodiscard]] bool NeedRehash_() const noexcept {
        if (size_ >= RehashPolicy::MaxSize()) return false;
        return (size_ >= bucketSize_);
    }

    /**
     * Rehash the hash table.
     */
    void Rehash_() {
        SizeT lastSize = bucketSize_;
        bucketSize_ = rehashPolicy_.NextSize();
        Node** newBucket;
        try {
            newBucket = bucketAllocator_.allocate(bucketSize_);
        } catch (...) {
            bucketSize_ = rehashPolicy_.PreviousSize();
            throw;
        }
        bucketAllocator_.deallocate(bucket_, lastSize);
        bucket_ = newBucket;
        for (int i = 0; i < bucketSize_; ++i) bucket_[i] = nullptr;
        for (Node* node = head_; node != nullptr; node = node->linkedNext) Reinsert_(node);
    }

    /**
     * Insert the node into the bucket, the size will not be updated.  Note
     * that the node must be unique.
     * @param node
     */
    void Insert_(Node* node) noexcept {
        node->linkedPrevious = tail_;
        if (head_ == nullptr) head_ = node;
        else tail_->linkedNext = node;
        tail_ = node;
        this->Reinsert_(node);
    }

    /**
     * Reinsert the node into the buckets.  This function only handles the
     * stuff about the hash table, it does not handle the linked list.
     * @param node the node to reinsert
     */
    void Reinsert_(Node* node) noexcept {
        std::size_t index = (node->hash) % bucketSize_;
        node->next = bucket_[index];
        bucket_[index] = node;
    }

    /**
     * Find the node that has the equal value with the given value.
     * @tparam K
     * @param key
     * @return the pointer to the node if found, otherwise nullptr
     */
    template<class K>
    [[nodiscard]] Node* Find_(const K& value) const {
        return Find_(value, hash_(value));
    }

    /**
     * Find the node that has the equal value with the given value.
     * @tparam K
     * @param key
     * @param hash
     * @return the pointer to the node if found, otherwise nullptr
     */
    template<class K>
    [[nodiscard]] Node* Find_(const K& value, std::size_t hash) const {
        if (size_ == 0) return nullptr;
        std::size_t bucketIndex = hash % bucketSize_;
        Node* tmpNode = bucket_[bucketIndex];
        while (tmpNode != nullptr) {
            if (equal_(tmpNode->value, value)) {
                return tmpNode;
            }
            tmpNode = tmpNode->next;
        }
        return nullptr;
    }

    /**
     * Remove the node from the bucket.
     * @param node
     */
    void Erase_(Node* node) noexcept {
        if (node == head_) head_ = node->linkedNext;
        if (node == tail_) tail_ = node->linkedPrevious;
        if (node->linkedNext != nullptr) {
            node->linkedNext->linkedPrevious = node->linkedPrevious;
        }
        if (node->linkedPrevious != nullptr) {
            node->linkedPrevious->linkedNext = node->linkedNext;
        }
        std::size_t hash = node->hash % bucketSize_;
        Node* tmpNode = bucket_[hash];
        if (tmpNode == node) {
            bucket_[hash] = node->next;
        } else {
            while (tmpNode->next != node) tmpNode = tmpNode->next;
            tmpNode->next = node->next;
        }
        node->~Node();
        nodeAllocator_.deallocate(node, 1);
        --size_;
    }

    Node*  head_       = nullptr;
    Node*  tail_       = nullptr;
    Node** bucket_     = nullptr;
    SizeT  size_       = 0; // the number of elements in the hash table
    SizeT  bucketSize_ = 0; // the number of buckets
    Hash                 hash_;
    Equal                equal_;
    RehashPolicy         rehashPolicy_;
    NodeAllocatorType    nodeAllocator_;
    BucketAllocatorType  bucketAllocator_;
};

template<class T, class Hash, class Equal, class Allocator>
void Swap(LinkedHashTable<T, Hash, Equal, Allocator>& lhs,
          LinkedHashTable<T, Hash, Equal, Allocator>& rhs) noexcept {
    lhs.Swap(rhs);
}

} // namespace lau

#endif // LAU_CPP_LIB_LAU_LINKED_HASH_TABLE_H
