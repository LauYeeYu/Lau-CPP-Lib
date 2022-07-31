# lau::RBTree

Switch to Other Languages: [繁體中文（香港）](RB_tree_zh.md)

Included in header `lau/RB_tree.h`.

```c++
namespace lau {
template<class T,
         class Compare = std::less<T>,
         class Allocator = std::allocator<T>>
class RBTree;
} // namespace lau
```

`lau::RBTree` uses the data structure called
[red-black tree](https://en.wikipedia.org/wiki/Red-black_tree).  The
tree is a sorted associative container with logarithmic complexity in
search, removal and insertion.

A customized compare function can be supplied to compare the elements.  Note
that the `Compare` must have an overloaded
`bool operator(const T&, const T&) const` function.  It must also guarantee
that the order of the elements is preserved.

Note: this class is an implementation of the data structure, so it gives
users the freedom to modify most of the data by using the iterator.  In most
case, as long as you didn't modify the order given by the `Compare` class,
there is no risk.  But if you want to be 100% sure that no undefined
behaviour will occur, you should use the `lau::Map` class.

For more information about the data structure, please go to the
[detail of data structure](#DataStructure).

## Overview
```c++
namespace lau {
template<class T,
         class Compare = std::less<T>,
         class Allocator = std::allocator<T>>
class RBTree {
public:
    struct Node;
    class  Iterator;
    class  ConstIterator;

    enum Flag {red, black};

    using AllocatorType = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;

    // Constructors
    RBTree();
    explicit RBTree(const Compare& compare);
    explicit RBTree(const Allocator& allocator);
    RBTree(const Compare& compare, const Allocator& allocator);
    template<class InputIterator>
    RBTree(InputIterator begin, InputIterator end);
    template<class InputIterator>
    RBTree(InputIterator begin, InputIterator end, const Compare& compare);
    template<class InputIterator>
    RBTree(InputIterator begin, InputIterator end, const Allocator& allocator);
    template<class InputIterator>
    RBTree(InputIterator begin, InputIterator end,
           const Compare& compare,
           const Allocator& allocator);
    RBTree(const RBTree& obj);
    RBTree(const RBTree& obj, const Allocator& allocator);
    RBTree(RBTree&& obj) noexcept;
    RBTree(std::initializer_list<T> init);
    RBTree(std::initializer_list<T> init,
           const Compare& compare);
    RBTree(std::initializer_list<T> init,
           const Allocator& allocator);
    RBTree(std::initializer_list<T> init,
           const Compare& compare,
           const Allocator& allocator);

    // Assignment Operators
    RBTree& operator=(const RBTree& obj);
    RBTree& operator=(RBTree&& obj) noexcept;

    // Destructor
    ~RBTree();

    // Modifiers
    Pair<Iterator, bool> Insert(const T& value);
    Pair<Iterator, bool> Insert(T&& value);
    template<class... Args>
    Pair<Iterator, bool> Emplace(Args&&... args);
    RBTree& Erase(const Iterator& position);
    RBTree& Erase(const T& value);
    template<class K>
    RBTree& Erase(const K& value);
    RBTree& Clear() noexcept;
    RBTree& Swap(RBTree& other) noexcept;

    // Capacity
    [[nodiscard]] SizeT Size() const noexcept;
    [[nodiscard]] bool Empty() const noexcept;
    [[nodiscard]] long MaxSize() const noexcept;

    // Getters
    [[nodiscard]] AllocatorType GetAllocator() const noexcept;
    [[nodiscard]] Compare GetCompare() const;

    // Lookup
    [[nodiscard]] bool Contains(const T& value) const;
    template<class K>
    [[nodiscard]] bool Contains(const K& value) const;
    [[nodiscard]] Iterator Find(const T& value);
    [[nodiscard]] ConstIterator Find(const T& value) const;
    template<class K>
    [[nodiscard]] Iterator Find(const K& value);
    template<class K>
    [[nodiscard]] ConstIterator Find(const K& value) const;
    [[nodiscard]] Iterator LowerBound(const T& value);
    [[nodiscard]] ConstIterator LowerBound(const T& value) const;
    template<class K>
    [[nodiscard]] Iterator LowerBound(const K& value);
    template<class K>
    [[nodiscard]] ConstIterator LowerBound(const K& value) const;
    [[nodiscard]] Iterator UpperBound(const T& value);
    [[nodiscard]] ConstIterator UpperBound(const T& value) const;
    template<class K>
    [[nodiscard]] Iterator UpperBound(const K& value);
    template<class K>
    [[nodiscard]] ConstIterator UpperBound(const K& value) const;

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
template <class T, class Compare, class Allocator>
void Swap(RBTree<T, Compare, Allocator>& lhs,
          RBTree<T, Compare, Allocator>& rhs) noexcept;
} // namespace lau
```
## Template
- `T`: the type of element
- `Compare`: the type of compare function (`std::less<T>` is default)
  - need to satisfy the C++ comparing class requirements (see
    [C++ named requirements: Compare](https://en.cppreference.com/w/cpp/named_req/Compare))
    - `bool Compare(const T& lhs, const T& rhs)` must be available.
    - If `Compare(a, b) = false` and `Compare(a, b) = false`, or
      `Compare(a, b) = true` and `Compare(a, b) = true`, `a` must be the
      same as `b`. (Uniqueness)
- `Allocator`: the type of allocator
  - need to satisfy the C++ allocator requirements (see
    [C++ named requirements: Allocator](https://en.cppreference.com/w/cpp/named_req/Allocator))
  - `std::allocator<T>` is default.

## Enumeration Type
- `Flag`: the colour of the tree nodes
  - `red`
  - `black`

## Member Types
- `Node`: the node in the read black tree
  - You may use the interface to have a view of the tree's structure. 
  - The code structure is listed below.
```c++
struct Node {
  friend RBTree;
  
  public:
  Node() = default;
  
  explicit Node(const T& valueIn) : value(valueIn) {}
  explicit Node(T&& valueIn) : value(std::move(valueIn)) {}
  
  Node(Node& obj) = default;
  Node(const Node& obj) = default;
  Node(Node&& obj) = default;
  
  template<class... Args>
  explicit Node(Args&&... args) : value(std::forward<Args>(args)...) {}
  
  Node& operator=(Node& obj) = default;
  Node& operator=(const Node& obj) = default;
  Node& operator=(Node&& obj) = default;
  
  ~Node() = default;
  
  Flag  Colour() const noexcept;
  Node* Parent() const noexcept;
  Node* Left()   const noexcept;
  Node* Right()  const noexcept;
  
  T value;
};
```
- `AllocatorType`: the allocator type for the `Node` type
- [`Iterator`](#iterator): The type of iterator in `lau::RBTree`
- [`ConstIterator`](#iterator): the type of constant iterator in `lau::RBTree`

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
- [`GetAllocator`](#GetAllocator): get the allocator of the tree
- [`GetCompare`](#GetCompare): get the compare function of the tree

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
The red-black tree is a kind of self-balancing binary search tree.  It
uses a special strategy to reduce the frequency of re-balancing.  By just
guaranteeing that the height of the left subtree of each node is at least
half and at most twice of the height of the right subtree, the frequency of
re-balancing is reduced to half of the AVL tree.  For more information, you
may view the
[wikipedia page](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree).

Complexity:
- Access: $O(\log n)$
- Insert: $O(\log n)$
- Remove: $O(\log n)$
- Traverse: $O(n)$

### <span id="Constructors">Constructors</span>
```c++
RBTree();
```
- Default constructor
- Construct an empty tree with the default compare class and the default
  allocator.

```c++
explicit RBTree(const Compare& compare);
```
- Construct an empty tree with a customized compare class and the default
  allocator.

```c++
explicit RBTree(const Allocator& allocator);
```
- Construct an empty tree with the default compare class and a customized
  allocator.

```c++
RBTree(const Compare& compare, const Allocator& allocator);
```
- Construct an empty tree with a customized compare class and the customized
  allocator.

```c++
template<class InputIterator>
RBTree(InputIterator begin, InputIterator end);
```
- Construct a tree whose elements are among the range `[begin, end)`
  with the default compare class and the default allocator.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  range)

```c++
template<class InputIterator>
RBTree(InputIterator begin, InputIterator end, const Compare& compare);
```
- Construct a tree whose elements are among the range `[begin, end)`
  with a customized compare class and the default allocator.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  range)

```c++
template<class InputIterator>
RBTree(InputIterator begin, InputIterator end, const Allocator& allocator);
```
- Construct a tree whose elements are among the range `[begin, end)`
  with the default compare class and a customized allocator.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  range)

```c++
template<class InputIterator>
RBTree(InputIterator begin, InputIterator end,
       const Compare& compare,
       const Allocator& allocator);
```
- Construct a tree whose elements are among the range `[begin, end)`
  with a customized compare class and a customized allocator.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  range)

```c++
RBTree(const RBTree& obj);
```
- Copy constructor
- Construct a class with the same elements, allocator and compare class
  as `obj`.
- Time complexity: $O(n)$. ($n$ is the number of elements in the
  `obj` class)

```c++
RBTree(const RBTree& obj, const Allocator& allocator);
```
- Copy constructor with customized allocator
- Construct a class with the same elements and compare class as `obj`.
- The allocator of the new class copied from `allocator`.
- Time complexity: $O(n)$. ($n$ is the number of elements in the
  `obj` class)

```c++
RBTree(RBTree&& obj) noexcept;
```
- Move constructor
- Construct a class with the same elements, allocator and compare class
  as `obj`.
- Time complexity: $O(1)$.

```c++
RBTree(std::initializer_list<T> init);
```
- Construct a tree with the elements in the initializer list `init`.
- The allocator and the compare class are constructed with their default
  constructor.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  initializer list)

```c++
RBTree(std::initializer_list<T> init,
       const Compare& compare);
```
- Construct a tree with the elements in the initializer list `init`
  and a customized compare class.
- The allocator is constructed with its default constructor.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  initializer list)

```c++
RBTree(std::initializer_list<T> init,
       const Allocator& allocator);
```
- Construct a tree with the elements in the initializer list `init`
  and a customized allocator.
- The compare class is constructed with its default constructor.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  initializer list)

```c++
RBTree(std::initializer_list<T> init,
       const Compare& compare,
       const Allocator& allocator);
```
- Construct a tree with the elements in the initializer list `init`
  and a customized compare class and allocator.
- Time complexity: $O(n \log n)$. ($n$ is the number of elements in the
  initializer list)

### <span id="operator=">operator=</span>
```c++
RBTree& operator=(const RBTree& obj);
```
- Assign the class with the `obj` class.
- Time complexity: $O(n)$. ($n$ is the number of elements in the
  `obj` class)

```c++
RBTree& operator=(RBTree&& obj) noexcept;
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
- Time complexity: $O(\log n)$.

```c++
Pair<Iterator, bool> Insert(T&& value);
```
- Insert a new element with the right value `value`.
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
RBTree& Erase(const Iterator& position);
```
- Erase the element at `position`.
- Throw `lau::InvalidIterator` if the iterator doesn't belong to this tree
  or the iterator is the end iterator.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(\log n)$ (on average).

```c++
RBTree& Erase(const T& value);
```
- Erase the element that is equal to `value`.
- Throw `lau::InvalidArgument` if the `value` doesn't belong to this tree.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(\log n)$.

```c++
template<class K>
RBTree& Erase(const K& value);
```
- Erase the element that is equal to `value`.
- Throw `lau::InvalidArgument` if the `value` doesn't belong to this tree.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(\log n)$.

### <span id="Clear">`Clear`</span>
```c++
RBTree& Clear() noexcept;
```
- Erase all the elements in the tree.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(n)$.

### <span id="Swap">`Swap`</span>
```c++
RBTree& Swap(RBTree& other) noexcept;
```
- Swap the contents of the two trees.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(1)$.

### <span id="Size">`Size`</span>
```c++
[[nodiscard]] SizeT Size() const noexcept;
```
- Get the number of elements in the tree.
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
[[nodiscard]] AllocatorType GetAllocator() const noexcept;
```
- Get a copy of the allocator.

### <span id="GetCompare">`GetCompare`</span>
```c++
[[nodiscard]] Compare GetCompare() const;
```
- Get a copy of the compare class.

### <span id="Contains">`Contains`</span>
```c++
[[nodiscard]] bool Contains(const T& value) const;
```
- Check whether the tree contains an element that is equal to `value`.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] bool Contains(const K& value) const;
```
- Check whether the tree contains an element that is equal to `value`.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

### <span id="Find">`Find`</span>
```c++
[[nodiscard]] Iterator Find(const T& value);
```
- Find the element that is equal to `value`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Time complexity: $O(\log n)$.

```c++
[[nodiscard]] ConstIterator Find(const T& value) const;
```
- Find the element that is equal to `value`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] Iterator Find(const K& value);
```
- Find the element that is equal to `value`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] ConstIterator Find(const K& value) const;
```
- Find the element that is equal to `value`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

### <span id="LowerBound">`LowerBound`</span>
```c++
[[nodiscard]] Iterator LowerBound(const T& value);
```
- Find the first element that no less than `value`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Time complexity: $O(\log n)$.

```c++
[[nodiscard]] ConstIterator LowerBound(const T& value) const;
```
- Find the first element that no less than `value`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] Iterator LowerBound(const K& value);
```
- Find the first element that no less than `value`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] ConstIterator LowerBound(const K& value) const;
```
- Find the first element that no less than `value`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

### <span id="UpperBound">`UpperBound`</span>
```c++
[[nodiscard]] Iterator UpperBound(const T& value);
```
- Find the first element that greater than `value`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Time complexity: $O(\log n)$.

```c++
[[nodiscard]] ConstIterator UpperBound(const T& value) const;
```
- Find the first element that greater than `value`.
- Return the constant iterator of the element if it exists; otherwise, return
  the end constant iterator.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] Iterator UpperBound(const K& value);
```
- Find the first element that greater than `value`.
- Return the iterator of the element if it exists; otherwise, return
  the end iterator.
- Please note that the type `K` must have valid `Compare::is_transparent`.
- Time complexity: $O(\log n)$.

```c++
template<class K>
[[nodiscard]] ConstIterator UpperBound(const K& value) const;
```
- Find the first element that greater than `value`.
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
template <class T, class Compare, class Allocator>
void Swap(RBTree<T, Compare, Allocator>& lhs,
          RBTree<T, Compare, Allocator>& rhs) noexcept;
```
- Swap the contents of two `RBTree`s.
- Time complexity: $O(1)$.

### <span id="iterator">iterator</span>
Types:
- `difference_type`: `std::ptrdiff_t`
- `value_type`: `T`
- `pointer`: `T*`
- `reference`: `T&`
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
