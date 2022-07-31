# lau::PriorityQueue

Switch to Other Languages: [繁體中文（香港）](priority_queue_zh.md)

Included in header `lau/priority_queue.h`.

```c++
namespace lau {
template<class T,
         class Compare = std::less<T>,
         class Allocator = std::allocator<T>>
class PriorityQueue;
} // namespace lau
```

`lau::PriorityQueue` is a queue that makes sure the element with priority will
be popped later than with less priority.  Therefore, the top element is the
one with the least priority, and it will be popped first.

A customized compare function can be supplied to compare the elements.  Note
that the `Compare` must have overloaded the `operator()` with two parameters.

This priority queue uses leftist tree as the data structure.  For more
information about the data structure, please go to the
[detail of data structure](#DataStructure).

Please note that because most operation in this class depends on recursive
functions, enough memory space must be reserved for stack to avoid stack
overflow.

## Overview
```c++
namespace lau {
template<class T,
         class Compare = std::less<T>,
         class Allocator = std::allocator<T>>
class PriorityQueue {
public:
    // Constructors
    PriorityQueue();
    explicit PriorityQueue(const Compare& compare);
    explicit PriorityQueue(const Allocator& allocator);
    PriorityQueue(const Compare& compare, const Allocator& allocator);
    PriorityQueue(const PriorityQueue& obj);
    PriorityQueue(PriorityQueue&& obj) noexcept;
    template<class Iterator>
    PriorityQueue(Iterator first, Iterator last,
                  const Compare& compare = Compare());
    template<class Iterator>
    PriorityQueue(Iterator first, Iterator last,
                  const Allocator& allocator);
    template<class Iterator>
    PriorityQueue(Iterator first, Iterator last,
                  const Compare& compare,
                  const Allocator& allocator);

    // Assignment Operators
    PriorityQueue& operator=(const PriorityQueue& obj);
    PriorityQueue& operator=(PriorityQueue&& obj) noexcept;

    // Destructor
    ~PriorityQueue();

    // Element Access
    const T& Top() const;

    // Capacity
    [[nodiscard]] SizeT Size() const;
    [[nodiscard]] bool Empty() const;
    [[nodiscard]] Compare GetCompare() const;

    // Modifiers
    PriorityQueue& Clear();
    PriorityQueue& Push(const T& value);
    template<class... Args>
    PriorityQueue& Emplace(Args&&... args);
    PriorityQueue& Pop();
    PriorityQueue& Merge(PriorityQueue& other);
    PriorityQueue& Swap(PriorityQueue& other);
};

// Non-member Function
template<class T, class Compare, class Allocator>
void Swap(PriorityQueue<T, Compare, Allocator> priorityQueue1,
          PriorityQueue<T, Compare, Allocator> priorityQueue2);
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

## Member Functions
- [(constructors)](#Constructors)
- [`operator=`](#operator=)
- (destructors)

### Element Access
- [`Top`](#Top): tell the top element

### Capacity
- [`Size`](#Size): tell the size
- [`Empty`](#Empty): tell whether the class is empty
- [`GetCompare`](#GetCompare): get a copy of the compare

### Modifiers
- [`Clear`](#Clear): clear all the data
- [`Push`](#Push): push a new element
- [`Emplace`](#Emplace): construct a new element in-place
- [`Pop`](#Pop): erase the top element
- [`Merge`](#Merge): merge another priority queue into this queue
- [`Swap`](#Swap): swap the content

## Non-member Function
- [`Swap`](#SwapNonmember): swap the content

## Details
### <span id="DataStructure">Data Structure</span>
This priority queue uses leftist tree as data structure.  As opposed to a
binary heap, a leftist tree attempts to be very unbalanced.  In addition to
the heap property, leftist trees are maintained so the right descendant of
each node has the lower s-value.

Every modification depends on merging.  Pushing a new element is merging a
single node binary heap into the main tree.  Popping from the queue is merging
the left descendant with the right one.  Merging two queue is to merge the two
trees together.

Complexity:
- Push: $O(\log n)$
- Pop: $O(\log n)$
- Merge: $O(\log n)$

### <span id="Constructors">Constructors</span>
```c++
PriorityQueue();
```
- Default constructor
- Construct an empty queue with default compare class, default allocator.

```c++
explicit PriorityQueue(const Compare& compare);
```
- Construct an empty class with customized compare class, and default
  allocator.

```c++
explicit PriorityQueue(const Allocator& allocator);
```
- Construct an empty class with customized allocator, and default compare
  class.

```c++
PriorityQueue(const Compare& compare, const Allocator& allocator);
```
- Construct an empty class with customized compare class and customized
  allocator.

```c++
PriorityQueue(const PriorityQueue& obj);
```
- Copy constructor
- Construct the class from another priority queue.
- This operation includes recursion, so please make sure that there is enough
  memory space for the stack.
- Time complexity: $O(n)$.

```c++
PriorityQueue(PriorityQueue&& obj) noexcept;
```
- Move constructor
- Construct the class from a rvalue priority queue.
- Time complexity: $O(1)$.

```c++
template<class Iterator>
PriorityQueue(Iterator first, Iterator last,
              const Compare& compare = Compare());
```
- Construct the class with the data from a certain iterator, with default
  (if omitted) or customized compare class, and default allocator.
- This operation includes recursion, so please make sure that there is enough
  memory space for the stack.
- Time complexity: $O(n\cdot \log n)$.

```c++
template<class Iterator>
PriorityQueue(Iterator first, Iterator last,
              const Allocator& allocator);
```
- Construct the class with the data from a certain iterator, with default
  compare class, and customized allocator.
- This operation includes recursion, so please make sure that there is enough
  memory space for the stack.
- Time complexity: $O(n\cdot \log n)$.

```c++
template<class Iterator>
PriorityQueue(Iterator first, Iterator last,
              const Compare& compare,
              const Allocator& allocator);
```
- Construct the class with the data from a certain iterator, with customized
  compare class, and customized allocator.
- This operation includes recursion, so please make sure that there is enough
  memory space for the stack.
- Time complexity: $O(n\cdot \log n)$.

### <span id="operator=">`operator=`</span>
```c++
PriorityQueue& operator=(const PriorityQueue& obj);
```
- Assign the class with the `obj` class.
- This operation includes recursion, so please make sure that there is enough
  memory space for the stack.

```c++
PriorityQueue& operator=(PriorityQueue&& obj) noexcept;
```
- Assign the class with the `obj` rvalue class.

### <span id="Top">`Top`</span>
```c++
const T& Top() const;
```
- Get the top element of the queue. The top element is one with the least
  priority.
- Time complexity: $O(1)$.

### <span id="Size">`Size`</span>
```c++
[[nodiscard]] SizeT Size() const;
```
- Get the number of elements in the queue.
- Time complexity: $O(1)$.

### <span id="Empty">`Empty`</span>
```c++
[[nodiscard]] bool Empty() const;
```
- Check whether the queue is empty.
- Time complexity: $O(1)$.

### <span id="GetCompare">`GetCompare`</span>
```c++
[[nodiscard]] Compare GetCompare() const;
```
- Get a copy of the compare class.

### <span id="Clear">`Clear`</span>
```c++
PriorityQueue& Clear();
```
- Clear the queue.
- This operation includes recursion, so please make sure that there is enough
  memory space for the stack.
- Time complexity: $O(n)$.

### <span id="Push">`Push`</span>
```c++
PriorityQueue& Push(const T& value);
```
- Push new element to the priority queue.
- This operation includes recursion, so please make sure that there is enough
  memory space for the stack.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(\log n)$.

### <span id="Pop">`Pop`</span>
```c++
PriorityQueue& Pop();
```
- Delete the top element.
- This operation includes recursion, so please make sure that there is enough
  memory space for the stack.
- Throw `lau::EmptyContainer` if the queue is empty.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(\log n)$.

### <span id="Merge">`Merge`</span>
```c++
PriorityQueue& Merge(PriorityQueue& other);
```
- Merge other priority queue into the current one.
- Clear the other priority queue.
- This operation includes recursion, so please make sure that there is enough
  memory space for the stack.
- Please note that the compare function of the two class *must* be the same.
  (For all elements, the same input must have the same output.)
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(\log n)$.

### <span id="Swap">`Swap`</span>
```c++
PriorityQueue& Swap(PriorityQueue& other);
```
- Swap all the component of two priority queue.
- Please note that the two class must have the same template parameters.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(1)$.

### <span id="Emplace">`Emplace`</span>
```c++
template<class... Args>
PriorityQueue& Emplace(Args&&... args);
```
- Push a new element to the priority queue.
- This operation constructs a new element in place.
- The constructor of the element is called with exactly the same arguments
  as supplied to the function.
- This operation includes recursion, so please make sure that there is enough
  memory space for the stack.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Time complexity: $O(\log n)$.

### <span id="SwapNonmember">`Swap`</span>
```c++
template<class T, class Compare, class Allocator>
void Swap(PriorityQueue<T, Compare, Allocator> priorityQueue1,
          PriorityQueue<T, Compare, Allocator> priorityQueue2);
```
- Swap all the component of two priority queue.
- Please note that the two class must have the same template parameters.
- Time complexity: $O(1)$.
