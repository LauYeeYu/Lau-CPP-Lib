# lau::RehashPolicy

Switch to Other Languages: [繁體中文（香港）](rehash_policy_zh.md)

Included in header `lau/rehash_policy.h`.

```c++
nasmepace lau {
class RehashPolicy;
}
```

`lau::RehashPolicy` is a class that gives the rehash policy of the hash table.
This class is used by the `lau::LinkedHashTable` and `lau::LinkedHashMap`
class.

## Overview
```c++
namespace lau {
class RehashPolicy {
public:
    // Constructors
    RehashPolicy() noexcept;
    explicit RehashPolicy(SizeT minimumSize);
    RehashPolicy(const RehashPolicy&) noexcept = default;

    // Assignment Operator
    RehashPolicy& operator=(const RehashPolicy&) noexcept = default;

    // Destructor
    ~RehashPolicy() = default;

    // Modifiers
    [[nodiscard]] SizeT NextSize() noexcept;
    [[nodiscard]] SizeT PreviousSize() noexcept;
    SizeT ReserveAtLeast(SizeT size);
    void SetSize(SizeT size);

    // Getter
    [[nodiscard]] SizeT GetSize() const noexcept;
    [[nodiscard]] constexpr static SizeT MaxSize() noexcept;
};
} // namespace lau
```

## Member Functions
- [(constructors)](#Constructors)
- [`operator=`](#operator=)
- (destructors)

### Modifiers
- [`NextSize`](#NextSize): set and get the larger size
- [`PreviousSize`](#PreviousSize): set and get the smaller size
- [`ReserveAtLeast`](#ReserveAtLeast): reserve at least the given size
- [`SetSize`](#SetSize): set the size

### Getter
- [`GetSize`](#GetSize): get the size
- [`MaxSize`](#MaxSize): get the maximum size

## Details
### <span id="Constructors">Constructors</span>
```c++
RehashPolicy() noexcept;
```
- Default constructor
- Set the size of the hash table to `0`.

```c++
explicit RehashPolicy(SizeT minimumSize);
```
- Constructor with the size of at least `minimumSize`.
- Note that if the `minimumSize` is larger than the maximum size, the size
  of the hash table is set to the maximum size. 

```c++
RehashPolicy(const RehashPolicy&) noexcept = default;
```
- Copy constructor
- All the data of the class will be copied.
- Since the class is trivially copyable and movable, the move constructor is
  the same as the copy constructor.

### <span id="operator=">`operator=`</span>
```c++
RehashPolicy& operator=(const RehashPolicy&) noexcept = default;
```
- Assignment operator
- All the data of the class will be copied.
- Since the class is trivially copyable and movable, the move assignment operator
  is the same as the copy assignment operator.

### <span id="NextSize">`NextSize`</span>
```c++
[[nodiscard]] SizeT NextSize() noexcept;
```
- Set and get the larger size of the hash table.
- Return the size of the hash table after this modification.
- Note that if the current size reaches the maximum size, the size will
  be set to the maximum size.
- Complexity: $O(1)$.

### <span id="PreviousSize">`PreviousSize`</span>
```c++
[[nodiscard]] SizeT PreviousSize() noexcept;
```
- Set and get the smaller size of the hash table.
- Return the size of the hash table after this modification.
- Note that if the current size reaches the minimum size, the size will
  be set to the minimum size.
- Complexity: $O(1)$.

### <span id="ReserveAtLeast">`ReserveAtLeast`</span>
```c++
SizeT ReserveAtLeast(SizeT size);
```
- Set the size to the minimum size that is larger than the given size.
- Return the size of the hash table after this modification.
- If the size is negative, a `lau::InvalidArgument` will be thrown.
- Note that if the size is larger than the maximum size, the size will
  be set to the maximum size.
- Complexity: $O(\log n)$.

### <span id="SetSize">`SetSize`</span>
```c++
void SetSize(SizeT size);
```
- Set the size to the input value.
- If the size is negative, or the size is not included in the table, a 
  `lau::InvalidArgument` will be thrown.
- It is recommended to use this function only when you have to fall back
  to the previous size.  If you don't know the previous size, you may use
  [`ReserveAtLeast`](#ReserveAtLeast) member function instead.
- Complexity: $O(\log n)$.

### <span id="GetSize">`GetSize`</span>
```c++
[[nodiscard]] SizeT GetSize() const noexcept;
```
- Get the size of the hash table.
- Complexity: $O(1)$.

### <span id="MaxSize">`MaxSize`</span>
```c++
[[nodiscard]] constexpr static SizeT MaxSize() noexcept;
```
- Get the maximum size of the hash table.
- Complexity: $O(1)$.
