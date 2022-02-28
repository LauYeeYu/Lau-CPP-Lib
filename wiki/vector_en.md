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
    class Iterator;
	class ConstIterator;

    // Constructors
    Vector() noexcept(noexcept(Allocator()));
    explicit Vector(const Allocator& allocator);
    Vector(SizeT count, const T& value, const Allocator& allocator = Allocator());
    explicit Vector(SizeT count, const Allocator& allocator = Allocator());
    template<class InputIterator>
    Vector(const InputIterator& first,
           const InputIterator& last,
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
    [[nodiscard]] AllocatorType GetAllocator() const;

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
    [[nodiscard]] Iterator Begin() const;
    [[nodiscard]] Iterator begin() const;
    [[nodiscard]] ConstIterator ConstBegin() const;
    [[nodiscard]] Iterator End() const;
    [[nodiscard]] Iterator end() const;
    [[nodiscard]] ConstIterator ConstEnd() const;

    // Capacity
    [[nodiscard]] SizeT Capacity() const;
    [[nodiscard]] bool Empty() const;
    [[nodiscard]] long MaxSize() const;
    [[nodiscard]] SizeT Size() const;
    Vector& ShrinkToFit();
    Vector& Reserve(SizeT newCapacity);

    // Modifiers
    Vector& Clear();
    Iterator Insert(const Iterator& position, const T& value);
    Iterator Insert(SizeT index, const T& value);
    Iterator Erase(const Iterator& position);
    Iterator Erase(SizeT index);
    Vector& PushBack(const T& value);
    Vector& PushFront(const T& value);
    Vector& PopBack();
    Vector& PopFront();
    Vector& Swap(Vector& other);
    Vector& Resize(SizeT count);
    Vector& Resize(SizeT count, const T& value);
};

// Non-member Function
template<class T>
void Swap(Vector<T>& vector1, Vector<T>& vector2);
} // namespace lau
```
## Template
- `T`: the type contained in the vector
- `Allocator`: the type of allocator (`std::allocator` is default).

## Member Type
- `AllocatorType`: the type of allocator, the same as `Allocator`
- [`Iterator`](#iterator): The type of iterator in `lau::vector`
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
- [`End`](#End): get the end iterator
- [`end`](#end): get the end iterator
- [`ConstEnd`](#ConstEnd): get the end constant iterator

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
- [`PushFront`](#PushFront): add an element to the front
- [`PopBack`](#PopBack): remove an element from the back
- [`PopFront`](#PopFront): remove an element from the front
- [`Swap`](#Swap): swap the content
- [`Resize`](#Resize): resize the vector

## Non-member Function
- [`Swap`](#SwapNonmember): swap the content

## Details
### <span id="DataStructure">Data Structure</span>
The vector is just an array with dynamic length.

Complexity:
- Access: $O(1)$
- Insert: $O(n)$
- Remove: $O(n)$

### <span id="Constructors">Constructors</span>
- `Vector() noexcept(noexcept(Allocator()));`
  - Default constructor
  - Construct an empty vector with default allocator.
- `explicit Vector(const Allocator& allocator);`
  - Construct an empty vector with customized allocator.
- `Vector(SizeT count, const T& value, const Allocator& allocator = Allocator());`
  - Construct a vector with element that is copied from `value` with size
    `count` and an allocator.
- `explicit Vector(SizeT count, const Allocator& allocator = Allocator());`
  - Construct a vector with default element, size `count` and an allocator.
- `template<class InputIterator> Vector(const InputIterator& first, const InputIterator& last, const Allocator& allocator = Allocator());`
  - Construct a vector with iterator in [first, end) and an allocator.
- `Vector(const Vector& obj);`
  - Copy constructor
  - Construct the class with all the elements in the `obj` vector.
- `Vector(Vector&& obj) noexcept;`
  - Move constructor
  - Construct the class from a rvalue.
- `Vector(std::initializer_list<T> init, const Allocator& allocator = Allocator());`
  - Construct the class from a `std::initializer_list<T>`, with an allocator.

### <span id="operator=">`operator=`</span>
- `Vector& operator=(const Vector& obj);`
  - Assign the class with the `obj` class.
- `Vector& operator=(Vector&& obj) noexcept;`
  - Assign the class with the `obj` rvalue class.

### <span id="GetAllocator">`GetAllocator`</span>
- `[[nodiscard]] AllocatorType GetAllocator() const;`
  - Get a copy of the allocator in the vector.

### <span id="At">`At`</span>
- `T& At(SizeT index);`
  - Access the element with the input index.
  - If pos is not in range [0, size), an `lau::OutOfRange` will be thrown.
  - You can either read or write the element.
  - Complexity: $O(1)$.
- `const T& At(SizeT index) const;`
  - Access the element with the input index.
  - If pos is not in range [0, size), an `lau::OutOfRange` will be thrown.
  - You can only read the element.
  - Complexity: $O(1)$.

### <span id="operator[]">`operator[]`</span>
- `T& operator[](SizeT index);`
  - Access the element with the input index.
  - If pos is not in range [0, size), an `lau::OutOfRange` will be thrown.
  - You can either read or write the element.
  - Complexity: $O(1)$.
- `const T& operator[](SizeT index) const;`
  - Access the element with the input index.
  - If pos is not in range [0, size), an `lau::OutOfRange` will be thrown.
  - You can only read the element.
  - Complexity: $O(1)$.

### <span id="Front">`Front`</span>
- `T& Front();`
  - Access the first element.
  - If the container is empty, a `lau::EmptyContainer` will be thrown.
  - You can either read or write the element.
  - Complexity: $O(1)$.
- `const T& Front() const;`
  - Access the first element.
  - If the container is empty, a `lau::EmptyContainer` will be thrown.
  - You can only read the element.
  - Complexity: $O(1)$.

### <span id="Back">`Back`</span>
- `T& Back();`
  - Access the last element.
  - If the container is empty, a `lau::EmptyContainer` will be thrown.
  - You can either read or write the element.
  - Complexity: $O(1)$.
- `const T& Back() const;`
  - Access the last element.
  - If the container is empty, a `lau::EmptyContainer` will be thrown.
  - You can only read the element.
  - Complexity: $O(1)$.

### <span id="Begin">`Begin`</span>
- `[[nodiscard]] Iterator Begin() const;`
  - Get the beginning iterator.

### <span id="begin">`begin`</span>
- `[[nodiscard]] Iterator Begin() const;`
  - Get the beginning iterator.

### <span id="ConstBegin">`ConstBegin`</span>
- `[[nodiscard]] ConstIterator ConstBegin() const;`
  - Get the beginning constant iterator.

### <span id="End">`End`</span>
- `[[nodiscard]] Iterator End() const;`
  - Get the end iterator.
  - Please note that the end iterator is pointing at the element that is right
    after the last element (not the last element).

### <span id="end">`end`</span>
- `[[nodiscard]] Iterator end() const;`
  - Get the end iterator.
  - Please note that the end iterator is pointing at the element that is right
    after the last element (not the last element).

### <span id="ConstEnd">`ConstEnd`</span>
- `[[nodiscard]] ConstIterator ConstEnd() const;`
  - Get the end constant iterator.
  - Please note that the end constant iterator is pointing at the element that
    is right after the last element (not the last element).

### <span id="Capacity">`Capacity`</span>
- `[[nodiscard]] SizeT Capacity();`
  - Tell the capacity of this container.
  - Please note that since its vector can be popped from the front, so the
    maximum of the vector might be less than the value.

### <span id="Empty">`Empty`</span>
- `[[nodiscard]] bool Empty() const;`
  - Tell whether the container is empty.

### <span id="MaxSize">`MaxSize`</span>
- `[[nodiscard]] long MaxSize() const;`
  - Tell the maximum size of this container.

### <span id="Size">`Size`</span>
- `[[nodiscard]] SizeT Size() const;`
  - Tell the size of the container.

### <span id="ShrinkToFit">`ShrinkToFit`</span>
- `Vector& ShrinkToFit();`
  - Reduce the capacity to the minimum capacity (i.e., its size).
  - To make the class visiting easier, the function returns a reference
    pointing to this class.
  - Complexity: Depends. But in most case, it is $O(n)$.

### <span id="Reserve">`Reserve`</span>
- `Vector& Reserve(SizeT newCapacity);`
  - Reserve enough space, at least `newCapacity`.
  - To make the class visiting easier, the function returns a reference
    pointing to this class.
  - Please note that the reserved space *does not* have already been
    constructed, so don't access that.  If you want to access that space,
    please use the [`Resize`](#Resize) member function.
  - Complexity: Depends. But in most case, it is $O(n)$.

### <span id="Clear">`Clear`</span>
- `Vector& Clear();`
  - Clear all the elements in this vector.
  - To make the class visiting easier, the function returns a reference
    pointing to this class.
  - Please note that this operation will also free all the memory it has
    allocated before, and after this operation, it will have no capacity.
  - Complexity: $O(n)$.

### <span id="Insert">`Insert`</span>
- `Iterator Insert(const Iterator& position, const T& value);`
  - Insert value before position.
  - Return an iterator pointing to the inserted value.
  - Complexity: Depends. In most case, it is $O(n)$.
- `Iterator Insert(SizeT index, const T& value);`
  - Insert value at index.
  - If `index > size`, a `lau::OutOfRange` will be thrown.
  - After this operation, `this->at(ind)` will be value.
  - Return an iterator pointing to the inserted value.
  - Complexity: Depends. In most case, it is $O(n)$.

### <span id="Erase">`Erase`</span>
- `Iterator Erase(const Iterator& position);`
  - Erase the element at pos.
  - If it is an `End()` iterator, a `lau::OutOfRange` will be thrown.
  - Return an iterator pointing to the following element.
  - If the iterator pos refers the last element, the `End()` iterator is
    returned.
  - Complexity: Depends. In most case, it is $O(n)$.
- `Iterator Erase(SizeT index);`
  - Erase the element at index.
  - If `index >= size`, a `lau::OutOfRange` will be thrown.
  - Return an iterator pointing to the following element.
  - If the iterator pos refers the last element, the `End()` iterator is
    returned.
  - Complexity: Depends. In most case, it is $O(n)$.

### <span id="PushBack">`PushBack`</span>
- `Vector& PushBack(const T& value);`
  - Add an element (`value`) to the end.
  - To make the class visiting easier, the function returns a reference
    pointing to this class.
  - Complexity: Depends. In most case, it is $O(1)$.  Only if the container
    need enlarging, it is $O(n)$.

### <span id="PushFront">`PushFront`</span>
- `Vector& PushFront(const T& value);`
  - Add an element (`value`) to the front.
  - To make the class visiting easier, the function returns a reference
    pointing to this class.
  - Complexity: Depends. In most case, it is $O(n)$. Only if the container
    have space in the front, it is $O(1)$.

### <span id="PopBack">`PopBack`</span>
- `Vector& PopBack();`
  - Remove the last element from the end.
  - If `size() == 0`, a `lau::EmptyContainer` will be thrown.
  - To make the class visiting easier, the function returns a reference
    pointing to this class.
  - Complexity: $O(1)$.

### <span id="PopBack">`PopBack`</span>
- `Vector& PopFront();`
  - Remove the first element from the beginning.
  - If `size() == 0`, a `lau::EmptyContainer` will be thrown.
  - To make the class visiting easier, the function returns a reference
    pointing to this class.
  - Complexity: $O(1)$.

### <span id="Swap">`Swap`</span>
- `Vector& Swap(Vector& other);`
  - Swap the content of two vector.
  - To make the class visiting easier, the function returns a reference
    pointing to this class.
  - Complexity: $O(1)$.

### <span id="Resize">`Resize`</span>
- `Vector& Resize(SizeT count);`
  - Resize the container to contain count elements.
  - If the current size is greater than count, the container is reduced to
    its first count elements.
  - If the current size is less than count, additional default elements are
    appended.
  - To make the class visiting easier, the function returns a reference
    pointing to this class.
- `Vector& Resize(SizeT count, const T& value);`
  - Resize the container to contain count elements.
  - If the current size is greater than count, the container is reduced to
    its first count elements.
  - If the current size is less than count, additional value elements are
    appended.
  - To make the class visiting easier, the function returns a reference
    pointing to this class.

### <span id="SwapNonmember">`Swap`</span>
- `template<class T> void Swap(Vector<T>& vector1, Vector<T>& vector2);`
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
