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
 * @file lau/map.h
 *
 * This is a external header file, including a map class (using the red black
 * tree).
 */

#ifndef LAU_CPP_LIB_LAU_MAP_H
#define LAU_CPP_LIB_LAU_MAP_H

#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include "exception.h"
#include "RB_tree.h"
#include "type_traits.h"
#include "utility.h"

namespace lau {

/**
 * A class mapping a key to a value.  Please note that if a key is inserted,
 * it cannot be modified.  If you want to modify the key stored in the
 * container, you may use the raw data structure class -
 * <code>lau::RBTree</code>.
 *
 * @tparam Key
 * @tparam Value
 * @tparam Compare
 * @tparam Allocator the allocator of key value pair
 */
template<class Key,
         class Value,
         class Compare   = std::less<Key>,
         class Allocator = std::allocator<KeyValuePair<const Key, Value>>>
class Map {
public:
    using MapPair    = KeyValuePair<const Key, Value>;
    using value_type = KeyValuePair<const Key, Value>;

    class Iterator;
    class ConstIterator;

    class PairCompare {
        friend Map;

    public:
        PairCompare() = default;
        explicit PairCompare(const Compare& compareIn) : compare_(compareIn) {}

        bool operator()(const MapPair& lhs, const MapPair& rhs) const {
            return compare_(lhs.key, rhs.key);
        }

        template<class K>
        bool operator()(const K& lhs, const MapPair& rhs) const {
            return compare_(lhs, rhs.key);
        }

        template<class K>
        bool operator()(const MapPair& lhs, const K& rhs) const {
            return compare_(lhs.key, rhs);
        }

    private:
        Compare compare_;
    };

    class Iterator {
        friend Map;
        friend ConstIterator;

    public:
        // The following code is written for the C++ type_traits library.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using value_type        = MapPair;
        using pointer           = MapPair*;
        using reference         = MapPair&;
        using iterator_category = std::output_iterator_tag;

        Iterator() noexcept = default;
        Iterator(const Iterator &obj) noexcept = default;

        Iterator& operator=(const Iterator& obj) noexcept = default;

        ~Iterator() = default;

        explicit operator MapPair*() const noexcept {
            return &((static_cast<typename RBTree<MapPair, PairCompare, Allocator>::Node*>(treeIterator_))->value);
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator& operator++() {
            ++treeIterator_;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        Iterator& operator--() {
            --treeIterator_;
            return *this;
        }

        bool operator==(const Iterator& rhs)      const noexcept { return this->treeIterator_ == rhs.treeIterator_; }
        bool operator==(const ConstIterator& rhs) const noexcept { return this->treeIterator_ == rhs.treeIterator_; }
        bool operator!=(const Iterator& rhs)      const noexcept { return this->treeIterator_ != rhs.treeIterator_; }
        bool operator!=(const ConstIterator& rhs) const noexcept { return this->treeIterator_ != rhs.treeIterator_; }

        MapPair& operator*()  const { return treeIterator_->value; }
        MapPair* operator->() const { return &(treeIterator_->value); }

    private:
        explicit Iterator(const typename RBTree<MapPair, PairCompare, Allocator>::Iterator& iterator)
        : treeIterator_(iterator) {}

        typename RBTree<MapPair, PairCompare, Allocator>::Iterator treeIterator_;
    };

    class ConstIterator {
        friend Map;
        friend Iterator;

    public:
        // The following code is written for the C++ type_traits library.
        // STL algorithms and containers may use these type_traits (e.g. the following
        // typedef) to work properly.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        using difference_type   = std::ptrdiff_t;
        using value_type        = MapPair;
        using pointer           = MapPair*;
        using reference         = MapPair&;
        using iterator_category = std::output_iterator_tag;

        ConstIterator() noexcept = default;
        ConstIterator(const ConstIterator& obj) noexcept = default;
        ConstIterator(const Iterator& iter) noexcept : treeIterator_(iter.treeIterator_) {}

        ConstIterator& operator=(const ConstIterator& obj) noexcept = default;

        ~ConstIterator() = default;

        explicit operator const MapPair*() const noexcept {
            return &((static_cast<typename RBTree<MapPair, PairCompare, Allocator>::Node const*>(treeIterator_))->value);
        }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        ConstIterator& operator++() {
            ++treeIterator_;
            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator tmp = *this;
            --(*this);
            return tmp;
        }

        ConstIterator& operator--() {
            --treeIterator_;
            return *this;
        }

        bool operator==(const Iterator& rhs)      const noexcept { return this->treeIterator_ == rhs.treeIterator_; }
        bool operator==(const ConstIterator& rhs) const noexcept { return this->treeIterator_ == rhs.treeIterator_; }
        bool operator!=(const Iterator& rhs)      const noexcept { return this->treeIterator_ != rhs.treeIterator_; }
        bool operator!=(const ConstIterator& rhs) const noexcept { return this->treeIterator_ != rhs.treeIterator_; }

        const MapPair& operator*()  const { return treeIterator_->value; }
        const MapPair* operator->() const { return &(treeIterator_->value); }

    private:
        explicit ConstIterator(const typename RBTree<MapPair, PairCompare, Allocator>::ConstIterator& iterator)
            : treeIterator_(iterator) {}

        typename RBTree<MapPair, PairCompare, Allocator>::ConstIterator treeIterator_;
    };

    Map() : tree_() {}
    explicit Map(const Compare& compare) : tree_(PairCompare(compare)) {}
    explicit Map(const Allocator& allocator) : tree_(allocator) {}
    Map(const Compare& compare, const Allocator& allocator)
        : tree_(PairCompare(compare), allocator) {}

    template<class InputIterator>
    Map(InputIterator begin, InputIterator end) : tree_(begin, end) {}

    template<class InputIterator>
    Map(InputIterator begin, InputIterator end, const Compare& compare)
        : tree_(begin, end, PairCompare(compare)) {}

    template<class InputIterator>
    Map(InputIterator begin, InputIterator end,
        const Compare& compare,
        const Allocator& allocator)
        : tree_(begin, end, PairCompare(compare), allocator) {}

    template<class InputIterator>
    Map(InputIterator begin, InputIterator end, const Allocator& allocator)
        : tree_(begin, end, allocator) {}

    Map(const Map& obj) = default;
    Map(const Map& obj, const Allocator& allocator) : tree_(obj, allocator) {}

    Map(Map&& obj) noexcept = default;

    Map(std::initializer_list<MapPair> init) : tree_(init) {}

    Map(std::initializer_list<MapPair> init,
        const Compare& compare) : tree_(init, compare) {}

    Map(std::initializer_list<MapPair> init,
        const Allocator& allocator) : tree_(init, allocator) {}

    Map(std::initializer_list<MapPair> init,
        const Compare& compare,
        const Allocator& allocator) : tree_(init, compare, allocator) {}

    Map& operator=(const Map& other) = default;
    Map& operator=(Map&& other) noexcept = default;

    ~Map() = default;

    /**
     * Access specified element with range checking. A
     * <code>lau::OutOfRange</code> will be thrown if there doesn't exist
     * such node.
     * @param key the input key
     * @return a reference to the value mapped to the input key
     */
    [[nodiscard]] Value& At(const Key& key) {
        auto iter = tree_.Find(key);
        if (iter == tree_.End()) throw OutOfRange();
        return iter->value.value;
    }

    /**
     * Access specified element with range checking. A
     * <code>lau::OutOfRange</code> will be thrown if there doesn't exist
     * such node.
     * @param key the input key
     * @return a constant reference to the value mapped to the input key
     */
    [[nodiscard]] const Value& At(const Key& key) const {
        auto iter = tree_.Find(key);
        if (iter == tree_.ConstEnd()) throw OutOfRange();
        return iter->value.value;
    }

    Value& operator[](const Key& key) {
        auto [iter, result] = tree_.Insert(MapPair(key, Value()));
        return iter->value.value;
    }

    Value& operator[](Key&& key) {
        auto [iter, result] = tree_.Insert(std::move(MapPair(std::move(key), Value())));
        return iter->value.value;
    }

    [[nodiscard]] const Value& operator[](const Key& key) const {
        auto iter = tree_.Find(key);
        if (iter == tree_.ConstEnd()) throw OutOfRange();
        return iter->value.value;
    }

    [[nodiscard]] Iterator Begin() noexcept { return Iterator(tree_.Begin()); }
    [[nodiscard]] ConstIterator Begin() const noexcept { return ConstIterator(tree_.ConstBegin()); }
    [[nodiscard]] Iterator begin() noexcept { return Iterator(tree_.Begin()); }
    [[nodiscard]] ConstIterator begin() const noexcept { return ConstIterator(tree_.ConstBegin()); }
    [[nodiscard]] ConstIterator ConstBegin() const noexcept { return ConstIterator(tree_.ConstBegin()); }

    [[nodiscard]] Iterator End() { return Iterator(tree_.End()); }
    [[nodiscard]] ConstIterator End() const { return ConstIterator(tree_.ConstEnd()); }
    [[nodiscard]] Iterator end() { return Iterator(tree_.End()); }
    [[nodiscard]] ConstIterator end() const { return ConstIterator(tree_.ConstEnd()); }
    [[nodiscard]] ConstIterator ConstEnd() const { return ConstIterator(tree_.ConstEnd()); }

    /**
     * Checks whether the container is empty
     * @return whether the class is empty or not
     */
    [[nodiscard]] bool Empty() const noexcept { return tree_.Empty(); }


    /**
     * Get the number of elements.
     * @return the number of elements
     */
    [[nodiscard]] SizeT Size() const noexcept { return tree_.Size(); }

    /**
     * Get the maximum size of the class.
     * @return the maximum size of the class
     */
    [[nodiscard]] long MaxSize() const noexcept {
        return tree_.MaxSize();
    }

    /**
     * Clear the contents
     * @return the reference to the current class
     */
    Map& Clear() noexcept {
        tree_.Clear();
        return *this;
    }

    /**
     * Insert an element.
     * @param value a key-value pair
     * @return a pair, the first of the pair is the iterator to the new
     * element (or the element that prevented the insertion), the second
     * one is a bool denoting whether the insertion took place.
     */
    Pair<Iterator, bool> Insert(const MapPair& pair) {
        auto [iter, success] = tree_.Insert(pair);
        return Pair<Iterator, bool>(Iterator(iter), success);
    }

    /**
     * Insert an element.
     * @param value a key-value pair
     * @return a pair, the first of the pair is the iterator to the new
     * element (or the element that prevented the insertion), the second
     * one is a bool denoting whether the insertion took place.
     */
    Pair<Iterator, bool> Insert(MapPair&& pair) {
        auto [iter, success] = tree_.Insert(std::move(pair));
        return Pair<Iterator, bool>(Iterator(iter), success);
    }

    /**
     * Insert or assign an element.
     * @param value a key-value pair
     * @return a pair, the first of the pair is the iterator to the new
     * element (or the element that prevented the insertion), the second
     * one is a bool denoting whether the insertion took place.
     */
    Pair<Iterator, bool> InsertOrAssign(const MapPair& pair) {
        auto [iter, success] = tree_.Insert(pair);
        if (!success) iter->value.value = pair.value;
        return Pair<Iterator, bool>(Iterator(iter), success);
    }

    /**
     * Insert or assign an element.
     * @param value a key-value pair
     * @return a pair, the first of the pair is the iterator to the new
     * element (or the element that prevented the insertion), the second
     * one is a bool denoting whether the insertion took place.
     */
    Pair<Iterator, bool> InsertOrAssign(MapPair&& pair) {
        auto [iter, success] = tree_.Insert(std::move(pair));
        if (!success) iter->value.value = std::move(pair.value);
        return Pair<Iterator, bool>(Iterator(iter), success);
    }

    /**
     * Insert an element in place.
     * @param value a key-value pair
     * @return a pair, the first of the pair is the iterator to the new
     * element (or the element that prevented the insertion), the second
     * one is a bool denoting whether the insertion took place.
     */
    template<class... Args>
    Pair<Iterator, bool> Emplace(Args&&... args) {
        auto [iter, success] = tree_.Emplace(std::forward<Args>(args)...);
        return Pair<Iterator, bool>(Iterator(iter), success);
    }

    /**
     * Erase the element at position.  A <code>lau::InvalidIterator</code>
     * will be thrown if the iterator doesn't belong to this map or the
     * iterator is the end iterator.
     * @param position the node to be erased
     * @return the reference to the current class
     */
    Map& Erase(const Iterator& position) {
        tree_.Erase(position.treeIterator_);
        return *this;
    }

    /**
     * Erase the element whose key is equal to the input key.  A
     * <code>lau::InvalidArgument</code> will be thrown if there doesn't
     * exist such node.
     * @param key the key to be erased
     * @return the reference to the current class
     */
    Map& Erase(const Key& key) {
        tree_.Erase(key);
        return *this;
    }

    /**
     * Erase the element whose key is equal to the input key.  A
     * <code>lau::InvalidArgument</code> will be thrown if there doesn't
     * exist such node.  Please note that the type K must have valid
     * <code>Compare::is_transparent</code>.
     * @param key the key to be erased
     * @return the reference to the current class
     */
    template<class K>
    Map& Erase(const K& key) {
        tree_.Erase(key);
        return *this;
    }

    /**
     * Count the number of element whose key is equal to the input key.  Since
     * the keys must be different from each other, the return value can only
     * be either 1 or 0.
     * @param key the input key
     * @return the number of elements with key
     */
    [[nodiscard]] SizeT Count(const Key& key) const {
        return tree_.Contains(MapPair(key, Value())) ? 1 : 0;
    }

    /**
     * Count the number of element whose key is equal to the input key.  Since
     * the keys must be different from each other, the return value can only
     * be either 1 or 0.  Please note that the type K must have valid
     * <code>Compare::is_transparent</code>.
     * @param key the input key
     * @return the number of elements with key
     */
    template<class K>
    [[nodiscard]] SizeT Count(const K& key) const {
        return tree_.Contains(key) ? 1 : 0;
    }

    /**
     * Check whether this container contains the element whose key is equal to
     * the input key.
     * @param key the input key
     * @return whether this container contains the element whose key is equal
     * to the input key
     */
    [[nodiscard]] bool Contains(const Key& key) const {
        return tree_.Contains(MapPair(key, Value()));
    }

    /**
     * Check whether this container contains the element whose key is equal to
     * the input key.  Please note that the type K must have valid
     * <code>Compare::is_transparent</code>.
     * @param key the input key
     * @return whether this container contains the element whose key is equal
     * to the input key
     */
    template<class K>
    [[nodiscard]] bool Contains(const K& key) const {
        return tree_.Contains(key);
    }

    /**
     * Find the element whose key is equal to the input key.  If there doesn't
     * exist such element, the end iterator will be return.
     * @param key the input key
     * @return the iterator of the found element, or the end iterator if not
     * exists
     */
    [[nodiscard]] Iterator Find(const Key& key) {
        return Iterator(tree_.Find(key));
    }

    /**
     * Find the element whose key is equal to the input key.  If there doesn't
     * exist such element, the end iterator will be return.  Please note that
     * the type K must have valid <code>Compare::is_transparent</code>.
     * @param key the input key
     * @return the iterator of the found element, or the end iterator if not
     * exists
     */
    template<class K>
    [[nodiscard]] Iterator Find(const K& key) {
        return Iterator(tree_.Find(key));
    }

    /**
     * Find the element whose key is equal to the input key.  If there doesn't
     * exist such element, the end constant iterator will be return.
     * @param key the input key
     * @return the constant iterator of the found element, or the end constant
     * iterator if not found
     */
    ConstIterator Find(const Key& key) const {
        return ConstIterator(tree_.Find(MapPair(key, Value())));
    }

    /**
     * Find the element whose key is equal to the input key.  If there doesn't
     * exist such element, the end constant iterator will be return.  Please
     * note that the type K must have valid
     * <code>Compare::is_transparent</code>.
     * @param key the input key
     * @return the constant iterator of the found element, or the end constant
     * iterator if not found
     */
    template<class K>
    ConstIterator Find(const K& key) const {
        return ConstIterator(tree_.Find(key));
    }

    /**
     * Find the first element whose key is no less than the input key.  If
     * there doesn't exist such element, the end iterator will be return.
     * @param key
     * @return the iterator of the first element whose key is no less than
     * the input key, or the end iterator if not exists
     */
    [[nodiscard]] Iterator LowerBound(const Key& key) {
        return Iterator(tree_.LowerBound(key));
    }

    /**
     * Find the first element whose key is no less than the input key.  If
     * there doesn't exist such element, the const end iterator will be return.
     * @param key
     * @return the const iterator of the first element whose key is no less
     * than the input key, or the end iterator if not exists
     */
    [[nodiscard]] ConstIterator LowerBound(const Key& key) const {
        return ConstIterator(tree_.LowerBound(key));
    }

    /**
     * Find the first element whose key is no less than the input key.  If
     * there doesn't exist such element, the end iterator will be return.
     * Please note that the type K must have valid
     * <code>Compare::is_transparent</code>.
     * @param key
     * @return the iterator of the first element whose key is no less
     * than the input key, or the end iterator if not exists
     */
    template<class K>
    [[nodiscard]] Iterator LowerBound(const K& key) {
        return Iterator(tree_.LowerBound(key));
    }

    /**
     * Find the first element whose key is no less than the input key.  If
     * there doesn't exist such element, the const end iterator will be return.
     * Please note that the type K must have valid
     * <code>Compare::is_transparent</code>.
     * @param key
     * @return the const iterator of the first element whose key is no less
     * than the input key, or the end iterator if not exists
     */
    template<class K>
    [[nodiscard]] ConstIterator LowerBound(const K& key) const {
        return ConstIterator(tree_.LowerBound(key));
    }

    /**
     * Find the first element whose key is greater than the input key.  If
     * there doesn't exist such element, the end iterator will be return.
     * @param key
     * @return the iterator of the first element whose key is no less than
     * the input key, or the end iterator if not exists
     */
    [[nodiscard]] Iterator UpperBound(const Key& key) {
        return Iterator(tree_.UpperBound(key));
    }

    /**
     * Find the first element whose key is greater than the input key.  If
     * there doesn't exist such element, the const end iterator will be return.
     * @param key
     * @return the const iterator of the first element whose key is no less
     * than the input key, or the end iterator if not exists
     */
    [[nodiscard]] ConstIterator UpperBound(const Key& key) const {
        return ConstIterator(tree_.UpperBound(key));
    }

    /**
     * Find the first element whose key is greater than the input key.  If
     * there doesn't exist such element, the end iterator will be return.
     * Please note that the type K must have valid
     * <code>Compare::is_transparent</code>.
     * @param key
     * @return the iterator of the first element whose key is no less than
     * the input key, or the end iterator if not exists
     */
    template<class K>
    [[nodiscard]] Iterator UpperBound(const K& key) {
        return Iterator(tree_.UpperBound(key));
    }

    /**
     * Find the first element whose key is greater than the input key.  If
     * there doesn't exist such element, the end iterator will be return.
     * Please note that the type K must have valid
     * <code>Compare::is_transparent</code>.
     * @param key
     * @return the iterator of the first element whose key is no less
     * than the input key, or the end iterator if not exists
     */
    template<class K>
    [[nodiscard]] ConstIterator UpperBound(const K& key) const {
        return ConstIterator(tree_.UpperBound(key));
    }

    /**
     * Get a copy of the allocator.  Please note that the allocator type is
     * not the type used to store the elements.
     * @return the copy of the allocator
     */
    [[nodiscard]] Allocator GetAllocator() const {
        return Allocator(tree_.GetAllocator());
    }

    /**
     * Get a copy of the original compare class for the key.
     * @return a copy of the original compare class
     */
    [[nodiscard]] Compare GetKeyCompare() const {
        return tree_.GetCompare().compare_;
    }

    /**
     * Get a copy of the compare class for key-value pair.  Please note that
     * this compare class is the real class used in the map class.
     * @return a copy of the compare class for key-value pair
     */
    [[nodiscard]] PairCompare GetPairCompare() const {
        return tree_.GetCompare();
    }

    /**
     * Swap the contents of two map.
     * @param other
     * @return the reference to the current class
     */
    Map& Swap(Map& other) noexcept {
        tree_.Swap(other.tree_);
    }

private:
    RBTree<MapPair, PairCompare, Allocator> tree_;
};

template<class Key, class Value, class Compare, class Allocator>
void Swap(Map<Key, Value, Compare, Allocator>& lhs,
          Map<Key, Value, Compare, Allocator>& rhs) noexcept {
    lhs.Swap(rhs);
}

} // namespace lau

#endif // LAU_CPP_LIB_LAU_MAP_H
