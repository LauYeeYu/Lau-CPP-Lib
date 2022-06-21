# lau::Pair

Switch to Other Languages: [繁體中文（香港）](pair_zh.md)

Included in header `lau/ultility.h`.

```c++
nasmepace lau {
template<class T1, class T2>
class Pair;
}
```

`lau::Pair` is a class that stores two elements `first` and `second`.

## Overview
```c++
namespace lau {

template<class T1, class T2>
class Pair {
public:
    Pair() noexcept(noexcept(T1()) && noexcept(T2())) : first(), second() {}
    Pair(const Pair& obj) = default;

    Pair(Pair&& obj)
        noexcept(noexcept(T1(std::move(obj.first))) && noexcept(T2(std::move(obj.second))))
        = default;

    Pair(const T1& firstIn, const T2& secondIn) noexcept(noexcept(T1(firstIn)) && noexcept(T2(secondIn)))
        : first(firstIn), second(secondIn) {}

    template<class U1, class U2>
    Pair(U1&& firstIn, U2&& secondIn) noexcept(noexcept(T1(firstIn)) && noexcept(T2(secondIn)))
        : first(firstIn), second(secondIn) {}

    template<class U1, class U2>
    Pair(const Pair<U1, U2>& obj)
        noexcept(noexcept(T1(obj.first)) && noexcept(T2(obj.second)))
        : first(obj.first), second(obj.second) {}

    template<class U1, class U2>
    Pair(Pair<U1, U2>&& obj)
        noexcept(noexcept(T1(std::move(obj.first))) && noexcept(std::move(T2(obj.second))))
        : first(obj.first), second(obj.second) {}

    Pair& operator=(const Pair& obj)  = default;
    Pair& operator=(Pair&& obj) = default;

    ~Pair() = default;

    bool operator==(const Pair& obj) const;
    bool operator!=(const Pair& obj) const;

    T1 first;
    T2 second;
};

template<class T1, class T2>
Pair<T1, T2> MakePair(const T1& firstIn, const T2& secondIn);

} // namespace lau
```

## Template
- `T1`: the type of the first element
- `T2`: the type of the second element

## Member Functions
- [(constructors)](#Constructors)
- [`operator=`](#operator=)
- (destructors)

### Comparison
- [`operator==`](#operator==)
- [`operator!=`](#operator!=)

### Non-member Function
- [`MakePair`](#MakePair)

## Details
### <span id="Constructors">Constructors</span>
```c++
Pair() noexcept(noexcept(T1()) && noexcept(T2())) : first(), second() {}
```
- Default constructor: user the default constructor of `T1` and `T2`.

```c++
Pair(const Pair& obj) = default;
```
- Copy constructor: copy the first and second elements of `obj`.

```c++
Pair(Pair&& obj)
    noexcept(noexcept(T1(std::move(obj.first))) && noexcept(T2(std::move(obj.second))))
    = default;
```
- Move constructor: move the first and second elements of `obj`.

```c++
Pair(const T1& firstIn, const T2& secondIn) noexcept(noexcept(T1(firstIn)) && noexcept(T2(secondIn)))
    : first(firstIn), second(secondIn) {}
```
- Initialize the first and second elements with `firstIn` and `secondIn`.

```c++
template<class U1, class U2>
Pair(U1&& firstIn, U2&& secondIn) noexcept(noexcept(T1(firstIn)) && noexcept(T2(secondIn)))
    : first(firstIn), second(secondIn) {}
```
- Initialize the first and second elements with the rvalue `firstIn` and `secondIn`. 
- The `U1` should be either `T1` or can be converted to `T1`.
- The `U2` should be either `T2` or can be converted to `T2`.

```c++
template<class U1, class U2>
Pair(const Pair<U1, U2>& obj)
    noexcept(noexcept(T1(obj.first)) && noexcept(T2(obj.second)))
    : first(obj.first), second(obj.second) {}
```
- Initialize the first and second elements with the elements of `obj`.
- The `U1` should be either `T1` or can be converted to `T1`.
- The `U2` should be either `T2` or can be converted to `T2`.

```c++
template<class U1, class U2>
Pair(Pair<U1, U2>&& obj)
    noexcept(noexcept(T1(std::move(obj.first))) && noexcept(std::move(T2(obj.second))))
    : first(obj.first), second(obj.second) {}
```
- Initialize the first and second elements with the elements of `obj`.
- The `U1` should be either `T1` or can be converted to `T1`.
- The `U2` should be either `T2` or can be converted to `T2`.

### <span id="operator=">operator=</span>
```c++
Pair& operator=(const Pair& obj) = default;
```
- Assign the first and second elements of `obj`.

```c++
Pair& operator=(Pair&& obj) = default;
```
- Assign the first and second elements of rvalue `obj`.

### <span id="operator==">operator==</span>
```c++
bool operator==(const Pair& obj) const;
```
- Return whether the first and second elements are equal to the first and
  second elements of `obj`.

### <span id="operator!=">operator!=</span>
```c++
bool operator!=(const Pair& obj) const;
```
- Return whether the first and second elements are not equal to the first and
  second elements of `obj`.

### <span id="MakePair">MakePair</span>
```c++
template<class T1, class T2>
Pair<T1, T2> MakePair(const T1& firstIn, const T2& secondIn);
```
- Return a `Pair` with the first and second elements initialized with `firstIn` and `secondIn`.
- The `T1` and `T2` can be deduced from the arguments.
