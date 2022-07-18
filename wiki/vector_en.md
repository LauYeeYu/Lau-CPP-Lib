# lau::Vector

Switch to Other Languages: [繁體中文（香港）](vector_zh.md)

Included in header `lau/vector.h`.

```c++
namespace lau {
template<class T,
         class Allocator = std::allocator<T>>
class Vector;
} // namespace lau
```

`lau::Vector` is a container that store data in a linear order, as a dynamic
array.  But different from `std::vector`, this class only store the pointer(s)
to data together, making it easier and faster to write data and to change
capacity.

For more information about the data structure, please go to the
[detail of data structure](#DataStructure).

## Overview
```c++
namespace lau {
template<class T,
         class Allocator = std::allocator<T>>
class Vector {
public:
    // Types
    using AllocatorType = Allocator;
    using PointerAllocatorType = typename std::allocator_traits<Allocator>::template rebind_alloc<T*>;
    class Iterator;
    class ConstIterator;

    // Constructors
    Vector() noexcept(noexcept(Allocator()));
    explicit Vector(const Allocator& allocator);
    Vector(SizeT count, const T& value, const Allocator& allocator = Allocator());
    explicit Vector(SizeT count, const Allocator& allocator = Allocator());
    template<class InputIterator>
    Vector(const InputIterator& begin,
           const InputIterator& end,
           const Allocator& allocator = Allocator());
    Vector(const Vector& obj);
    Vector(Vector&& obj) noexcept;
    Vector(std::initializer_list<T> init,
           const Allocator& allocator = Allocator());

    // Assignment Operators
    Vector& operator=(const Vector& obj);
    Vector& operator=(Vector&& obj) noexcept;

    // Destructor
    ~Vector();

    // Getter
    [[nodiscard]] AllocatorType GetAllocator() const noexcept;

    // Element Access
    T& At(SizeT index);
    const T& At(SizeT index) const;
    T& operator[](SizeT index);
    const T& operator[](SizeT index) const;
    T& Front();
    const T& Front() const;
    T& Back();
    const T& Back() const;

    // Iterators
    [[nodiscard]] Iterator Begin() noexcept;
    [[nodiscard]] ConstIterator Begin() const noexcept;
    [[nodiscard]] Iterator begin() noexcept;
    [[nodiscard]] ConstIterator begin() const noexcept;
    [[nodiscard]] ConstIterator ConstBegin() const noexcept;
    [[nodiscard]] Iterator End() noexcept;
    [[nodiscard]] ConstIterator End() const noexcept;
    [[nodiscard]] Iterator end() noexcept;
    [[nodiscard]] ConstIterator end() const noexcept;
    [[nodiscard]] ConstIterator ConstEnd() const noexcept;

    // Capacity
    [[nodiscard]] SizeT Capacity() const noexcept;
    [[nodiscard]] bool Empty() const noexcept;
    [[nodiscard]] long MaxSize() const noexcept;
    [[nodiscard]] SizeT Size() const noexcept;
    Vector& ShrinkToFit();
    Vector& Reserve(SizeT newCapacity);

    // Modifiers
    Vector& Clear() noexcept;
    Iterator Insert(const Iterator& position, const T& value);
    Iterator Insert(const ConstIterator& position, const T& value);
    Iterator Insert(SizeT index, const T& value);
    Iterator Erase(const Iterator& position);
    Iterator Erase(const ConstIterator& position);
    Iterator Erase(SizeT index);
    Vector& PushBack(const T& value);
    template<class... Args>
    Vector& EmplaceBack(Args&&... args);
    Vector& PushFront(const T& value);
    template<class... Args>
    Vector& EmplaceFront(Args... args);
    Vector& PopBack();
    Vector& PopFront();
    Vector& Swap(Vector& other) noexcept;
    Vector& SwapElement(SizeT index1, SizeT index2);
    Vector& SwapElement(const Iterator& iterator1, const Iterator& iterator2);
    Vector& Resize(SizeT count);
    Vector& Resize(SizeT count, const T& value);
    template<class Compare = std::less<T>>
    Vector& Sort(const Compare& compare = Compare());
};

// Non-member Function
template<class T, class Allocator>
void Swap(Vector<T, Allocator>& vector1, Vector<T, Allocator>& vector2) noexcept;
} // namespace lau
```
## Template
- `T`: the type contained in the vector
- `Allocator`: the type of allocator
  - need to satisfy the C++ allocator requirements (see
    [C++ named requirements: Allocator](https://en.cppreference.com/w/cpp/named_req/Allocator))
  - `std::allocator` is default.

## Member Types
- `AllocatorType`: the type of allocator, the same as `Allocator`
- `PointerAllocatorType`: the type of the allocator of `T*`, which is converted
  from the allocator
- [`Iterator`](#iterator): The type of iterator in `lau::Vector`
- [`ConstIterator`](#iterator): the type of constant iterator in `lau::Vector`

## Member Functions
- [(constructors)](#Constructors)
- [`operator=`](#operator=)
- (destructors)

### Getter
- [`GetAllocator`](#GetAllocator): get a copy of the allocator

### Element Access
- [`At`](#At): access the element with an index
- [`operator[]`](#operator[]): access the element with an index
- [`Front`](#Front): access the front element
- [`Back`](#Back): access the back element

### Iterators
- [`Begin`](#Begin): get the beginning iterator
- [`begin`](#begin): get the beginning iterator
- [`ConstBegin`](#ConstBegin): get the beginning constant iterator
- [`cbegin`](#cbegin): get the beginning constant iterator
- [`End`](#End): get the end iterator
- [`end`](#end): get the end iterator
- [`ConstEnd`](#ConstEnd): get the end constant iterator
- [`cend`](#cend): get the end constant iterator

### Capacity
- [`Capacity`](#Capacity): tell the capacity
- [`Empty`](#Empty): tell whether the container is empty
- [`MaxSize`](#MaxSize): tell the maximum size
- [`Size`](#Size): tell the size
- [`ShrinkToFit`](#ShrinkToFit): reduce the capacity to the minimum size
- [`Reserve`](#Reserve): reserve enough space

### Modifiers
- [`Clear`](#Clear): clear all the data
- [`Insert`](#Insert): insert an element
- [`Erase`](#Erase): erase the corresponding element
- [`PushBack`](#PushBack): add an element to the back
- [`EmplaceBack`](#EmplaceBack): add an element in-place to the back
- [`PushFront`](#PushFront): add an element to the front
- [`EmplaceFront`](#EmplaceFront): add an element in-place to the front
- [`PopBack`](#PopBack): remove an element from the back
- [`PopFront`](#PopFront): remove an element from the front
- [`Swap`](#Swap): swap the content
- [`SwapElement`](#SwapElement): swap two elements
- [`Resize`](#Resize): resize the vector
- [`Sort`](#Sort): sort the vector

## Non-member Function
- [`Swap`](#SwapNonmember): swap the content

## Details
### <span id="DataStructure">Data Structure</span>
The vector is just an array with dynamic length.

Complexity:
- Access: $O(1)$
- Insert: $O(n)$ (On average)
- Remove: $O(n)$ (On average)
- Traverse: $O(n)$

### <span id="Constructors">Constructors</span>
```c++
Vector() noexcept(noexcept(Allocator()));
```
- Default constructor
- Construct an empty vector with default allocator.

```c++
explicit Vector(const Allocator& allocator);
```
- Construct an empty vector with customized allocator.

```c++
Vector(SizeT count, const T& value, const Allocator& allocator = Allocator());
```
- Construct a vector with element that is copied from `value` with size
  `count` and an allocator.

```c++
explicit Vector(SizeT count, const Allocator& allocator = Allocator());
```
- Construct a vector with default element, size `count` and an allocator.
```c++
template<class InputIterator>
Vector(const InputIterator& begin,
       const InputIterator& end,
       const Allocator& allocator = Allocator());
```
- Construct a vector with iterator in [begin, end) and an allocator.

```c++
Vector(const Vector& obj);
```
- Copy constructor
- Construct the class with all the elements in the `obj` vector.

```c++
- Vector(Vector&& obj) noexcept;
```
- Move constructor
- Construct the class from a rvalue.

```c++
Vector(std::initializer_list<T> init,
       const Allocator& allocator = Allocator());
```
- Construct the class from a `std::initializer_list<T>`, with an allocator.

### <span id="operator=">`operator=`</span>
```c++
Vector& operator=(const Vector& obj);
```
- Assign the class with the `obj` class.

```c++
Vector& operator=(Vector&& obj) noexcept;
```
- Assign the class with the `obj` rvalue class.

### <span id="GetAllocator">`GetAllocator`</span>
```c++
[[nodiscard]] AllocatorType GetAllocator() const noexcept;
```
- Get a copy of the allocator in the vector.

### <span id="At">`At`</span>
```c++
T& At(SizeT index);
```
- Access the element with the input index.
- If pos is not in range [0, size), an `lau::OutOfRange` will be thrown.
- You can either read or write the element.
- Complexity: $O(1)$.
```c++
const T& At(SizeT index) const;
```
- Access the element with the input index.
- If pos is not in range [0, size), an `lau::OutOfRange` will be thrown.
- You can only read the element.
- Complexity: $O(1)$.

### <span id="operator[]">`operator[]`</span>
```c++
T& operator[](SizeT index);
```
- Access the element with the input index.
- If pos is not in range [0, size), an `lau::OutOfRange` will be thrown.
- You can either read or write the element.
- Complexity: $O(1)$.

```c++
const T& operator[](SizeT index) const;
```
- Access the element with the input index.
- If pos is not in range [0, size), an `lau::OutOfRange` will be thrown.
- You can only read the element.
- Complexity: $O(1)$.

### <span id="Front">`Front`</span>
```c++
T& Front();
```
- Access the first element.
- If the container is empty, a `lau::EmptyContainer` will be thrown.
- You can either read or write the element.
- Complexity: $O(1)$.

```c++
const T& Front() const;
```
- Access the first element.
- If the container is empty, a `lau::EmptyContainer` will be thrown.
- You can only read the element.
- Complexity: $O(1)$.

### <span id="Back">`Back`</span>
```c++
T& Back();
```
- Access the last element.
- If the container is empty, a `lau::EmptyContainer` will be thrown.
- You can either read or write the element.
- Complexity: $O(1)$.

```c++
const T& Back() const;
```
- Access the last element.
- If the container is empty, a `lau::EmptyContainer` will be thrown.
- You can only read the element.
- Complexity: $O(1)$.

### <span id="Begin">`Begin`</span>
```c++
[[nodiscard]] Iterator Begin() noexcept;
```
- Get the beginning iterator.

```c++
[[nodiscard]] ConstIterator Begin() const noexcept;
```
- Get the beginning constant iterator.

### <span id="begin">`begin`</span>
```c++
[[nodiscard]] Iterator Begin() noexcept;
```
- Get the beginning iterator.

```c++
[[nodiscard]] ConstIterator Begin() const noexcept;
```
- Get the beginning constant iterator.

### <span id="ConstBegin">`ConstBegin`</span>
```c++
[[nodiscard]] ConstIterator ConstBegin() const noexcept;
```
- Get the beginning constant iterator.

### <span id="End">`End`</span>
```c++
[[nodiscard]] Iterator End() noexcept;
```
- Get the end iterator.
- Please note that the end iterator is pointing at the element that is right
  after the last element (not the last element).

```c++
[[nodiscard]] ConstIterator End() const noexcept;
```
- Get the end constant iterator.
- Please note that the end iterator is pointing at the element that is right
  after the last element (not the last element).

### <span id="end">`end`</span>
```c++
[[nodiscard]] Iterator end() noexcept;
```
- Get the end iterator.
- Please note that the end iterator is pointing at the element that is right
  after the last element (not the last element).

```c++
[[nodiscard]] ConstIterator end() const noexcept;
```
- Get the end constant iterator.
- Please note that the end iterator is pointing at the element that is right
  after the last element (not the last element).

### <span id="ConstEnd">`ConstEnd`</span>
```c++
[[nodiscard]] ConstIterator ConstEnd() const noexcept;
```
- Get the end constant iterator.
- Please note that the end constant iterator is pointing at the element that
  is right after the last element (not the last element).

### <span id="Capacity">`Capacity`</span>
```c++
[[nodiscard]] SizeT Capacity() noexcept;
```
- Tell the capacity of this container.
- Please note that since its vector can be popped from the front, so the
  maximum of the vector might be less than the value.

### <span id="Empty">`Empty`</span>
```c++
[[nodiscard]] bool Empty() const noexcept;
```
- Tell whether the container is empty.

### <span id="MaxSize">`MaxSize`</span>
```c++
[[nodiscard]] long MaxSize() const noexcept;
```
- Tell the maximum size of this container.

### <span id="Size">`Size`</span>
```c++
[[nodiscard]] SizeT Size() const noexcept;
```
- Tell the size of the container.

### <span id="ShrinkToFit">`ShrinkToFit`</span>
```c++
Vector& ShrinkToFit();
```
- Reduce the capacity to the minimum capacity (i.e., its size).
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Complexity: Depends. But in most case, it is $O(n)$.

### <span id="Reserve">`Reserve`</span>
```c++
Vector& Reserve(SizeT newCapacity);
```
- Reserve enough space, at least `newCapacity`.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Please note that the reserved space *does not* have already been
  constructed, so don't access that.  If you want to access that space,
  please use the [`Resize`](#Resize) member function.
- Complexity: Depends. But in most case, it is $O(n)$.

### <span id="Clear">`Clear`</span>
```c++
Vector& Clear() noexcept;
```
- Clear all the elements in this vector.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Please note that this operation will also free all the memory it has
  allocated before, and after this operation, it will have no capacity.
- Complexity: $O(n)$.

### <span id="Insert">`Insert`</span>
```c++
Iterator Insert(const Iterator& position, const T& value);
```
- Insert value before position.
- Return an iterator pointing to the inserted value.
- Complexity: Depends. In most case, it is $O(n)$.

```c++
Iterator Insert(const ConstIterator& position, const T& value);
```
- Insert value before position.
- Return an iterator pointing to the inserted value.
- Complexity: Depends. In most case, it is $O(n)$.

```c++
Iterator Insert(SizeT index, const T& value);
```
- Insert value at index.
- If `index > size`, a `lau::OutOfRange` will be thrown.
- After this operation, `this->at(ind)` will be value.
- Return an iterator pointing to the inserted value.
- Complexity: Depends. In most case, it is $O(n)$.

### <span id="Erase">`Erase`</span>
```c++
Iterator Erase(const Iterator& position);
```
- Erase the element at pos.
- If it is an `End()` iterator, a `lau::OutOfRange` will be thrown.
- Return an iterator pointing to the following element.
- If the iterator pos refers the last element, the `End()` iterator is
  returned.
- Complexity: Depends. In most case, it is $O(n)$.

```c++
Iterator Erase(const ConstIterator& position);
```
- Erase the element at pos.
- If it is an `End()` iterator, a `lau::OutOfRange` will be thrown.
- Return an iterator pointing to the following element.
- If the iterator pos refers the last element, the `End()` iterator is
  returned.
- Complexity: Depends. In most case, it is $O(n)$.

```c++
Iterator Erase(SizeT index);
```
- Erase the element at index.
- If `index >= size`, a `lau::OutOfRange` will be thrown.
- Return an iterator pointing to the following element.
- If the iterator pos refers the last element, the `End()` iterator is
  returned.
- Complexity: Depends. In most case, it is $O(n)$.

### <span id="PushBack">`PushBack`</span>
```c++
Vector& PushBack(const T& value);
```
- Add an element (`value`) to the end.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Complexity: Depends. In most case, it is $O(1)$.  Only if the container
  need enlarging, it is $O(n)$.

### <span id="PushFront">`PushFront`</span>
```c++
Vector& PushFront(const T& value);
```
- Add an element (`value`) to the front.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Complexity: Depends. In most case, it is $O(n)$. Only if the container
  have space in the front, it is $O(1)$.

### <span id="PopBack">`PopBack`</span>
```c++
Vector& PopBack();
```
- Remove the last element from the end.
- If `size() == 0`, a `lau::EmptyContainer` will be thrown.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Complexity: $O(1)$.

### <span id="PopBack">`PopBack`</span>
```c++
Vector& PopFront();
```
- Remove the first element from the beginning.
- If `size() == 0`, a `lau::EmptyContainer` will be thrown.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Complexity: $O(1)$.

### <span id="Swap">`Swap`</span>
```c++
Vector& Swap(Vector& other) noexcept;
```
- Swap the content of two vector.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Complexity: $O(1)$.

### <span id="Resize">`Resize`</span>
```c++
Vector& Resize(SizeT count);
```
- Resize the container to contain count elements.
- If the current size is greater than count, the container is reduced to
  its first count elements.
- If the current size is less than count, additional default elements are
  appended.
- To make the class visiting easier, the function returns a reference
  pointing to this class.

```c++
Vector& Resize(SizeT count, const T& value);
```
- Resize the container to contain count elements.
- If the current size is greater than count, the container is reduced to
  its first count elements.
- If the current size is less than count, additional value elements are
  appended.
- To make the class visiting easier, the function returns a reference
  pointing to this class.

### <span id="SwapElement">`SwapElement`</span>
```c++
Vector& SwapElement(SizeT index1, SizeT index2);
```
- Swap the elements of two indexes.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- 
```c++
Vector& SwapElement(const Iterator& iterator1, const Iterator& iterator2);
```
- Swap the elements of two iterators.
- To make the class visiting easier, the function returns a reference
  pointing to this class.

### <span id="EmplaceBack">`EmplaceBack`</span>
```c++
template<class... Args>
Vector& EmplaceBack(Args&&... args);
```
- Add an element to the back.
- This operation constructs a new element in place.
- The constructor of the element is called with exactly the same arguments
  as supplied to the function.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Complexity: Depends. In most case, it is $O(1)$.  Only if the container
  need enlarging, it is $O(n)$.

### <span id="EmplaceFront">`EmplaceFront`</span>
```c++
template<class... Args>
Vector& EmplaceFront(Args&&... args);
```
- Add an element to the front.
- This operation constructs a new element in place.
- The constructor of the element is called with exactly the same arguments
  as supplied to the function.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Complexity: Depends. In most case, it is $O(n)$. Only if the container
  have space in the front, it is $O(1)$.

### <span id="Sort">`Sort`</span>
```c++
template<class Compare = std::less<T>>
Vector& Sort(const Compare& compare = Compare());
```
- Sort the vector.
- The function uses the [merge sort](https://en.wikipedia.org/wiki/Merge_sort)
  algorithm.
- The `Compare` class must define the
  `bool operator()(const T&, const T&) const` function.
- The function uses the `operator<` (`std::less`) as default to compare
 the elements.
- To make the class visiting easier, the function returns a reference
  pointing to this class.
- Complexity: $O(n \log n)$.

### <span id="SwapNonmember">`Swap`</span>
```c++
template<class T, class Allocator>
void Swap(Vector<T, Allocator>& vector1, Vector<T, Allocator>& vector2) noexcept;
```
- Swap the content of two vector.

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
- `iterator + n`
- `iterator - n`
- `iterator1 - iterator2`
- `iterator += n`
- `iterator -= n`
- `*iterator`
- `iterator->...`
- `iterator1 == iterator2`
- `iterator1 != iterator2`

Invalidate when:
- Insert: the iterator after the place
- Erase: the iterator after the place
- Enlarge: all iterators
- PushBack: the end iterator
- PopBack: the end iterator
- PushFront: all except the begin iterator if there is no space in the front;
  only begin iterator if there is space in the front
- PopFront: only begin iterator
