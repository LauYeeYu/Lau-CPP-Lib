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
 * @file lau/linked_hash_map.h
 *
 * This is a external header file, including a linked hash map class.
 */

#ifndef LAU_CPP_LIB_LAU_LINKED_HASH_MAP_H
#define LAU_CPP_LIB_LAU_LINKED_HASH_MAP_H

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include "exception.h"
#include "linked_hash_table.h"
#include "type_traits.h"
#include "rehash_policy.h"
#include "utility.h"

namespace lau {

/**
 * A mapping class whose key-value pairs are linked together.  Please note
 * that if a key is inserted, it cannot be modified.  If you want to modify
 * the key, you may use the raw data structure class -
 * <code>lau::RBTree</code>.
 *
 * @tparam Key the key type
 * @tparam Value the value type
 * @tparam Hash the class that can get the hash value of the key
 * @tparam Equal the class that can tell whether two keys are equal
 */
template<class Key,
         class Value,
         class Hash      = std::hash<Key>,
         class Equal     = std::equal_to<Key>,
         class Allocator = std::allocator<KeyValuePair<const Key, Value>>>
class LinkedHashMap {
public:
    using MapPair    = KeyValuePair<const Key, Value>;
    using value_type = KeyValuePair<const Key, Value>;
    
    class Iterator;
    class ConstIterator;

    class PairHash {
        friend class LinkedHashMap;

    public:
        PairHash() = default;
        explicit PairHash(const Hash& hash) : hash_(hash) {}
        PairHash(const PairHash&) = default;

        PairHash& operator=(const PairHash&) = default;

        [[nodiscard]] std::size_t operator()(const MapPair& pair) const { return hash_(pair.key); }

        template<class K>
        [[nodiscard]] std::size_t operator()(const K& key) const { return hash_(key); }

    private:
        Hash hash_;
    };

    class PairEqual {
        friend class LinkedHashMap;

    public:
        PairEqual() = default;
        explicit PairEqual(const Equal& equal) : equal_(equal) {}
        PairEqual(const PairEqual&) = default;

        PairEqual& operator=(const PairEqual&) = default;

        [[nodiscard]] bool operator()(const MapPair& lhs, const MapPair& rhs) const {
            return equal_(lhs.key, rhs.key);
        }

        template<class K>
        [[nodiscard]] bool operator()(const MapPair& lhs, const K& rhs) const {
            return equal_(lhs.key, rhs);
        }

        template<class K>
        [[nodiscard]] bool operator()(const K& lhs, const MapPair& rhs) const {
            return equal_(lhs, rhs.key);
        }

        template<class K1, class K2>
        [[nodiscard]] bool operator()(const K1& lhs, const K2& rhs) const {
            return equal_(lhs, rhs);
        }

    private:
        Equal equal_;
    };

    class Iterator {
        friend class LinkedHashMap;
        friend class ConstIterator;

    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // For instance, for an iterator, iterator::MapPair is the type that the
        // iterator points to.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using pointer           = MapPair*;
        using reference         = MapPair&;
        using iterator_category = std::output_iterator_tag;

        Iterator() = default;
        Iterator(const Iterator&) = default;

        Iterator& operator=(const Iterator&) = default;

        ~Iterator() = default;

        explicit operator MapPair*() const noexcept {
            return &((static_cast<typename LinkedHashTable<MapPair, PairHash, PairEqual, Allocator>::Node*>(iterator_))->value);
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator& operator++() {
            ++iterator_;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        Iterator& operator--() {
            --iterator_;
            return *this;
        }

        bool operator==(const Iterator& rhs) const { return this->iterator_ == rhs.iterator_; }
        bool operator==(const ConstIterator& rhs) const { return this->Iterator_ == rhs.iterator_; }
        bool operator!=(const Iterator& rhs) const { return this->iterator_ != rhs.iterator_; }
        bool operator!=(const ConstIterator& rhs) const { return this->iterator_ != rhs.iterator_; }

        MapPair& operator*()  const { return iterator_->value; }
        MapPair* operator->() const noexcept {return &(iterator_->value);}

    private:
        explicit Iterator(const typename LinkedHashTable<MapPair, PairHash, PairEqual, Allocator>::Iterator& iterator)
            : iterator_(iterator) {}

        typename LinkedHashTable<MapPair, PairHash, PairEqual, Allocator>::Iterator iterator_;
    };

    class ConstIterator {
        friend class LinkedHashMap;
        friend class Iterator;

    public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // For instance, for an iterator, iterator::MapPair is the type that the
        // iterator points to.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using pointer           = MapPair*;
        using reference         = MapPair&;
        using iterator_category = std::output_iterator_tag;

        ConstIterator() = default;
        ConstIterator(const ConstIterator& other) = default;
        ConstIterator(const Iterator& obj) : iterator_(obj.iterator_) {}

        ConstIterator& operator=(const ConstIterator& other) = default;

        ~ConstIterator() = default;

        explicit operator const MapPair*() const noexcept { return &(iterator_->value); }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        ConstIterator& operator++() {
            ++iterator_;
            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator tmp = *this;
            --(*this);
            return tmp;
        }

        ConstIterator& operator--() {
            --iterator_;
            return *this;
        }

        bool operator==(const Iterator& rhs) const { return this->iterator_ == rhs.iterator_; }
        bool operator==(const ConstIterator& rhs) const { return this->iterator_ == rhs.iterator_; }
        bool operator!=(const Iterator& rhs) const { return this->iterator_ != rhs.iterator_; }
        bool operator!=(const ConstIterator& rhs) const { return this->iterator_ != rhs.iterator_; }

        const MapPair& operator*()  const { return iterator_->value; }
        const MapPair* operator->() const { return &(iterator_->value); }

    private:
        explicit ConstIterator(const typename LinkedHashTable<MapPair, PairHash, PairEqual, Allocator>::ConstIterator& iterator)
            : iterator_(iterator) {}
        typename LinkedHashTable<MapPair, PairHash, PairEqual, Allocator>::ConstIterator iterator_;
    };

    LinkedHashMap() = default;

    explicit LinkedHashMap(const Allocator& allocator) : table_(allocator) {}

    explicit LinkedHashMap(const Hash& hash,
                           const Equal& equal = Equal(),
                           const Allocator& allocator = Allocator())
        : table_(PairHash(hash), PairEqual(equal), allocator) {}

    explicit LinkedHashMap(SizeT reservedSize,
                           const Hash& hash = Hash(),
                           const Equal& equal = Equal(),
                           const Allocator& allocator = Allocator())
        : table_(reservedSize, PairHash(hash), PairEqual(equal), allocator) {}

    template<class InputIt>
    LinkedHashMap(InputIt first, InputIt last,
                  const Hash& hash = Hash(),
                  const Equal& equal = Equal(),
                  const Allocator& allocator = Allocator())
        : table_(first, last, PairHash(hash), PairEqual(equal), allocator) {}

    template<class InputIt>
    LinkedHashMap(InputIt first, InputIt last,
                  SizeT reservedSize,
                  const Hash& hash = Hash(),
                  const Equal& equal = Equal(),
                  const Allocator& allocator = Allocator())
        : table_(first, last, reservedSize, PairHash(hash), PairEqual(equal), allocator) {}

    LinkedHashMap(const LinkedHashMap&) = default;

    LinkedHashMap(const LinkedHashMap& obj, const Allocator& allocator)
        : table_(obj.table_, allocator) {}

    LinkedHashMap(LinkedHashMap&&) noexcept = default;

    LinkedHashMap& operator=(const LinkedHashMap&) = default;
    LinkedHashMap& operator=(LinkedHashMap&&) noexcept = default;

    ~LinkedHashMap() = default;

    /**
     * Access a certain element.  If no such element exists, an
     * <code>lau::OutOfRange</code> will be returned.
     * @param key the key of the element to access
     * @return the reference to the mapped value of the element with key equal
     * to key.
     */
    [[nodiscard]] Value& At(const Key& key) {
        auto iter = table_.Find(key);
        if (iter == table_.End()) {
            throw OutOfRange();
        }
        return iter->value.value;
    }

    /**
     * Access a certain element.  If no such element exists, an
     * <code>lau::OutOfRange</code> will be returned.
     * @param key the key of the element to access
     * @return the const reference to the mapped value of the element with
     * key equal to key.
     */
    [[nodiscard]] const Value& At(const Key& key) const {
        auto iter = table_.Find(key);
        if (iter == table_.ConstEnd()) {
            throw OutOfRange();
        }
        return iter->value.value;
    }

    /**
     * Access or create a specified element.  If the element does exist, this
     * function will return the reference to the mapped value of the element.
     * If the element does not exist, this function will insert a new element
     * with the specified key and the default value, and return the reference
     * to the mapped value.
     *
     * @param key the input key
     * @return the reference to the value that is mapped to a key equal to
     * the input key, or the reference to the value that is inserted just now.
     */
    Value& operator[](const Key& key) {
        auto [iter, success] = table_.Insert(MapPair(key, Value()));
        return iter->value.value;
    }

    /**
     * Access or create a specified element.  If the element does exist, this
     * function will return the reference to the mapped value of the element.
     * If the element does not exist, this function will insert a new element
     * with the specified key and the default value, and return the reference
     * to the mapped value.
     *
     * @param key the input key
     * @return the reference to the value that is mapped to a key equal to
     * the input key, or the reference to the value that is inserted just now.
     */
    Value& operator[](Key&& key) {
        auto [iter, success] = table_.Insert(std::move(MapPair(std::move(key), Value())));
        return iter->value.value;
    }

    /**
     * Access a certain element.  If no such element exists, an
     * <code>lau::OutOfRange</code> will be returned.
     * @param key the key of the element to access
     * @return the const reference to the mapped value of the element with
     * key equal to key.
     */
    [[nodiscard]] const Value& operator[](const Key& key) const {
        auto iter = table_.Find(key);
        if (iter == table_.ConstEnd()) {
            throw OutOfRange();
        }
        return iter->value.value;
    }

    [[nodiscard]] Iterator Begin() { return Iterator(table_.Begin()); }
    [[nodiscard]] Iterator begin() { return Iterator(table_.Begin()); }
    [[nodiscard]] ConstIterator Begin() const { return ConstIterator(table_.ConstBegin()); }
    [[nodiscard]] ConstIterator ConstBegin() const { return ConstIterator(table_.Begin()); }

    [[nodiscard]] Iterator End() { return Iterator(table_.End()); }
    [[nodiscard]] Iterator end() { return Iterator(table_.End()); }
    [[nodiscard]] ConstIterator End() const { return ConstIterator(table_.ConstEnd()); }
    [[nodiscard]] ConstIterator ConstEnd() const { return ConstIterator(table_.End()); }

    [[nodiscard]] bool Empty() const { return table_.Empty(); }
    [[nodiscard]] SizeT Size() const { return table_.Size(); }

    /**
     * Clear the map.
     * @return the reference to the map.
     */
    LinkedHashMap& Clear() {
        table_.Clear();
        return *this;
    }

    /**
     * Try inserting the key-value pair into the map.  If the pair has not
     * already been contained in the table, the pair will be inserted and a
     * pair of the iterator pointing to the inserted element and a bool set
     * to true will be returned.  If not, a pair of the iterator pointing to
     * the existing element that is equal to the key and a bool set to false
     * will be returned.
     * @param pair
     * @return the iterator pointing to the inserted element or the existing
     * element that is equal to the key and a bool indicating whether the
     * operation is successful or not
     */
    Pair<Iterator, bool> Insert(const MapPair& pair) {
        auto [iter, success] = table_.Insert(pair);
        return Pair<Iterator, bool>(Iterator(iter), success);
    }

    /**
     * Try inserting the key-value pair into the map.  If the pair has not
     * already been contained in the table, the pair will be inserted and a
     * pair of the iterator pointing to the inserted element and a bool set
     * to true will be returned.  If not, a pair of the iterator pointing to
     * the existing element that is equal to the key and a bool set to false
     * will be returned.
     * @param pair
     * @return the iterator pointing to the inserted element or the existing
     * element that is equal to the key and a bool indicating whether the
     * operation is successful or not
     */
    Pair<Iterator, bool> Insert(MapPair&& pair) {
        auto [iter, success] = table_.Insert(std::move(pair));
        return Pair<Iterator, bool>(Iterator(iter), success);
    }

    /**
     * Insert or assign the key-value pair into the map.  If the pair has not
     * already been contained in the map, the pair will be inserted and a pair
     * of the iterator pointing to the inserted element and a bool set to true
     * will be returned.  If not, the element will be set to the input pair
     * and a pair of the iterator pointing to that existing element and a bool
     * set to false will be returned.  Note that the order of the linked list
     * is not changed in this progress.
     * @param pair
     * @return the iterator pointing to the inserted element or the existing
     * element that is equal to the key and a bool indicating whether the
     * operation is successful or not
     */
    Pair<Iterator, bool> InsertOrAssign(const MapPair& pair) {
        auto [iter, success] = table_.Insert(pair);
        if (!success) iter->value.value = pair.value;
        return Pair<Iterator, bool>(Iterator(iter), success);
    }

    /**
     * Insert or assign the key-value pair into the map.  If the pair has not
     * already been contained in the map, the pair will be inserted and a pair
     * of the iterator pointing to the inserted element and a bool set to true
     * will be returned.  If not, the element will be set to the input pair
     * and a pair of the iterator pointing to that existing element and a bool
     * set to false will be returned.  Note that the order of the linked list
     * is not changed in this progress.
     * @param pair
     * @return the iterator pointing to the inserted element or the existing
     * element that is equal to the key and a bool indicating whether the
     * operation is successful or not
     */
    Pair<Iterator, bool> InsertOrAssign(MapPair&& pair) {
        auto [iter, success] = table_.Insert(std::move(pair));
        if (!success) iter->value.value = std::move(pair.value);
        return Pair<Iterator, bool>(Iterator(iter), success);
    }

    /**
     * Try inserting the key-value pair into the hash using a in-place
     * construction of the contained class.  The function will construct the
     * contained class even if there is an element equal to it.  If the pair
     * has not already been contained in the table, the pair will be inserted
     * and a pair of the iterator pointing to the inserted element and a bool
     * set to true will be returned.  If not, a pair of the iterator pointing
     * to the existing element that is equal to the key and a bool set to
     * false will be returned.
     * @tparam Args...
     * @param args... the argument(s) to construct the contained class
     * @return the iterator pointing to the inserted element or the existing
     * element that is equal to the key and a bool indicating whether the
     * operation is successful or not
     */
    template<class... Args>
    Pair<Iterator, bool> Emplace(Args&&... args) {
        auto [iter, success] = table_.Emplace(std::forward<Args>(args)...);
        return Pair<Iterator, bool>(Iterator(iter), success);
    }

    /**
     * Erasing the key-value pair from the hash map.  If the position iterator
     * is not pointing to this table or is the end iterator, a
     * <code>lau::InvalidIterator</code> will be thrown.
     * @param position the iterator pointing to the element to be erased
     * @return the reference to the map
     */
    LinkedHashMap& Erase(Iterator position) {
        table_.Erase(position.iterator_);
        return *this;
    }

    /**
     * Erasing the element whose key is equal to the input key from the hash
     * map.  If the container is empty, a <code>lau::EmptyContainer</code>
     * will be thrown.  If the value is not contained in the table, a
     * <code>lau::InvalidArgument</code> will be thrown.
     * @param key the input key
     * @return the reference to the table
     */
    LinkedHashMap& Erase(const Key& key) {
        table_.Erase(key);
        return *this;
    }

    /**
     * Erasing the element whose key is equal to the input key from the hash
     * map.  If the container is empty, a <code>lau::EmptyContainer</code>
     * will be thrown.  If the value is not contained in the table, a
     * <code>lau::InvalidArgument</code> will be thrown.  Note that the
     * <code>Hash::is_transparent</code> and
     * <code>KeyEqual::is_transparent</code> must be valid and each denotes a
     * type.
     * @param key the input key
     * @return the reference to the table
     */
    template<class K>
    LinkedHashMap& Erase(const K& key) {
        table_.Erase(key);
        return *this;
    }

    /**
     * Merge other linked hash map into this linked hash map.  The function
     * will merge the elements of the other linked hash map into this linked
     * hash map.  If the key of the other linked hash map is already contained
     * in this linked hash map, the value of the other linked hash map will be
     * replaced by the value of this linked hash map.  If the key of the other
     * linked hash map is not contained in this linked hash map, the key-value
     * pair of the other linked hash map will be inserted into this linked
     * hash map.  After this operation, the other table will be cleared.
     * @param other
     * @return the reference to the map
     */
    LinkedHashMap& Merge(const LinkedHashMap& other) {
        table_.Merge(other.table_);
        return *this;
    }

    /**
     * Swap the content of this linked hash map with the other linked hash map.
     * @param other
     * @return the reference to the map
     */
    LinkedHashMap& Swap(LinkedHashMap& other) {
        table_.Swap(other.table_);
        return *this;
    }

    /**
     * Reserve at least the input size for the hash buckets.  If the input
     * size is larger than the maximum number of buckets, the number of
     * buckets will be set to the maximum number of buckets.
     * @param size
     * @return the reference to the map
     */
    LinkedHashMap& ReserveAtLeast(SizeT size) {
        table_.ReserveAtLeast(size);
        return *this;
    }

    [[nodiscard]] SizeT Count(const Key& key) const { return table_.Contains(key) ? 1 : 0; }

    template<class K>
    [[nodiscard]] SizeT Count(const K& key) const { return table_.Contains(key) ? 1 : 0; }

    [[nodiscard]] Iterator Find(const Key& key) { return Iterator(table_.Find(key)); }

    template<class K>
    [[nodiscard]] Iterator Find(const K& key) { return Iterator(table_.Find(key)); }

    [[nodiscard]] ConstIterator Find(const Key& key) const { return ConstIterator(table_.Find(key)); }

    template<class K>
    [[nodiscard]] ConstIterator Find(const K& key) const { return ConstIterator(table_.Find(key)); }

    [[nodiscard]] bool Contains(const Key& key) const { return table_.Contains(key); }

    template<class K>
    [[nodiscard]] bool Contains(const K& key) const { return table_.Contains(key); }

    [[nodiscard]] Allocator GetAllocator() const { return Allocator(table_.GetNodeAllocator()); }
    [[nodiscard]] PairHash GetPairHash() const { return table_.GetHash(); }
    [[nodiscard]] PairHash GetKeyHash() const { return table_.GetHash().hash_; }
    [[nodiscard]] PairHash GetPairEqual() const { return table_.GetEqual(); }
    [[nodiscard]] PairHash GetEqual() const { return table_.GetEqual().equal_; }

private:
    LinkedHashTable<MapPair, PairHash, PairEqual, Allocator> table_;
};

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
void Swap(LinkedHashMap<Key, Value, Hash, KeyEqual, Allocator>& lhs,
          LinkedHashMap<Key, Value, Hash, KeyEqual, Allocator>& rhs) {
    lhs.Swap(rhs);
}

} // namespace lau

#endif // LAU_CPP_LIB_LAU_LINKED_HASH_MAP_H
