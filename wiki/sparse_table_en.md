# lau::SparseTable

Switch to Other Languages: [繁體中文（香港）](sparse_table_zh.md)

Included in header `lau/algorithm.h`.

```c++
namespace lau {
template<class T>
class SparseTable;
} // namespace lau
```

`lau::SparseTable` is a data structure to boost the efficiency of querying
for multiple times by preprocessing data.

For more information about the data structure, please go to the
[detail of data structure](#DataStructure).

Please note that the function used in this procedure must have the following
properties: (The Function below is just a mathematical motion. The calculation
only call the function with two input.)
1. Function(a) = a;
2. Function(a, b) = Function(b, a);
3. Function(a, b, ...) = Function(Function(a, b), ...).

Simple example of such function: gcd, max, min.

## Overview
```c++
namespace lau {
template<class T>
class SparseTable;
public:
    // Constructors
    SparseTable() noexcept;
    template<class Iterator>
    SparseTable(Iterator begin, Iterator end, std::function<T(T, T)> Function);
    template<class Iterator>
    SparseTable(Iterator begin, SizeT size, std::function<T(T, T)> Function);
    SparseTable(const SparseTable& obj);
    SparseTable(SparseTable&& obj) noexcept;

    // Assignment Operators
    SparseTable& operator=(const SparseTable& obj);
    SparseTable& operator=(SparseTable&& obj) noexcept;

    // Destructor
    ~SparseTable();

    // Getter
    [[nodiscard]] std::function<T(T, T)> GetFunction() const;
    [[nodiscard]] SizeT Size() const noexcept;

    // Capacity
    [[nodiscard]] bool Empty() const noexcept;

    // Modifiers
    SparseTable& Clear();
    template<class Iterator>
    SparseTable& Calculate(Iterator begin,
                           Iterator end,
                           std::function<T(T, T)> Function);
    template<class Iterator>
    SparseTable& Calculate(Iterator begin,
                           SizeT size,
                           std::function<T(T, T)> Function);
    
    //operation
    T Query(SizeT beginIndex, SizeT endIndex);
};
} // namespace lau
```
## Template
- `T`: the type of the input type and the return type

## Member Functions
- [(constructors)](#Constructors)
- [`operator=`](#operator=)
- (destructors)

### Getter
- [`GetFunction`](#GetFunction): get a copy of the function
- [`Size`](#Size): get the number of objects

### Capacity
- [`Empty`](#Empty): tell whether the container is empty

### Modifiers
- [`Clear`](#Clear): clear all the data
- [`Calculate`](#Calculate): calculate new data

### Operation
- [`Query`](#Query): get the answer of certain range

## Details
### <span id="DataStructure">Data Structure</span>
Sparse table is a table that has preprocessed the answer on $2^i$ value in
each level.  Therefore, we can only calculate the answer of the prefix and
suffix that can cover the range.

To meet this feature, the function must have the following properties:
1. Function(a) = a;
2. Function(a, b) = Function(b, a);
3. Function(a, b, ...) = Function(Function(a, b), ...).

Simple example of such function: gcd, max, min.

Complexity:
- Preprocess: $O(n \times \log (n))$
- Query: $O(1)$

### <span id="Constructors">Constructors</span>
```c++
SparseTable() noexcept;
```
- Default constructor
- Construct an empty table

```c++
template<class Iterator>
SparseTable(Iterator begin, Iterator end, std::function<T(T, T)> Function);
```
- Create and calculate the sparse table in `[begin, end)`.
- If the `begin` is greater than `end`, then a
  [`lau::InvalidArgument`](exception_en.md) will be thrown.
- Please note that `end - begin` must be available.

```c++
template<class Iterator>
SparseTable(Iterator begin, SizeT size, std::function<T(T, T)> Function);
```
- Calculate the sparse table in `[begin, begin + size)`.

```c++
SparseTable(const SparseTable& obj);
```
- Copy constructor
- Copy the table and functions of `obj`.

```c++
SparseTable(SparseTable&& obj) noexcept;
```
- Move constructor
- Get the table and functions of rvalue `obj`.

### <span id="operator=">`operator=`</span>

```c++
SparseTable& operator=(const SparseTable& obj);
```
- Assign the table and function from `obj`.
- This operation will delete the previous data.

```c++
- SparseTable& operator=(SparseTable&& obj) noexcept;
```
- Assign the table and function from rvalue `obj`.
- This operation will delete the previous data.

### <span id="GetFunction">`GetFunction`</span>
```c++
[[nodiscard]] std::function<T(T, T)> GetFunction() const;
```
- Get a copy of the function.
- Please note that even if the class is cleared, the function will not
  be cleared.

### <span id="Size">`Size`</span>
```c++
[[nodiscard]] SizeT Size() const noexcept;
```
- Get the size of the origin data.

### <span id="Empty">`Empty`</span>
```c++
[[nodiscard]] bool Empty() const noexcept;
```
- Check whether there are data in this class.

### <span id="Clear">`Clear`</span>
```c++
SparseTable& Clear();
```
- Clear all the result that has been generated before.
- Please note that this operation cannot clear the function data due to lack
  of such interface.

### <span id="Calculate">`Calculate`</span>
```c++
template<class Iterator>
    SparseTable& Calculate(Iterator begin,
                           Iterator end,
                           std::function<T(T, T)> Function);
```
- Create and calculate the sparse table.
- If the argument is logically incorrect, a
  [`lau::InvalidArgument`](exception_en.md) will be thrown.
- In this operation, the table is clear anyway (including the case of throwing
  an exception).

```c++
    template<class Iterator>
    SparseTable& Calculate(Iterator begin,
                           SizeT size,
                           std::function<T(T, T)> Function);
```
- Create and calculate the sparse table.
- If the argument is logically incorrect, a
  [lau::InvalidArgument](exception_en.md) will be thrown.
- In this operation, the table is clear anyway (including the case of throwing
  an exception).

### <span id="Query">`Query`</span>
```c++
T Query(SizeT beginIndex, SizeT endIndex);
```
- Query the sparse table.
- If the index is logically incorrect, a
  [`lau::InvalidArgument`](exception_en.md) will be thrown.
- If the table is still empty, a [`lau::EmptyContainer`](exception_en.md)
   will be thrown.
