# lau::Map

切換到其他語言： [English](map_en.md)

包含於標頭檔 `lau/map.h` 中。

```c++
namespace lau {
template<class Key,
         class Value,
         class Compare   = std::less<Key>,
         class Allocator = std::allocator<KeyValuePair<const Key, Value>>>
class Map;
} // namespace lau
```

`lau::Map` 是一個映射容器，使用[紅黑樹](https://zh.wikipedia.org/wiki/%E7%BA%A2%E9%BB%91%E6%A0%91)作爲底層數據結構。

您可以自定義比較函數，以決定比較兩個鍵的順序。注意：`Compare`
必須重載 `bool operator(const T&, const T&) const` 運算符，並保證鍵的順序並不會中途改變。

注意：此類不是數據結構類，鍵值對中的鍵一經插入，就無法被修改。如果需要修改鍵，請移除原先的鍵值對或使用
`lau::RBTree`。`lau::RBTree` 是一個數據結構類，可以自行修改鍵。

欲瞭解關於數據結構的更多訊息，請檢視[數據結構詳情](#DataStructure)。

## 概覽
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

    // 構造函數
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

    // 複製運算符
    Map& operator=(const Map& other);
    Map& operator=(Map&& other) noexcept;

    // 析構函數
    ~Map() = default;

    // 元素訪問
    [[nodiscard]] Value& At(const Key& key);
    [[nodiscard]] const Value& At(const Key& key) const;
    Value& operator[](const Key& key);
    Value& operator[](Key&& key);
    [[nodiscard]] const Value& operator[](const Key& key) const;

    // 修改
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

    // 容量
    [[nodiscard]] SizeT Size() const noexcept;
    [[nodiscard]] bool Empty() const noexcept;
    [[nodiscard]] long MaxSize() const noexcept;

    // 獲取
    [[nodiscard]] Allocator GetAllocator() const noexcept;
    [[nodiscard]] Compare GetKeyCompare() const;
    [[nodiscard]] PairCompare GetPairCompare() const;

    // 查找
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

    // 迭代器
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

// 非成員函數
template<class Key, class Value, class Compare, class Allocator>
void Swap(Map<Key, Value, Compare, Allocator>& lhs,
          Map<Key, Value, Compare, Allocator>& rhs) noexcept;
} // namespace lau
```
## 模板
- `Key`：鍵的類型
- `Value`：值的類型
- `Compare`：比較類之類型（默認爲 `std::less<T>`）
  - 需要符合 C++ 對於比較類之要求（參見 [C++ 具名要求：比较 (Compare)](https://zh.cppreference.com/w/cpp/named_req/Compare)）
    - 必須具有合法的 `bool Compare(const T& lhs, const T& rhs)`。
    - 保證唯一性（若 `Compare(a, b) = false` 且 `Compare(a, b) = false`，或
      `Compare(a, b) = true` 且 `Compare(a, b) = true`，則 `a` 與 `b` 相同）
- `Allocator`：記憶體分配器之類型（默認爲 `std::allocator`）
  - 需要符合 C++ 對於記憶體分配器之要求（參見 [C++ 具名要求：分配器 (Allocator)](https://zh.cppreference.com/w/cpp/named_req/Allocator)）
  - 默認爲 `std::allocator<KeyValuePair<const Key, Value>>`。

## 成員類型
- `MapPair`：`KeyValuePair<const Key, Value>`
- `value_type`：`KeyValuePair<const Key, Value>`

- `PairCompare`：鍵值對的比較類
```c++
class PairCompare {
    public:
        PairCompare() = default;
        explicit PairCompare(const Compare&);

        bool operator()(const MapPair& lhs, const MapPair& rhs) const;
        template<class K>
        bool operator()(const K& lhs, const MapPair& rhs) const;
        template<class K>
        bool operator()(const MapPair& lhs, const K& rhs) const;
    };
```
- [`Iterator`](#iterator)：`lau::Map` 的迭代器類型
- [`ConstIterator`](#iterator)：`lau::Map` 的常量迭代器類型

## 成員函數
- [（構造函數）](#Constructors)
- [`operator=`](#operator=)
- （析構函數）

### 修改
- [`Insert`](#Insert)：插入一個鍵值對
- [`Emplace`](#Emplace)：原位插入一個鍵值對
- [`Erase`](#Erase)：移除一個鍵值對
- [`Clear`](#Clear)：清除所有鍵值對
- [`Swap`](#Swap)：交換類

### 容量
- [`Size`](#Size)：取得鍵值對的數量
- [`Empty`](#Empty)：檢查容器是否為空
- [`MaxSize`](#MaxSize)：取得容器的最大容量

### 獲取
- [`GetAllocator`](#GetAllocator)：取得記憶體分配器
- [`GetKeyCompare`](#GetKeyCompare)：取得鍵比較器
- [`GetPairCompare`](#GetPairCompare)：取得鍵值對比較器

### 查找
- [`Contains`](#Contains)：檢查是否包含某個鍵
- [`Find`](#Find)：查找某個鍵
- [`LowerBound`](#LowerBound)：查找首個不小於輸入元素的元素位置
- [`UpperBound`](#UpperBound)：查找首個大於輸入元素的元素位置

### 迭代器
- [`Begin`](#Begin)：取得首迭代器
- [`begin`](#begin)：取得首迭代器
- [`ConstBegin`](#ConstBegin)：取得首常量迭代器
- [`End`](#End)：取得末迭代器
- [`end`](#end)：取得末迭代器
- [`ConstEnd`](#ConstEnd)：取得末常量迭代器

## 非成員函數
- [`Swap`](#SwapNonmember)：交換元素

## 詳情
### <span id="DataStructure">數據結構</span>
此類使用紅黑樹作爲底層數據結構。紅黑樹是一種自平衡二叉搜索樹。與普通平衡二叉樹不同的是，其使用一種特別的方式來減少調整平衡之次數。其僅僅保證任一節點之左子樹的深度大於右子樹的深度的一半，小於右子樹深度的兩倍。如欲瞭解更多關於此樹之信息，請訪問[維基百科頁面](https://zh.wikipedia.org/wiki/%E7%BA%A2%E9%BB%91%E6%A0%91)。

時間複雜度：
- 訪問： $O(\log n)$
- 插入： $O(\log n)$
- 移除： $O(\log n)$
- 遍歷： $O(n)$

### <span id="Constructors">構造函數</span>
```c++
Map();
```
- 默認構造函數
- 以默認比較類、默認記憶體分配器建立一個空容器。

```c++
explicit Map(const Compare& compare);
```
- 以輸入之比較類、默認記憶體分配器建立一個空容器。

```c++
explicit Map(const Allocator& allocator);
```
- 以輸入之比較類、記憶體分配器建立一個空容器。

```c++
Map(const Compare& compare, const Allocator& allocator);
```
- 以輸入之比較類、記憶體分配器建立一個空容器。

```c++
template<class InputIterator>
Map(InputIterator begin, InputIterator end);
```
- 以 `[begin, end)` 之間的元素、默認比較類、默認記憶體建立此容器。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
template<class InputIterator>
Map(InputIterator begin, InputIterator end, const Compare& compare);
```
- 以 `[begin, end)` 之間的元素、輸入之比較類、默認記憶體建立此容器。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
template<class InputIterator>
Map(InputIterator begin, InputIterator end, const Allocator& allocator);
```
- 以 `[begin, end)` 之間的元素、輸入之比較類、記憶體分配器建立此容器。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
template<class InputIterator>
Map(InputIterator begin, InputIterator end,
    const Compare& compare,
    const Allocator& allocator);
```
- 以 `[begin, end)` 之間的元素、輸入之比較類、記憶體分配器建立此容器。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
Map(const Map& obj);
```
- 複製構造函數
- 建立一個與 `obj` 所含元素、記憶體分配器以及比較類相同的類。
- 時間複雜度： $O(n)$。（$n$ 是範圍中的元素個數）

```c++
Map(const Map& obj, const Allocator& allocator);
```
- 具有自定義記憶體分配器的複製構造函數
- 建立一個與 `obj` 所含元素以及比較類相同的類。
- 其中元素透過輸入之記憶體分配器 `allocator` 分配記憶體。
- 時間複雜度： $O(n)$。（$n$ 是範圍中的元素個數）

```c++
Map(Map&& obj) noexcept;
```
- 移動構造函數
- 建立一個與 `obj` 所含元素、記憶體分配器以及比較類相同的類。
- 時間複雜度： $O(1)$。

```c++
Map(std::initializer_list<MapPair> init);
```
- 以 `init` 內元素、默認比較類、默認記憶體建立此容器。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
Map(std::initializer_list<MapPair> init, const Compare& compare);
```
- 以 `init` 內元素、輸入之比較類、默認記憶體建立此容器。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
Map(std::initializer_list<MapPair> init, const Allocator& allocator);
```
- 以 `init` 內元素、默認比較類、輸入之記憶體建立此容器。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
Map(std::initializer_list<MapPair> init,
    const Compare& compare,
    const Allocator& allocator);
```
- 以 `init` 內元素、輸入之比較類、輸入之記憶體建立此容器。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

### <span id="operator=">`operator=`</span>
```c++
Map& operator=(const Map& obj);
```
- 建立一個與 `obj` 所含元素、記憶體分配器以及比較類相同的類。
- 時間複雜度： $O(n)$。（$n$ 是範圍中的元素個數）

```c++
Map& operator=(Map&& obj) noexcept;
```
- 建立一個與 `obj` 所含元素、記憶體分配器以及比較類相同的類。
- 時間複雜度： $O(1)$。

### <span id="Insert">`Insert`</span>
```c++
Pair<Iterator, bool> Insert(const MapPair& pair);
```
- 將 `pair` 插入容器。
- 返回一個 `Pair`，其中 `first` 爲新插入的元素的位置（如果插入成功）或阻止
  `value` 插入的位置（如果插入失敗），`second` 爲是否成功。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

```c++
Pair<Iterator, bool> Insert(MapPair&& pair);
```
- 將右值 `pair` 插入容器。
- 返回一個 `Pair`，其中 `first` 爲新插入的元素的位置（如果插入成功）或阻止
  `value` 插入的位置（如果插入失敗），`second` 爲是否成功。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

### <span id="Emplace">`Emplace`</span>
```c++
template<class... Args>
Pair<Iterator, bool> Emplace(Args&&... args);
```
- 將元素（從 `args...` 構造）插入容器。
- 返回一個 `Pair`，其中 `first` 爲新插入的元素的位置（如果插入成功）或阻止
  `value` 插入的位置（如果插入失敗），`second` 爲是否成功。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

### <span id="Erase">`Erase`</span>
```c++
Map& Erase(const Iterator& position);
```
- 移除 `position` 所指的元素。
- 如果 `position` 指向的元素不是容器中的元素或者迭代器不是末迭代器，則會拋出
  `lau::InvalidIterator` 异常。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(\log n)$。（平均值，$n$ 是容器中元素個數）

```c++
Map& Erase(const Key& key);
```
- 移除容器中鍵與 `key` 相等的元素。
- 如果元素不是容器中的元素，則會拋出 `lau::InvalidIterator` 异常。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

```c++
template<class K>
Map& Erase(const K& key);
```
- 移除容器中鍵與 `key` 相等的元素。
- 如果元素不是容器中的元素，則會拋出 `lau::InvalidIterator` 异常。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

### <span id="Clear">`Clear`</span>
```c++
Map& Clear() noexcept;
```
- 清除容器中所有元素。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(n)$。（$n$ 是容器中元素個數）

### <span id="Swap">`Swap`</span>
```c++
Map& Swap(Map& other) noexcept;
```
- 交換元素。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(1)$。

### <span id="Size">`Size`</span>
```c++
[[nodiscard]] SizeT Size() const noexcept;
```
- 取得容器中元素個數。
- 時間複雜度： $O(1)$。

### <span id="Empty">`Empty`</span>
```c++
[[nodiscard]] bool Empty() const noexcept;
```
- 檢查容器是否為空。
- 時間複雜度： $O(1)$。

### <span id="MaxSize">`MaxSize`</span>
```c++
[[nodiscard]] long MaxSize() const noexcept;
```
- 取得容器可容納的最大元素個數。

### <span id="GetAllocator">`GetAllocator`</span>
```c++
[[nodiscard]] Allocator GetAllocator() const noexcept;
```
- 取得容器的記憶體分配器副本。

### <span id="GetKeyCompare">`GetKeyCompare`</span>
```c++
[[nodiscard]] Compare GetKeyCompare() const;
```
- 取得容器的鍵比較器副本。

### <span id="GetPairCompare">`GetPairCompare`</span>
```c++
[[nodiscard]] PairCompare GetPairCompare() const;
```
- 取得容器的元素比較器副本。

### <span id="Contains">`Contains`</span>
```c++
[[nodiscard]] bool Contains(const Key& key) const;
```
- 檢查容器中是否有鍵與 `key` 相等的元素。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

```c++
template<class K>
[[nodiscard]] bool Contains(const K& key) const;
```
- 檢查容器中是否有鍵與 `key` 相等的元素。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

### <span id="Find">`Find`</span>
```c++
[[nodiscard]] Iterator Find(const Key& key);
```
- 取得容器中與 `key` 相等的元素的迭代器。
- 如果元素不是容器中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

```c++
[[nodiscard]] ConstIterator Find(const Key& key) const;
```
- 取得容器中與 `key` 相等的元素的常量迭代器。
- 如果元素不是容器中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

```c++
template<class K>
[[nodiscard]] Iterator Find(const K& key);
```
- 取得容器中與 `key` 相等的元素的迭代器。
- 如果元素不是容器中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

```c++
template<class K>
[[nodiscard]] ConstIterator Find(const K& key) const;
```
- 取得容器中與 `key` 相等的元素的常量迭代器。
- 如果元素不是容器中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

### <span id="LowerBound">`LowerBound`</span>
```c++
[[nodiscard]] Iterator LowerBound(const Key& key);
```
- 取得容器中不小於（大於等於）`key` 的最小元素的迭代器。
- 如果元素不是容器中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

```c++
[[nodiscard]] ConstIterator LowerBound(const Key& key) const;
```
- 取得容器中不小於（大於等於）`key` 的最小元素的常量迭代器。
- 如果元素不是容器中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

```c++
template<class K>
[[nodiscard]] Iterator LowerBound(const K& key);
```
- 取得容器中不小於（大於等於）`key` 的最小元素的迭代器。
- 如果元素不是容器中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

```c++
template<class K>
[[nodiscard]] ConstIterator LowerBound(const K& key) const;
```
- 取得容器中不小於（大於等於）`key` 的最小元素的常量迭代器。
- 如果元素不是容器中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

### <span id="UpperBound">`UpperBound`</span>
```c++
[[nodiscard]] Iterator UpperBound(const Key& key);
```
- 取得容器中大於 `key` 的最小元素的迭代器。
- 如果元素不是容器中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

```c++
[[nodiscard]] ConstIterator UpperBound(const Key& key) const;
```
- 取得容器中大於`key` 的最小元素的常量迭代器。
- 如果元素不是容器中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

```c++
template<class K>
[[nodiscard]] Iterator UpperBound(const K& key);
```
- 取得容器中大於 `key` 的最小元素的迭代器。
- 如果元素不是容器中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

```c++
template<class K>
[[nodiscard]] ConstIterator UpperBound(const K& key) const;
```
- 取得容器中大於 `key` 的最小元素的常量迭代器。
- 如果元素不是容器中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是容器中元素個數）

### <span id="Begin">`Begin`</span>
```c++
[[nodiscard]] Iterator Begin() noexcept;
```
- 取得容器中第一個元素的迭代器。
- 時間複雜度： $O(1)$。

```c++
[[nodiscard]] ConstIterator Begin() const noexcept;
```
- 取得容器中第一個元素的常量迭代器。
- 時間複雜度： $O(1)$。

### <span id="begin">`begin`</span>
```c++
[[nodiscard]] Iterator begin() noexcept;
```
- 取得容器中第一個元素的迭代器。
- 時間複雜度： $O(1)$。

```c++
[[nodiscard]] ConstIterator begin() const;
```
- 取得容器中第一個元素的常量迭代器。
- 時間複雜度： $O(1)$。

### <span id="ConstBegin">`ConstBegin`</span>
```c++
[[nodiscard]] ConstIterator ConstBegin() const noexcept;
```
- 取得容器中第一個元素的常量迭代器。
- 時間複雜度： $O(1)$。

### <span id="End">`End`</span>
```c++
[[nodiscard]] Iterator End() noexcept;
```
- 取得容器的末迭代器。
- 注意：末迭代器不指向容器中任何元素，而是指向最後一個元素之後的位置。
- 時間複雜度： $O(1)$。

```c++
[[nodiscard]] ConstIterator End() const noexcept;
```
- 取得容器的常量末迭代器。
- 注意：末迭代器不指向容器中任何元素，而是指向最後一個元素之後的位置。
- 時間複雜度： $O(1)$。

### <span id="end">`end`</span>
```c++
[[nodiscard]] Iterator end() noexcept;
```
- 取得容器的末迭代器。
- 注意：末迭代器不指向容器中任何元素，而是指向最後一個元素之後的位置。
- 時間複雜度： $O(1)$。

```c++
[[nodiscard]] ConstIterator end() const noexcept;
```
- 取得容器的常量末迭代器。
- 注意：末迭代器不指向容器中任何元素，而是指向最後一個元素之後的位置。
- 時間複雜度： $O(1)$。

### <span id="ConstEnd">`ConstEnd`</span>
```c++
[[nodiscard]] ConstIterator ConstEnd() const;
```
- 取得容器的常量末迭代器。
- 注意：末迭代器不指向容器中任何元素，而是指向最後一個元素之後的位置。
- 時間複雜度： $O(1)$。

### <span id="SwapNonmember">`Swap`</span>
```c++
template<class Key, class Value, class Compare, class Allocator>
void Swap(Map<Key, Value, Compare, Allocator>& lhs,
          Map<Key, Value, Compare, Allocator>& rhs) noexcept;
```
- 交換兩容器。
- 時間複雜度： $O(1)$。

### <span id="iterator">迭代器</span>
成員類型：
- `difference_type`: `std::ptrdiff_t`
- `value_type`: `MapPair`
- `pointer`: `MapPair*`
- `reference`: `MapPair&`
- `iterator_category`: `std::output_iterator_tag`


支援的操作：（以下所列之迭代器可以是 `Iterator` 或 `ConstIterator` 中任一）
- `iterator++`
- `++iterator`
- `iterator--`
- `--iterator`
- `*iterator`
- `iterator->...`
- `iterator1 == iterator2`
- `iterator1 != iterator2`

非法化：
- 移除元素：指向移除元素的迭代器
- 清空容器：除末迭代器以外的全部迭代器
