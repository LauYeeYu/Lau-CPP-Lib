# lau::Map

Switch to Other Languages: [繁體中文（香港）](map_zh.md)

Included in header `lau/map.h`.

```c++
namespace lau {
template<class Key,
         class Value,
         class Compare   = std::less<Key>,
         class Allocator = std::allocator<KeyValuePair<const Key, Value>>>
class Map;
} // namespace lau
```

`lau::Map` is a mapping container that uses a
[red-black tree](https://en.wikipedia.org/wiki/Red-black_tree) as its
underlying data structure.

A customized compare function can be supplied to compare the elements.  Note
that the `Compare` must have an overloaded
`bool operator(const T&, const T&) const` function.  It must also guarantee
that the order of the keys is preserved.

Note: this class is not a raw data structure.  You cannot modify the keys as
long as it is inserted.  If you want to modify the key, you may erase the
pair or use the `lau::RBTree` class instead.  The `lau::RBTree` class is
a data structure that allows you to modify the key.

For more information about the data structure, please go to the
[detail of data structure](#DataStructure).

## Overview
```c++
namespace lau {
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

    class PairCompare;

    // Constructors
    Map();
    explicit Map(const Compare& compare);
    explicit Map(const Allocator& allocator);
    Map(const Compare& compare, const Allocator& allocator);
    template<class InputIterator>
    Map(InputIterator begin, InputIterator end);
    template<class InputIterator>
    Map(InputIterator begin, InputIterator end, const Compare& compare);
    template<class InputIterator>
    Map(InputIterator begin, InputIterator end, const Allocator& allocator);
    template<class InputIterator>
    Map(InputIterator begin, InputIterator end,
        const Compare& compare,
        const Allocator& allocator);
    Map(const Map& obj);
    Map(const Map& obj, const Allocator& allocator);
    Map(Map&& obj) noexcept;
    Map(std::initializer_list<MapPair> init);
    Map(std::initializer_list<MapPair> init, const Compare& compare);
    Map(std::initializer_list<MapPair> init, const Allocator& allocator);
    Map(std::initializer_list<MapPair> init,
        const Compare& compare,
        const Allocator& allocator);

    // Assignment Operators
    Map& operator=(const Map& other);
    Map& operator=(Map&& other) noexcept;

    // Destructor
    ~Map() = default;

    // Element Access
    [[nodiscard]] Value& At(const Key& key);
    [[nodiscard]] const Value& At(const Key& key) const;
    Value& operator[](const Key& key);
    Value& operator[](Key&& key);
    [[nodiscard]] const Value& operator[](const Key& key) const;
    
    // Modifiers
    Pair<Iterator, bool> Insert(const MapPair& pair);
    Pair<Iterator, bool> Insert(MapPair&& pair);
    Pair<Iterator, bool> InsertOrAssign(const MapPair& pair);
    Pair<Iterator, bool> InsertOrAssign(MapPair&& pair);
    template<class... Args>
    Pair<Iterator, bool> Emplace(Args&&... args);
    Map& Erase(const Iterator& position);
    Map& Erase(const Key& key);
    template<class K>
    Map& Erase(const K& key);
    Map& Clear() noexcept;
    Map& Swap(Map& other) noexcept;

    // Capacity
    [[nodiscard]] SizeT Size() const noexcept;
    [[nodiscard]] bool Empty() const noexcept;
    [[nodiscard]] long MaxSize() const noexcept;

    // Getters
    [[nodiscard]] Allocator GetAllocator() const noexcept;
    [[nodiscard]] Compare GetKeyCompare() const;
    [[nodiscard]] PairCompare GetPairCompare() const;

    // Lookup
    [[nodiscard]] SizeT Count(const Key& key) const;
    template<class K>
    [[nodiscard]] SizeT Count(const K& key) const;
    [[nodiscard]] bool Contains(const Key& key) const;
    template<class K>
    [[nodiscard]] bool Contains(const K& key) const;
    [[nodiscard]] Iterator Find(const Key& key);
    template<class K>
    [[nodiscard]] Iterator Find(const K& key);
    [[nodiscard]] ConstIterator Find(const Key& key) const;
    template<class K>
    [[nodiscard]] ConstIterator Find(const K& key) const;
    [[nodiscard]] Iterator LowerBound(const Key& key);
    [[nodiscard]] ConstIterator LowerBound(const Key& key) const;
    template<class K>
    [[nodiscard]] Iterator LowerBound(const K& key);
    template<class K>
    [[nodiscard]] ConstIterator LowerBound(const K& key) const;
    [[nodiscard]] Iterator UpperBound(const Key& key);
    [[nodiscard]] ConstIterator UpperBound(const Key& key) const;
    template<class K>
    [[nodiscard]] Iterator UpperBound(const K& key);
    template<class K>
    [[nodiscard]] ConstIterator UpperBound(const K& key) const;

    // Iterators
    [[nodiscard]] Iterator Begin() noexcept;
    [[nodiscard]] ConstIterator Begin() const noexcept;
    [[nodiscard]] Iterator begin() noexcept;
    [[nodiscard]] ConstIterator begin() const;
    [[nodiscard]] ConstIterator ConstBegin() const noexcept;
    [[nodiscard]] Iterator End() noexcept;
    [[nodiscard]] ConstIterator End() const noexcept;
    [[nodiscard]] Iterator end() noexcept;
    [[nodiscard]] ConstIterator end() const noexcept;
    [[nodiscard]] ConstIterator ConstEnd() const;
};

// Non-member Function
template<class Key, class Value, class Compare, class Allocator>
void Swap(Map<Key, Value, Compare, Allocator>& lhs,
          Map<Key, Value, Compare, Allocator>& rhs) noexcept;
} // namespace lau
```
## Template
- `Key`: the type of the key
- `Value`: the type of the value
- `Compare`: the type of compare function (`std::less<Key>` is default)
  - need to satisfy the C++ comparing class requirements (see
    [C++ named requirements: Compare](https://en.cppreference.com/w/cpp/named_req/Compare))
    - `bool Compare(const T& lhs, const T& rhs)` must be available.
    - If `Compare(a, b) = false` and `Compare(a, b) = false`, or
      `Compare(a, b) = true` and `Compare(a, b) = true`, `a` must be the
      same as `b`. (Uniqueness)
- `Allocator`: the type of allocator
  - need to satisfy the C++ allocator requirements (see
    [C++ named requirements: Allocator](https://en.cppreference.com/w/cpp/named_req/Allocator))
  - `std::allocator<KeyValuePair<const Key, Value>>` is default.

## Member Types
- `MapPair`: `KeyValuePair<const Key, Value>` 
- `value_type`: `KeyValuePair<const Key, Value>`

- `PairCompare`: the type to compare the pair
```c++
class PairCompare {
    public:
        PairCompare() = default;
        explicit PairCompare(const Compare& compareIn) : compare_(compareIn) {}

        bool operator()(const MapPair& lhs, const MapPair& rhs) const;
        template<class K>
        bool operator()(const K& lhs, const MapPair& rhs) const;
        template<class K>
        bool operator()(const MapPair& lhs, const K& rhs) const;
    };
```
- [`Iterator`](#iterator): The type of iterator in `lau::Map`
- [`ConstIterator`](#iterator): the type of constant iterator in `lau::Map`

## Member Functions
- [(constructors)](#Constructors)
- [`operator=`](#operator=)
- (destructors)

### Modifiers
- [`Insert`](#Insert): insert a new element to the tree
- [`Emplace`](#Emplace): insert a new element to the tree in place
- [`Erase`](#Erase): erase an element from the tree
- [`Clear`](#Clear): clear the tree
- [`Swap`](#Swap): swap the tree

### Capacity
- [`Size`](#Size): get the size of the tree
- [`Empty`](#Empty): tell whether the tree is empty
- [`MaxSize`](#MaxSize): get the maximum size of the tree

### Getter
- [`GetAllocator`](#GetAllocator): get the allocator
- [`GetKeyCompare`](#GetKeyCompare): get the compare class for the key
- [`GetPairCompare`](#GetPairCompare): get the compare class for the map pair

### Lookup
- [`Contains`](#Contains): tell whether the tree contains the element
- [`Find`](#Find): find the element in the tree
- [`LowerBound`](#LowerBound): find the first element that is not less than the
  given element
- [`UpperBound`](#UpperBound): find the first element that is greater than
  the given element

### Iterators
- [`Begin`](#Begin): get the iterator to the first element
- [`End`](#End): get the iterator to the end of the tree
- [`begin`](#begin): get the iterator to the first element
- [`end`](#end): get the iterator to the end of the tree
- [`ConstBegin`](#ConstBegin): get the constant iterator to the first element
- [`ConstEnd`](#ConstEnd): get the constant iterator to the end of the tree

## Non-member Function
- [`Swap`](#SwapNonmember): swap the content

## Details
### <span id="DataStructure">Data Structure</span>
The class uses the data structure called the red-black tree.  The red-black
tree is a kind of self-balancing binary search tree.  It uses a special
strategy to reduce the frequency of re-balancing.  By just guaranteeing
that the height of the left subtree of each node is at least half and at
most twice of the height of the right subtree, the frequency of re-balancing
is reduced to half of the AVL tree.  For more information, you may view the
[wikipedia page](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree).

Complexity:
- Access: $O(\log n)$
- Insert: $O(\log n)$
- Remove: $O(\log n)$
- Traverse: $O(n)$

### <span id="Constructors">Constructors</span>
```c++
Map();
```
- Default constructor
- Construct an empty class with default compare class and the default allocator.

```c++
explicit Map(const Compare& compare);
```
- Construct an empty class with a customized compare class and the default allocator.

```c++
explicit Map(const Allocator& allocator);
```
- Construct an empty class with the default compare class and a customized allocator.

```c++
Map(const Compare& compare, const Allocator& allocator);
```
- Construct an empty class with a customized compare class, a customized allocator.

```c++
template<class InputIterator>
Map(InputIterator begin, InputIterator end);
```
- Construct a class whose elements are among the range `[begin, end)`
  with the default compare class, and the default allocator.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  range)

```c++
template<class InputIterator>
Map(InputIterator begin, InputIterator end, const Compare& compare);
```
- Construct a class whose elements are among the range `[begin, end)`
  with a customized compare class, and the default allocator.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  range)

```c++
template<class InputIterator>
Map(InputIterator begin, InputIterator end, const Allocator& allocator);
```
- Construct a class whose elements are among the range `[begin, end)`
  with the default compare class, and a customized allocator.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  range)

```c++
template<class InputIterator>
Map(InputIterator begin, InputIterator end,
    const Compare& compare,
    const Allocator& allocator);
```
- Construct a class whose elements are among the range `[begin, end)`
  with a customized compare class, and a customized allocator.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  range)

```c++
Map(const Map& obj);
```
- Copy constructor
- Construct a class with the same elements, allocator and compare class
  as `obj`.
- Time complexity: $O(n)$. ($n$ is the number of elements in the
  `obj` class)

```c++
Map(const Map& obj, const Allocator& allocator);
```
- Copy constructor with customized allocator
- Construct a class with the same elements and compare class as `obj`.
- The allocator of the new class copied from `allocator`.
- Time complexity: $O(n)$. ($n$ is the number of elements in the
  `obj` class)

```c++
Map(Map&& obj) noexcept;
```
- Move constructor
- Construct a class with the same elements, allocator and compare class
  as `obj`.
- Time complexity: $O(1)$.

```c++
Map(std::initializer_list<MapPair> init);
```
- Construct a class with the elements in the initializer list `init`.
- The allocator and the compare class are constructed with their default
  constructor.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  initializer list)

```c++
Map(std::initializer_list<MapPair> init, const Compare& compare);
```
- Construct a class with the elements in the initializer list `init`
  and a customized compare class.
- The allocator is constructed with its default constructor.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  initializer list)

```c++
Map(std::initializer_list<MapPair> init, const Allocator& allocator);
```
- Construct a class with the elements in the initializer list `init`
  and a customized allocator.
- The compare class is constructed with its default constructor.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  initializer list)

```c++
Map(std::initializer_list<MapPair> init,
    const Compare& compare,
    const Allocator& allocator);
```
- Construct a class with the elements in the initializer list `init`
  and a customized compare class and a customized allocator.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  initializer list)

### <span id="operator=">operator=</span>
```c++
Map& operator=(const Map& obj);
```
- Assign the class with the `obj` class.
- Time complexity: $O(n)$. ($n$ is the number of elements in the
  `obj` class)

```c++
Map& operator=(Map&& obj) noexcept;
```
- Assign the class with the `obj` class.
- Time complexity: $O(1)$.

### <span id="Insert">`Insert`</span>
```c++
Pair<Iterator, bool> Insert(const MapPair& pair);
```
- Insert a new element with `pair`.
- Return a pair of `Iterator` and a `bool`.  If such pair doesn't exist,
  the first one is the iterator of the new node, and the second one is
  true; if such node exists, the first one is the iterator of the node
  that prevents this insertion, and the second one is false.
- Time complexity: $O(\log n)$.

```c++
Pair<Iterator, bool> Insert(MapPair&& pair);
```
- Insert a new element with the right value `pair`.
- Return a pair of `Iterator` and a `bool`.  If such node doesn't exist,
  the first one is the iterator of the new node, and the second one is
  true; if such node exists, the first one is the iterator of the node
  that prevents this insertion, and the second one is false.
- Time complexity: $O(\log n)$.

### <span id="Emplace">`Emplace`</span>
```c++
template<class... Args>
Pair<Iterator, bool> Emplace(Args&&... args);
```
- Insert a new element in place with parameters `args...`.
- Return a pair of `Iterator` and a `bool`.  If such node doesn't exist,
  the first one is the iterator of the new node, and the second one is
  true; if such node exists, the first one is the iterator of the node
  that prevents this insertion, and the second one is false.
- Time complexity: $O(\log n)$.

### <span id="Erase">`Erase`</span>
```c++
Map& Erase(const Iterator& position);
```
- Erase the element at `position`.
- Throw `lau::InvalidIterator` if the iterator doesn't belong to this class
  or the iterator is the end iterator.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(\log n)$ (on average).

```c++
Map& Erase(const Key& key);
```
- Erase the element that is equal to `key`.
- Throw `lau::InvalidArgument` if the `key` doesn't belong to this class.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(\log n)$.

```c++
template<class K>
Map& Erase(const K& key);
```
- Erase the element that is equal to `key`.
- Throw `lau::InvalidArgument` if the `key` doesn't belong to this class.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(\log n)$.

### <span id="Clear">`Clear`</span>
```c++
Map& Clear() noexcept;
```
- Erase all the elements in the class.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(n)$.

### <span id="Swap">`Swap`</span>
```c++
Map& Swap(Map& other) noexcept;
```
- Swap the contents of the two classes.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(1)$.

### <span id="Size">`Size`</span>
```c++
[[nodiscard]] SizeT Size() const noexcept;
```
- Get the number of elements in the class.
- Time complexity: $O(1)$.

### <span id="Empty">`Empty`</span>
```c++
[[nodiscard]] bool Empty() const noexcept;
```
- Check whether the tree is empty.
- Time complexity: $O(1)$.

### <span id="MaxSize">`MaxSize`</span>
```c++
[[nodiscard]] long MaxSize() const noexcept;
``` 
- Get the maximum number of elements that can be stored in the tree.

### <span id="GetAllocator">`GetAllocator`</span>
```c++
[[nodiscard]] Allocator GetAllocator() const noexcept;
```
- Get a copy of the allocator.

### <span id="GetKeyCompare">`GetKeyCompare`</span>
```c++
[[nodiscard]] Compare GetKeyCompare() const;
```
- Get a copy of the compare class for the key.

### <span id="GetPairCompare">`GetPairCompare`</span>
```c++
[[nodiscard]] PairCompare GetPairCompare() const;
```
- Get a copy of the compare class for the pair.

### <span id="Contains">`Contains`</span>
```c++
[[nodiscard]] bool Contains(const Key& key) const;
```
- Check whether the class contains an element that is equal to `key`.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] bool Contains(const K& key) const;
```
- Check whether the class contains an element that is equal to `key`.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

### <span id="Find">`Find`</span>
```c++
[[nodiscard]] Iterator Find(const Key& key);
```
- Find the element that is equal to `key`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Time complexity: $O(\log n)$.

```c++
[[nodiscard]] ConstIterator Find(const Key& key) const;
```
- Find the element that is equal to `key`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] Iterator Find(const K& key);
```
- Find the element that is equal to `key`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] ConstIterator Find(const K& key) const;
```
- Find the element that is equal to `key`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

### <span id="LowerBound">`LowerBound`</span>
```c++
[[nodiscard]] Iterator LowerBound(const Key& key);
```
- Find the first element that no less than `key`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Time complexity: $O(\log n)$.

```c++
[[nodiscard]] ConstIterator LowerBound(const Key& key) const;
```
- Find the first element that no less than `key`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] Iterator LowerBound(const K& key);
```
- Find the first element that no less than `key`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] ConstIterator LowerBound(const K& key) const;
```
- Find the first element that no less than `key`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

### <span id="UpperBound">`UpperBound`</span>
```c++
[[nodiscard]] Iterator UpperBound(const Key& key);
```
- Find the first element that greater than `key`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Time complexity: $O(\log n)$.

```c++
[[nodiscard]] ConstIterator UpperBound(const Key& key) const;
```
- Find the first element that greater than `key`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] Iterator UpperBound(const K& key);
```
- Find the first element that greater than `key`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] ConstIterator UpperBound(const K& key) const;
```
- Find the first element that greater than `key`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

### <span id="Begin">`Begin`</span>
```c++
[[nodiscard]] Iterator Begin() noexcept;
```
- Get the iterator of the first element.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstIterator Begin() const noexcept;
```
- Get the constant iterator of the first element.
- Time complexity: $O(1)$.

### <span id="begin">`begin`</span>
```c++
[[nodiscard]] Iterator begin() noexcept;
```
- Get the iterator of the first element.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstIterator begin() const;
```
- Get the constant iterator of the first element.
- Time complexity: $O(1)$.

### <span id="ConstBegin">`ConstBegin`</span>
```c++
[[nodiscard]] ConstIterator ConstBegin() const noexcept;
```
- Get the constant iterator of the first element.
- Time complexity: $O(1)$.

### <span id="End">`End`</span>
```c++
[[nodiscard]] Iterator End() noexcept;
```
- Get the end iterator.
- Please note that the end iterator is pointing at the node after the last
  element.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstIterator End() const noexcept;
```
- Get the end constant iterator.
- Please note that the end constant iterator is pointing at the node after the
  last element.
- Time complexity: $O(1)$.

### <span id="end">`end`</span>
```c++
[[nodiscard]] Iterator end() noexcept;
```
- Get the end iterator.
- Please note that the end iterator is pointing at the node after the last
  element.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstIterator end() const noexcept;
```
- Get the end constant iterator.
- Please note that the end constant iterator is pointing at the node after the
  last element.
- Time complexity: $O(1)$.

### <span id="ConstEnd">`ConstEnd`</span>
```c++
[[nodiscard]] ConstIterator ConstEnd() const;
```
- Get the end constant iterator.
- Please note that the end constant iterator is pointing at the node after the
  last element.
- Time complexity: $O(1)$.

### <span id="SwapNonmember">`Swap`</span>
```c++
template<class Key, class Value, class Compare, class Allocator>
void Swap(Map<Key, Value, Compare, Allocator>& lhs,
          Map<Key, Value, Compare, Allocator>& rhs) noexcept;
```
- Swap the contents of two `RBTree`s.
- Time complexity: $O(1)$.

### <span id="iterator">iterator</span>
Types:
- `difference_type`: `std::ptrdiff_t`
- `value_type`: `MapPair`
- `pointer`: `MapPair*`
- `reference`: `MapPair&`
- `iterator_category`: `std::output_iterator_tag`

Supported Operation: (The following iterators can be either `Iterator` or
`ConstIterator`)
- `iterator++`
- `++iterator`
- `iterator--`
- `--iterator`
- `*iterator`
- `iterator->...`
- `iterator1 == iterator2`
- `iterator1 != iterator2`

Invalidate when:
- Erase: the iterator pointing to the erased element
- Clear: all the iterators except the end iterator
