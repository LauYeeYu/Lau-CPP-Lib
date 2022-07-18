# lau::Pair

Switch to Other Languages: [繁體中文（香港）](pair_zh.md)

包含於標頭檔 `lau/ultility.h`。

```c++
nasmepace lau {
template<class T1, class T2>
class Pair;
}
```

`lau::Pair` 儲存兩個元素，分別名爲 `first` and `second`.

## 概覽
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
- `T1`：第一個元素之類型
- `T2`：第二個元素之類型

## 成員函數
- [（構造函數）](#Constructors)
- [`operator=`](#operator=)
- （析構函數）

### 比較
- [`operator==`](#operator==)
- [`operator!=`](#operator!=)

### 非成員函數
- [`MakePair`](#MakePair)：建立一個 `Pair` 實體

## Details
### <span id="Constructors">Constructors</span>
```c++
Pair() noexcept(noexcept(T1()) && noexcept(T2())) : first(), second() {}
```
- 默認構造函數

```c++
Pair(const Pair& obj) = default;
```
- 拷貝構造函數

```c++
Pair(Pair&& obj)
    noexcept(noexcept(T1(std::move(obj.first))) && noexcept(T2(std::move(obj.second))))
    = default;
```
- 移動構造函數

```c++
Pair(const T1& firstIn, const T2& secondIn) noexcept(noexcept(T1(firstIn)) && noexcept(T2(secondIn)))
    : first(firstIn), second(secondIn) {}
```
- 以 `firstIn` 和 `secondIn` 的值建立一個 `Pair` 實體。

```c++
template<class U1, class U2>
Pair(U1&& firstIn, U2&& secondIn) noexcept(noexcept(T1(firstIn)) && noexcept(T2(secondIn)))
    : first(firstIn), second(secondIn) {}
```
- 以 `firstIn` 和 `secondIn` 的值建立一個 `Pair` 實體。
- `U1` 和 `U2` 必須可轉換為 `T1` 和 `T2` 各自的類型。

```c++
template<class U1, class U2>
Pair(const Pair<U1, U2>& obj)
    noexcept(noexcept(T1(obj.first)) && noexcept(T2(obj.second)))
    : first(obj.first), second(obj.second) {}
```
- 以 `obj` 的值建立一個 `Pair` 實體。
- `U1` 和 `U2` 必須可轉換為 `T1` 和 `T2` 各自的類型。

```c++
template<class U1, class U2>
Pair(Pair<U1, U2>&& obj)
    noexcept(noexcept(T1(std::move(obj.first))) && noexcept(std::move(T2(obj.second))))
    : first(obj.first), second(obj.second) {}
```
- 以右值 `obj` 的值建立一個 `Pair` 實體。
- `U1` 和 `U2` 必須可轉換為 `T1` 和 `T2` 各自的類型。

### <span id="operator=">operator=</span>
```c++
Pair& operator=(const Pair& obj) = default;
```
- 將此鍵值對設定爲 `obj` 的值。

```c++
Pair& operator=(Pair&& obj) = default;
```
- 將此鍵值對設定爲右值 `obj` 的值。

### <span id="operator==">operator==</span>
```c++
bool operator==(const Pair& obj) const;
```
- 判斷 `first` 以及 `second` 是否爲 `obj` 的對應成員的值。

### <span id="operator!=">operator!=</span>
```c++
bool operator!=(const Pair& obj) const;
```
- 判斷 `first` 以及 `second` 是否爲 `obj` 的對應成員的值。

### <span id="MakePair">MakePair</span>
```c++
template<class T1, class T2>
Pair<T1, T2> MakePair(const T1& firstIn, const T2& secondIn);
```
- 建立一個 `Pair` 實體。
- `T1` 和 `T2` 可以透過模板類型推斷得出。
