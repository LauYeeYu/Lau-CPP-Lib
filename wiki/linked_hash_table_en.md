# lau::LinkedHashTable

Switch to Other Languages: [繁體中文（香港）](linked_hash_table_zh.md)

Included in header `lau/linked_hash_table.h`.

```c++
namespace lau {
template <class T,
          class Hash = std::hash<T>,
          class Equal = std::equal_to<T>,
          class Allocator = std::allocator<T>>
class LinkedHashTable;
} // namespace lau
```

`lau::LinkedHashTable` is an implementation of hash table which is linked
by the order of insertion.  The class uses the
[rehash policy class](rehash_policy_en.md) to determine when to rehash.

A customized hash function can be supplied to compare the elements.  Note
that the `Compare` must have an overloaded
`std::size_t operator(const T&) const` function.

Note: this class is an implementation of the data structure, so it gives
users the freedom to modify most of the data by using the iterator.  As long
as you didn't change the hash of the element, or modify the class structure,
there is no risk.  If you want to use a mapping class, you may use the
`lau::LinkedHashMap` instead.

For more information about the data structure, please go to the
[detail of data structure](#DataStructure).

## Overview
```c++
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

    // Constructors
    explicit LinkedHashTable(const Allocator& allocator = Allocator());
    explicit LinkedHashTable(const Hash& hash,
                             const Equal& equal = Equal(),
                             const Allocator& allocator = Allocator());
    explicit LinkedHashTable(SizeT minimumBucketSize,
                             const Hash& hash = Hash(),
                             const Equal& equal = Equal(),
                             const Allocator& allocator = Allocator());
    template<class InputIt>
    LinkedHashTable(InputIt first, InputIt last,
                    const Hash& hash = Hash(),
                    const Equal& equal = Equal(),
                    const Allocator& allocator = Allocator());
    template<class InputIt>
    LinkedHashTable(InputIt first, InputIt last,
                    SizeT minimumBucketSize,
                    const Hash& hash = Hash(),
                    const Equal& equal = Equal(),
                    const Allocator& allocator = Allocator());
    LinkedHashTable(const LinkedHashTable& obj);
    LinkedHashTable(const LinkedHashTable& obj, const Allocator& allocator);
    LinkedHashTable(LinkedHashTable&& obj) noexcept;
    LinkedHashTable(std::initializer_list<T> init,
                    const Hash& hash,
                    const Equal& equal = Equal(),
                    const Allocator& allocator = Allocator());
    LinkedHashTable(std::initializer_list<T> init,
                    const Allocator& allocator = Allocator());

    // Assignment Operators
    LinkedHashTable& operator=(const LinkedHashTable& obj);
    LinkedHashTable& operator=(LinkedHashTable&& obj) noexcept;

    // Destructor
    ~LinkedHashTable();

    // Modifiers
    Pair<Iterator, bool> Insert(const T& value);
    Pair<Iterator, bool> Insert(T&& value);
    template<class... Args>
    Pair<Iterator, bool> Emplace(Args&&... args);
    LinkedHashTable& Erase(const Iterator& position);
    LinkedHashTable& Erase(const T& value);
    template<class K>
    LinkedHashTable& Erase(const K& value);
    LinkedHashTable& Clear() noexcept;
    LinkedHashTable& Swap(LinkedHashTable& other) noexcept;
    LinkedHashTable& Merge(LinkedHashTable& other);

    // Capacity
    [[nodiscard]] SizeT Size() const noexcept;
    [[nodiscard]] bool Empty() const noexcept;
    [[nodiscard]] SizeT BucketCount() const noexcept;
    LinkedHashTable& ReserveAtLeast(SizeT minimumSize) noexcept;

    // Getters
    [[nodiscard]] Hash GetHash() const;
    [[nodiscard]] Equal GetEqual() const;
    [[nodiscard]] NodeAllocatorType GetNodeAllocator() const;
    [[nodiscard]] BucketAllocatorType GetBucketAllocator() const;

    // Lookup
    [[nodiscard]] Iterator Find(const T& value);
    template<class K>
    [[nodiscard]] Iterator Find(const K& value);
    [[nodiscard]] ConstIterator Find(const T& value) const;
    template<class K>
    [[nodiscard]] ConstIterator Find(const K& value) const;
    [[nodiscard]] bool Contains(const T& value) const;
    template<class K>
    [[nodiscard]] bool Contains(const K& value) const;

    // Iterators
    [[nodiscard]] Iterator Begin() noexcept;
    [[nodiscard]] ConstIterator Begin() const noexcept;
    [[nodiscard]] BucketIterator Begin(SizeT bucketIndex);
    [[nodiscard]] ConstBucketIterator Begin(SizeT bucketIndex) const;
    [[nodiscard]] Iterator begin() noexcept;
    [[nodiscard]] ConstIterator begin() const;
    [[nodiscard]] ConstIterator ConstBegin() const noexcept;
    [[nodiscard]] ConstBucketIterator ConstBegin(SizeT bucketIndex) const;
    [[nodiscard]] Iterator End() noexcept;
    [[nodiscard]] ConstIterator End() const noexcept;
    [[nodiscard]] BucketIterator End(SizeT bucketIndex);
    [[nodiscard]] ConstBucketIterator End(SizeT bucketIndex) const;
    [[nodiscard]] Iterator end() noexcept;
    [[nodiscard]] ConstIterator end() const noexcept;
    [[nodiscard]] ConstIterator ConstEnd() const;
    [[nodiscard]] ConstBucketIterator ConstEnd(SizeT bucketIndex) const;
};

// Non-member Function
template<class T, class Hash, class Equal, class Allocator>
void Swap(LinkedHashTable<T, Hash, Equal, Allocator>& lhs,
          LinkedHashTable<T, Hash, Equal, Allocator>& rhs) noexcept;
} // namespace lau
```
## Template
- `T`: the type of element
- `Hash`: the type of hash function (`std::hash<T>` is default)
  - need to satisfy the C++ hash class requirements (see
    [C++ named requirements: Hash](https://en.cppreference.com/w/cpp/named_req/Hash))
    - `std::size_t Hash(const T&)` must be available.
- `Hash`: the type of equality comparing function (`std::hash<T>` is default)
  - need to satisfy the C++ equality comparing class requirements (see
    [C++ named requirements: EqualityComparable](https://en.cppreference.com/w/cpp/named_req/EqualityComparable))
    - `bool Equal(const T& lhs, const T& rhs)` must be available.
    - The function above must return `true` if `lhs == rhs`, and return
      `false` if `lhs != rhs`.
- `Allocator`: the type of allocator
  - need to satisfy the C++ allocator requirements (see
    [C++ named requirements: Allocator](https://en.cppreference.com/w/cpp/named_req/Allocator))
  - `std::allocator<T>` is default.

## Member Types
- `Node`: the node in the hash table
  - You may use the interface to have a view of the structure of the table.
  - Every time you change the value, you should also change the hash.
  - `Node* BucketNext() const noexcept` returns the pointer to the next node
    in the same bucket.
  - `Node* LinkedNext() const noexcept` returns the pointer to the next node
    in the linked list.
  - `Node* LinkedPrev() const noexcept` returns the pointer to the previous
    node in the linked list.
  - The code structure is listed below.
```c++
struct Node {
public:
    Node() = default;
    Node(Node&) = default;
    Node(const Node&) = default;
    Node(Node&&) = default;

    Node(std::size_t hashIn, const T& valueIn) : value(valueIn), hash(hashIn) {}
    Node(std::size_t hashIn, T&& valueIn) : value(std::move(valueIn)), hash(hashIn) {}

    template<class... Args>
    explicit Node(std::size_t hashIn, Args&&... args) : value(std::forward<Args>(args)...), hash(hashIn) {}

    Node& operator=(Node&) = default;
    Node& operator=(const Node&) = default;
    Node& operator=(Node&&) = default;

    ~Node() = default;

    Node* BucketNext() const noexcept;
    Node* LinkedNext() const noexcept;
    Node* LinkedPrevious() const noexcept;

    T value;
    std::size_t hash = 0;
};
```
- [`Iterator`](#iterator): The type of iterator in `lau::LinkedHashTable`
- [`ConstIterator`](#iterator): the type of constant iterator in
  `lau::LinkedHashTable`
- [`BucketIterator`](#bucket-iterator): the type of bucket iterator in `lau::LinkedHashTable`
- [`ConstBucketIterator`](#bucket-iterator): the type of constant bucket
  iterator in `lau::LinkedHashTable`
- `NodeAllocatorType`: the type of allocator for `Node`
- `BucketAllocatorType`: the type of allocator for the bucket

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
- [`BucketCount`](#BucketCount): get the number of buckets
- [`ReserveAtLeast`](#ReserveAtLeast): reserve enough buckets

### Getter
- [`GetHash`](#GetHash): get the hash function
- [`GetEqual`](#GetEqual): get the equality comparing function
- [`GetBucketAllocator`](#GetBucketAllocator): get the allocator for buckets
- [`GetNodeAllocator`](#GetNodeAllocator): get the allocator for nodes

### Lookup
- [`Contains`](#Contains): tell whether the class contains the element
- [`Find`](#Find): find the element in the class

### Iterators
- [`Begin`](#Begin): get the iterator to the first element
- [`begin`](#begin): get the iterator to the first element
- [`ConstBegin`](#ConstBegin): get the constant iterator to the first element
- [`End`](#End): get the iterator to the end of the tree
- [`end`](#end): get the iterator to the end of the tree
- [`ConstEnd`](#ConstEnd): get the constant iterator to the end of the tree

## Non-member Function
- [`Swap`](#SwapNonmember): swap the content

## Details
### <span id="DataStructure">Data Structure</span>
In a hash table, elements are stored in several buckets.  If elements are
distributed evenly, the time complexity of searching an element can have a
constant time complexity.  In this class, the elements in a bucket are
linked with a single linked list.  Besides, the elements are linked
together by the order of insertion with a double linked list.  For more
information, you may view the
[wikipedia page](https://en.wikipedia.org/wiki/Hash_table).

Complexity:
- Access: $O(1)$
- Insert: $O(1)$
- Remove: $O(1)$
- Traverse: $O(n)$

### <span id="Constructors">Constructors</span>
```c++
explicit LinkedHashTable(const Allocator& allocator = Allocator());
```
- Construct an empty table with the given allocator or the default allocator
  by default, as well as the default equality comparing class and default
  hash class.

```c++
explicit LinkedHashTable(const Hash& hash,
                         const Equal& equal = Equal(),
                         const Allocator& allocator = Allocator());
```
- Construct an empty table with the given hash function, equality comparing
  class, and allocator or the default allocator by default.

```c++
explicit LinkedHashTable(SizeT minimumBucketSize,
                         const Hash& hash = Hash(),
                         const Equal& equal = Equal(),
                         const Allocator& allocator = Allocator());
```
- Construct an empty table with the given minimum bucket size, hash function,
  equality comparing class, and allocator or the default allocator by
  default.
- The bucket size is the minimum size that satisfies the need bucket size
  in the [rehash policy class](rehash_policy_en.md).

```c++
template<class InputIt>
LinkedHashTable(InputIt first, InputIt last,
                const Hash& hash = Hash(),
                const Equal& equal = Equal(),
                const Allocator& allocator = Allocator());
```
- Construct a table with the elements in the range `[first, last)`, with the
  given hash function, equality comparing class, and allocator.
- Time complexity: $O(n)$. ($n$ is the number of elements in the range)

```c++
template<class InputIt>
LinkedHashTable(InputIt first, InputIt last,
                SizeT minimumBucketSize,
                const Hash& hash = Hash(),
                const Equal& equal = Equal(),
                const Allocator& allocator = Allocator());
```
- Construct a table with the elements in the range `[first, last)`, with the
  given minimum bucket size, hash function, equality comparing class, and
  allocator.
- The bucket size is the minimum size that satisfies the need bucket size
  in the [rehash policy class](rehash_policy_en.md).
- Time complexity: $O(n)$. ($n$ is the number of elements in the range)

```c++
LinkedHashTable(const LinkedHashTable& obj);
```
- Copy constructor
- Construct a table with the elements as `obj`.
- Time complexity: $O(n)$. ($n$ is the number of elements in the
  `obj` class)

```c++
LinkedHashTable(const LinkedHashTable& obj, const Allocator& allocator);
```
- Copy constructor with customized allocator
- Construct a class with the same elements as `obj`.
- The allocator of the new class copied from `allocator`.
- Time complexity: $O(n)$. ($n$ is the number of elements in the
  `obj` class)

```c++
LinkedHashTable(LinkedHashTable&& obj) noexcept;
```
- Move constructor
- Move all the elements from `obj` to the new class.
- Time complexity: $O(1)$.

```c++
LinkedHashTable(std::initializer_list<T> init,
                const Hash& hash,
                const Equal& equal = Equal(),
                const Allocator& allocator = Allocator());
```
- Construct a table with the elements in the initializer list `init`, with
  the given hash function, equality comparing class, and allocator.
- Time complexity: $O(n)$. ($n$ is the number of elements in the
  initializer list)

```c++
LinkedHashTable(std::initializer_list<T> init,
                const Allocator& allocator = Allocator());
```
- Construct a table with the elements in the initializer list `init`, with
  the given allocator or the default allocator by default, as well as the
  default equality comparing class and default hash class.

### <span id="operator=">`operator=`</span>
```c++
LinkedHashTable& operator=(const LinkedHashTable& obj);
```
- Assign the class with the `obj` class.
- Time complexity: $O(n)$. ($n$ is the number of elements in the
  `obj` class)

```c++
LinkedHashTable& operator=(LinkedHashTable&& obj) noexcept;
```
- Assign the class with the `obj` class.
- Time complexity: $O(1)$.

### <span id="Insert">`Insert`</span>
```c++
Pair<Iterator, bool> Insert(const T& value);
```
- Insert a new element with the value `value`.
- Return a pair of `Iterator` and a `bool`.  If such node doesn't exist,
  the first one is the iterator of the new node, and the second one is
  true; if such node exists, the first one is the iterator of the node
  that prevents this insertion, and the second one is false.
- Time complexity: $O(1)$ (on average).

```c++
Pair<Iterator, bool> Insert(T&& value);
```
- Insert a new element with the right value `value`.
- Return a pair of `Iterator` and a `bool`.  If such node doesn't exist,
  the first one is the iterator of the new node, and the second one is
  true; if such node exists, the first one is the iterator of the node
  that prevents this insertion, and the second one is false.
- Time complexity: $O(1)$ (on average).

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
- Time complexity: $O(1)$ (on average).

### <span id="Erase">`Erase`</span>
```c++
LinkedHashTable& Erase(const Iterator& position);
```
- Erase the element at `position`.
- Throw `lau::InvalidIterator` if the iterator doesn't belong to this class
  or the iterator is the end iterator.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(1)$.

```c++
LinkedHashTable& Erase(const T& value);
```
- Erase the element that is equal to `value`.
- Throw `lau::InvalidArgument` if the `value` doesn't belong to this class.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(1)$ (on average).

```c++
template<class K>
LinkedHashTable& Erase(const K& value);
```
- Erase the element that is equal to `value`.
- Throw `lau::InvalidArgument` if the `value` doesn't belong to this class.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(1)$ (on average).

### <span id="Clear">`Clear`</span>
```c++
LinkedHashTable& Clear() noexcept;
```
- Erase all the elements in the class.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(n)$.

### <span id="Swap">`Swap`</span>
```c++
LinkedHashTable& Swap(RBTree& other) noexcept;
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

### <span id="BucketCount">`BucketCount`</span>
```c++
[[nodiscard]] SizeT BucketCount() const noexcept;
```
- Get the number of buckets in the class.

### <span id="BucketCount">`ReserveAtLeast`</span>
```c++
LinkedHashTable& ReserveAtLeast(SizeT minimumSize) noexcept;
```
- Reserve at least `minimumSize` buckets in the class.

### <span id="GetHash">`GetHash`</span>
```c++
[[nodiscard]] Hash GetHash() const;
```
- Get a copy of the hash class.

### <span id="GetEqual">`GetEqual`</span>
```c++
[[nodiscard]] Equal GetEqual() const;
```
- Get a copy of the equality comparing class.

### <span id="GetCompare">`GetNodeAllocator`</span>
```c++
[[nodiscard]] NodeAllocatorType GetNodeAllocator() const;
```
- Get a copy of the allocator for nodes.

### <span id="GetCompare">`GetBucketAllocator`</span>
```c++
[[nodiscard]] BucketAllocatorType GetBucketAllocator() const;
```
- Get a copy of the allocator for buckets.

### <span id="Contains">`Contains`</span>
```c++
[[nodiscard]] bool Contains(const T& value) const;
```
- Check whether the class contains an element that is equal to `value`.
- Time complexity: $O(1)$ (on average).

```c++
template<class K>
[[nodiscard]] bool Contains(const K& value) const;
```
- Check whether the class contains an element that is equal to `value`.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(1)$ (on average).

### <span id="Find">`Find`</span>
```c++
[[nodiscard]] Iterator Find(const T& value);
```
- Find the element that is equal to `value`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Time complexity: $O(1)$ (on average).

```c++
[[nodiscard]] ConstIterator Find(const T& value) const;
```
- Find the element that is equal to `value`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Time complexity: $O(1)$ (on average).

```c++
template<class K>
[[nodiscard]] Iterator Find(const K& value);
```
- Find the element that is equal to `value`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(1)$ (on average).

```c++
template<class K>
[[nodiscard]] ConstIterator Find(const K& value) const;
```
- Find the element that is equal to `value`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(1)$ (on average).

### <span id="Begin">`Begin`</span>
```c++
[[nodiscard]] Iterator Begin() noexcept;
```
- Get the iterator of the first element.
- Note that the iterator is for the linked list of the class (the order
  of insertion).
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstIterator Begin() const noexcept;
```
- Get the constant iterator of the first element.
- Note that the iterator is for the linked list of the class (the order
  of insertion).
- Time complexity: $O(1)$.

```c++
[[nodiscard]] BucketIterator Begin(SizeT bucketIndex);
```
- Get the iterator of the first element in the bucket at `bucketIndex`.
- Note that the iterator is for the bucket at `bucketIndex`.
- If `bucketIndex` is out of bound, a `lau::InvalidArgument` will be thrown.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstBucketIterator Begin(SizeT bucketIndex) const;
```
- Get the constant iterator of the first element in the bucket at
  `bucketIndex`.
- Note that the iterator is for the bucket at `bucketIndex`.
- If `bucketIndex` is out of bound, a `lau::InvalidArgument` will be thrown.
- Time complexity: $O(1)$.

### <span id="begin">`begin`</span>
```c++
[[nodiscard]] Iterator begin() noexcept;
```
- Get the iterator of the first element.
- Note that the iterator is for the linked list of the class (the order
  of insertion).
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstIterator begin() const;
```
- Get the constant iterator of the first element.
- Note that the iterator is for the linked list of the class (the order
  of insertion).
- Time complexity: $O(1)$.

```c++
[[nodiscard]] BucketIterator begin(SizeT bucketIndex);
```
- Get the iterator of the first element in the bucket at `bucketIndex`.
- Note that the iterator is for the bucket at `bucketIndex`.
- If `bucketIndex` is out of bound, a `lau::InvalidArgument` will be thrown.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstBucketIterator begin(SizeT bucketIndex) const;
```
- Get the constant iterator of the first element in the bucket at
  `bucketIndex`.
- Note that the iterator is for the bucket at `bucketIndex`.
- If `bucketIndex` is out of bound, a `lau::InvalidArgument` will be thrown.
- Time complexity: $O(1)$.

### <span id="ConstBegin">`ConstBegin`</span>
```c++
[[nodiscard]] ConstIterator ConstBegin() const noexcept;
```
- Get the constant iterator of the first element.
- Note that the iterator is for the linked list of the class (the order
  of insertion).
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstBucketIterator ConstBegin(SizeT bucketIndex) const;
```
- Get the constant iterator of the first element in the bucket at
  `bucketIndex`.
- Note that the iterator is for the bucket at `bucketIndex`.
- If `bucketIndex` is out of bound, a `lau::InvalidArgument` will be thrown.
- Time complexity: $O(1)$.

### <span id="End">`End`</span>
```c++
[[nodiscard]] Iterator End() noexcept;
```
- Get the end iterator.
- Note that the iterator is for the linked list of the class (the order
  of insertion).
- Please note that the end iterator is pointing at the node after the last
  element.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstIterator End() const noexcept;
```
- Get the end constant iterator.
- Note that the iterator is for the linked list of the class (the order
  of insertion).
- Please note that the end constant iterator is pointing at the node after the
  last element.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] BucketIterator End(SizeT bucketIndex);
```
- Get the end iterator of the bucket at `bucketIndex`.
- Note that the iterator is for the bucket at `bucketIndex`.
- If `bucketIndex` is out of bound, a `lau::InvalidArgument` will be thrown.
- Please note that the end iterator is pointing at the node after the last
  element.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstBucketIterator End(SizeT bucketIndex) const;
```
- Get the end constant iterator of the bucket at `bucketIndex`.
- Note that the iterator is for the bucket at `bucketIndex`.
- If `bucketIndex` is out of bound, a `lau::InvalidArgument` will be thrown.
- Please note that the end constant iterator is pointing at the node after the
  last element.
- Time complexity: $O(1)$.

### <span id="end">`end`</span>
```c++
[[nodiscard]] Iterator end() noexcept;
```
- Get the end iterator.
- Note that the iterator is for the linked list of the class (the order
  of insertion).
- Please note that the end iterator is pointing at the node after the last
  element.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstIterator end() const noexcept;
```
- Get the end constant iterator.
- Note that the iterator is for the linked list of the class (the order
  of insertion).
- Please note that the end constant iterator is pointing at the node after the
  last element.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] BucketIterator end(SizeT bucketIndex);
```
- Get the end iterator of the bucket at `bucketIndex`.
- Note that the iterator is for the bucket at `bucketIndex`.
- If `bucketIndex` is out of bound, a `lau::InvalidArgument` will be thrown.
- Please note that the end iterator is pointing at the node after the last
  element.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstBucketIterator end(SizeT bucketIndex) const;
```
- Get the end constant iterator of the bucket at `bucketIndex`.
- Note that the iterator is for the bucket at `bucketIndex`.
- If `bucketIndex` is out of bound, a `lau::InvalidArgument` will be thrown.
- Please note that the end constant iterator is pointing at the node after the
  last element.
- Time complexity: $O(1)$.

### <span id="ConstEnd">`ConstEnd`</span>
```c++
[[nodiscard]] ConstIterator ConstEnd() const;
```
- Get the end constant iterator.
- Note that the iterator is for the linked list of the class (the order
  of insertion).
- Please note that the end constant iterator is pointing at the node after the
  last element.
- Time complexity: $O(1)$.

```c++
[[nodiscard]] ConstBucketIterator ConstEnd(SizeT bucketIndex) const;
```
- Get the end constant iterator of the bucket at `bucketIndex`.
- Note that the iterator is for the bucket at `bucketIndex`.
- If `bucketIndex` is out of bound, a `lau::InvalidArgument` will be thrown.
- Please note that the end constant iterator is pointing at the node after the
  last element.
- Time complexity: $O(1)$.

### <span id="SwapNonmember">`Swap`</span>
```c++
template<class T, class Hash, class Equal, class Allocator>
void Swap(LinkedHashTable<T, Hash, Equal, Allocator>& lhs,
          LinkedHashTable<T, Hash, Equal, Allocator>& rhs) noexcept;
```
- Swap the contents of two `LinkedHashTable`s.
- Time complexity: $O(1)$.

### <span id="iterator">iterator</span>
Types:
- `difference_type`: `std::ptrdiff_t`
- `value_type`: `T`
- `pointer`: `T*`
- `reference`: `T&`
- `iterator_category`: `std::output_iterator_tag`

Supported Operation: (The following iterators can be either `Iterator`,
`ConstIterator`, `BucketIterator`, or `ConstBucketIterator`)
- `iterator++`
- `++iterator`
- `iterator--`
- `--iterator`
- `*iterator`
- `iterator->...`
- `iterator1 == iterator2`
- `iterator1 != iterator2`

Invalidate when:
- Insert: all the bucket iterator (`BucketIterator` or `ConstBucketIterator`) if
  the number of buckets is changed
- Erase: the iterator pointing to the erased element
- Clear: all the iterators except the end iterator (the end iterator is not
  the end bucket iterator)
- Reserve: all the bucket iterator (`BucketIterator` or `ConstBucketIterator`)
  if the number of buckets is changed
