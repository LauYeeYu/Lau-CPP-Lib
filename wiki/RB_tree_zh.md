# lau::RBTree

切換到其他語言： [English](RB_tree_en.md)

包含於標頭檔 `lau/RB_tree.h` 中。

```c++
namespace lau {
template<class T,
         class Compare = std::less<T>,
         class Allocator = std::allocator<T>>
class RBTree;
} // namespace lau
```

`lau::RBTree` 使用
[紅黑樹](https://zh.wikipedia.org/wiki/%E7%BA%A2%E9%BB%91%E6%A0%91)作爲數據結構。此樹爲有序容器，搜索、插入以及移除操作皆僅需對數複雜度。

您可以針對排序方式自定義比較方法（`Comapre` 類）。注意 `Compare` 類必須支援
`bool operator(const T&, const T&) const` 函數，並保證此函數能始終保證唯一的全序關係。

注意：此類爲數據結構的封裝實現，因此透過迭代器，用戶可以有更多自由空間來修改大多數數據，即便此操作可能會導致未定義行爲。絕大多數情況下，只要不影響
`Compare` 類所確定的全序關係，並不會產生任何問題。但是如果你需要保證不會出現未定義行爲，建議使用
`lau::Map` 類。

欲瞭解關於紅黑樹的更多訊息，請檢視[數據結構詳情](#DataStructure)。

## 概覽
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

    // 構造函數
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

    // 賦值運算符
    RBTree& operator=(const RBTree& obj);
    RBTree& operator=(RBTree&& obj) noexcept;

    // 析構函數
    ~RBTree();

    // 修改
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

    // 容量
    [[nodiscard]] SizeT Size() const noexcept;
    [[nodiscard]] bool Empty() const noexcept;
    [[nodiscard]] long MaxSize() const noexcept;

    // 獲取
    [[nodiscard]] AllocatorType GetAllocator() const noexcept;
    [[nodiscard]] Compare GetCompare() const;

    // 查找
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

    // 迭代器
    [[nodiscard]] Iterator Begin() noexcept;
    [[nodiscard]] ConstIterator Begin() const noexcept;
    [[nodiscard]] Iterator begin();
    [[nodiscard]] ConstIterator begin() const;
    [[nodiscard]] ConstIterator ConstBegin() const noexcept;
    [[nodiscard]] Iterator End() noexcept;
    [[nodiscard]] ConstIterator End() const noexcept;
    [[nodiscard]] Iterator end();
    [[nodiscard]] ConstIterator end() const noexcept;
    [[nodiscard]] ConstIterator ConstEnd() const;
};

// 非成員函數
template <class T, class Compare, class Allocator>
void Swap(RBTree<T, Compare, Allocator>& lhs,
          RBTree<T, Compare, Allocator>& rhs) noexcept;
} // namespace lau
```
## 模板
- `T`：元素的類型
- `Compare`：比較類之類型（默認爲 `std::less<T>`）
  - 需要符合 C++ 對於比較類之要求（參見 [C++ 具名要求：比较 (Compare)](https://zh.cppreference.com/w/cpp/named_req/Compare)）
    - 必須具有合法的 `bool Compare(const T& lhs, const T& rhs)`。
    - 保證唯一性（若 `Compare(a, b) = false` 且 `Compare(a, b) = false`，或
      `Compare(a, b) = true` 且 `Compare(a, b) = true`，則 `a` 與 `b` 相同）
- `Allocator`：記憶體分配器之類型（默認爲 `std::allocator`）
  - 需要符合 C++ 對於記憶體分配器之要求（參見 [C++ 具名要求：分配器 (Allocator)](https://zh.cppreference.com/w/cpp/named_req/Allocator)）
  - 默認爲 `std::allocator`。

## 枚舉類型
- `Flag`：樹節點的顏色
  - `red`
  - `black`

## 成員類型
- `Node`：紅黑樹之節點
  - 可以透過此類之接口來檢視樹結構。 
  - 代碼結構如下所列。
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
- `AllocatorType`：`Node` 類之記憶體分配器類型
- [`Iterator`](#iterator)：`lau::RBTree` 之迭代器類型
- [`ConstIterator`](#iterator)：`lau::RBTree` 之唯讀迭代器類型

## 成員函數
- [（構造函數）](#Constructors)
- [`operator=`](#operator=)
- （析構函數）

### 修改
- [`Insert`](#Insert)：插入元素
- [`Emplace`](#Emplace)：以原位構造方式插入元素
- [`Erase`](#Erase)：移除元素
- [`Clear`](#Clear)：清空此樹
- [`Swap`](#Swap)：交換元素

### 容量
- [`Size`](#Size)：取得所存之元素數量
- [`Empty`](#Empty)：檢查容器是否爲空
- [`MaxSize`](#MaxSize)：取得容器最大元素數量

### 獲取
- [`GetAllocator`](#GetAllocator)：取得記憶體分配器
- [`GetCompare`](#GetCompare)：取得比較類

### 查找
- [`Contains`](#Contains)：檢查是否存在對應元素
- [`Find`](#Find)：查找對應元素
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
紅黑樹是一種自平衡二叉搜索樹。與普通平衡二叉樹不同的是，其使用一種特別的方式來減少調整平衡之次數。其僅僅保證任一節點之左子樹的深度大於右子樹的深度的一半，小於右子樹深度的兩倍。如欲瞭解更多關於此樹之信息，請訪問[維基百科頁面](https://zh.wikipedia.org/wiki/%E7%BA%A2%E9%BB%91%E6%A0%91)。

時間複雜度：
- 訪問： $O(\log n)$
- 插入： $O(\log n)$
- 移除： $O(\log n)$
- 遍歷： $O(n)$

### <span id="Constructors">構造函數</span>
```c++
RBTree();
```
- 默認構造函數
- 以默認比較類、默認記憶體分配器建立一個空的紅黑樹。

```c++
explicit RBTree(const Compare& compare);
```
- 以輸入之比較類、默認記憶體分配器建立一個空的紅黑樹。

```c++
explicit RBTree(const Allocator& allocator);
```
- 以默認比較類、輸入之記憶體分配器建立一個空的紅黑樹。

```c++
RBTree(const Compare& compare, const Allocator& allocator);
```
- 以輸入之比較類、輸入之記憶體分配器建立一個空的紅黑樹。

```c++
template<class InputIterator>
RBTree(InputIterator begin, InputIterator end);
```
- 以 `[begin, end)` 之間的元素、默認比較類、默認記憶體建立一個紅黑樹。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
template<class InputIterator>
RBTree(InputIterator begin, InputIterator end, const Compare& compare);
```
- 以 `[begin, end)` 之間的元素、輸入之比較類、默認記憶體建立一個紅黑樹。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
template<class InputIterator>
RBTree(InputIterator begin, InputIterator end, const Allocator& allocator);
```
- 以 `[begin, end)` 之間的元素、默認比較類、輸入之記憶體建立一個紅黑樹。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
template<class InputIterator>
RBTree(InputIterator begin, InputIterator end,
       const Compare& compare,
       const Allocator& allocator);
```
- 以 `[begin, end)` 之間的元素、輸入之比較類、輸入之記憶體建立一個紅黑樹。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
RBTree(const RBTree& obj);
```
- 複製構造函數
- 建立一個與 `obj` 所含元素、記憶體分配器以及比較類相同的類。
- 時間複雜度： $O(n)$。（$n$ 是範圍中的元素個數）

```c++
RBTree(const RBTree& obj, const Allocator& allocator);
```
- 具有自定義記憶體分配器的複製構造函數
- 建立一個與 `obj` 所含元素以及比較類相同的類。
- 其中元素透過輸入之記憶體分配器 `allocator` 分配記憶體。
- 時間複雜度： $O(n)$。（$n$ 是範圍中的元素個數）

```c++
RBTree(RBTree&& obj) noexcept;
```
- 移動構造函數
- 建立一個與 `obj` 所含元素、記憶體分配器以及比較類相同的類。
- 時間複雜度： $O(1)$。

```c++
RBTree(std::initializer_list<T> init);
```
- 以 `init` 內元素、默認比較類、默認記憶體建立一個紅黑樹。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
RBTree(std::initializer_list<T> init,
       const Compare& compare);
```
- 以 `init` 內元素、輸入之比較類、默認記憶體建立一個紅黑樹。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
RBTree(std::initializer_list<T> init,
       const Allocator& allocator);
```
- 以 `init` 內元素、默認比較類、輸入之記憶體建立一個紅黑樹。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

```c++
RBTree(std::initializer_list<T> init,
       const Compare& compare,
       const Allocator& allocator);
```
- 以 `init` 內元素、輸入之比較類、輸入之記憶體建立一個紅黑樹。
- 時間複雜度： $O(n \log n)$。（$n$ 是範圍中的元素個數）

### <span id="operator=">operator=</span>
```c++
RBTree& operator=(const RBTree& obj);
```
- 複製賦值運算符
- 建立一個與 `obj` 所含元素、記憶體分配器以及比較類相同的類。
- 時間複雜度： $O(n)$。（$n$ 是範圍中的元素個數）

```c++
RBTree& operator=(RBTree&& obj) noexcept;
```
- 建立一個與 `obj` 所含元素、記憶體分配器以及比較類相同的類。
- 時間複雜度： $O(1)$。

### <span id="Insert">`Insert`</span>
```c++
Pair<Iterator, bool> Insert(const T& value);
```
- 將 `value` 插入紅黑樹中。
- 返回一個 `Pair`，其中 `first` 爲新插入的元素的位置（如果插入成功）或阻止
  `value` 插入的位置（如果插入失敗），`second` 爲是否成功。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

```c++
Pair<Iterator, bool> Insert(T&& value);
```
- 將右值 `value` 插入紅黑樹中。
- 返回一個 `Pair`，其中 `first` 爲新插入的元素的位置（如果插入成功）或阻止
  `value` 插入的位置（如果插入失敗），`second` 爲是否成功。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

### <span id="Emplace">`Emplace`</span>
```c++
template<class... Args>
Pair<Iterator, bool> Emplace(Args&&... args);
```
- 將元素（從 `args...` 構造）插入紅黑樹中。
- 返回一個 `Pair`，其中 `first` 爲新插入的元素的位置（如果插入成功）或阻止
  `value` 插入的位置（如果插入失敗），`second` 爲是否成功。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

### <span id="Erase">`Erase`</span>
```c++
RBTree& Erase(const Iterator& position);
```
- 刪除 `position` 所指的元素。
- 如果 `position` 指向的元素不是紅黑樹中的元素或者迭代器不是末迭代器，則會拋出
  `lau::InvalidIterator` 异常。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(\log n)$。（平均值，$n$ 是樹中元素個數）

```c++
RBTree& Erase(const T& value);
```
- 刪除紅黑樹中與 `value` 相等的元素。
- 如果元素不是紅黑樹中的元素，則會拋出 `lau::InvalidIterator` 异常。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

```c++
template<class K>
RBTree& Erase(const K& value);
```
- 刪除紅黑樹中與 `value` 相等的元素。
- 如果元素不是紅黑樹中的元素，則會拋出 `lau::InvalidIterator` 异常。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

### <span id="Clear">`Clear`</span>
```c++
RBTree& Clear() noexcept;
```
- 刪除紅黑樹中所有元素。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(n)$。（$n$ 是樹中元素個數）

### <span id="Swap">`Swap`</span>
```c++
RBTree& Swap(RBTree& other) noexcept;
```
- 交換紅黑樹中元素。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(1)$。

### <span id="Size">`Size`</span>
```c++
[[nodiscard]] SizeT Size() const noexcept;
```
- 取得紅黑樹中元素個數。
- 時間複雜度： $O(1)$。

### <span id="Empty">`Empty`</span>
```c++
[[nodiscard]] bool Empty() const noexcept;
```
- 檢查紅黑樹是否為空。
- 時間複雜度： $O(1)$。

### <span id="MaxSize">`MaxSize`</span>
```c++
[[nodiscard]] long MaxSize() const noexcept;
``` 
- 取得紅黑樹可容納的最大元素個數。

### <span id="GetAllocator">`GetAllocator`</span>
```c++
[[nodiscard]] AllocatorType GetAllocator() const noexcept;
```
- 取得紅黑樹的空間分配器副本。

### <span id="GetCompare">`GetCompare`</span>
```c++
[[nodiscard]] Compare GetCompare() const;
```
- 取得紅黑樹的比較類副本。

### <span id="Contains">`Contains`</span>
```c++
[[nodiscard]] bool Contains(const T& value) const;
```
- 檢查紅黑樹中是否有與 `value` 相等的元素。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

```c++
template<class K>
[[nodiscard]] bool Contains(const K& value) const;
```
- 檢查紅黑樹中是否有與 `value` 相等的元素。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

### <span id="Find">`Find`</span>
```c++
[[nodiscard]] Iterator Find(const T& value);
```
- 取得紅黑樹中與 `value` 相等的元素的迭代器。
- 如果元素不是紅黑樹中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

```c++
[[nodiscard]] ConstIterator Find(const T& value) const;
```
- 取得紅黑樹中與 `value` 相等的元素的常量迭代器。
- 如果元素不是紅黑樹中的元素，則返回末常量迭代器；否則返回指向對應元素的常量迭代器。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

```c++
template<class K>
[[nodiscard]] Iterator Find(const K& value);
```
- 取得紅黑樹中與 `value` 相等的元素的迭代器。
- 如果元素不是紅黑樹中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

```c++
template<class K>
[[nodiscard]] ConstIterator Find(const K& value) const;
```
- 取得紅黑樹中與 `value` 相等的元素的常量迭代器。
- 如果元素不是紅黑樹中的元素，則返回末常量迭代器；否則返回指向對應元素的常量迭代器。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

### <span id="LowerBound">`LowerBound`</span>
```c++
[[nodiscard]] Iterator LowerBound(const T& value);
```
- 取得紅黑樹中不小於（大於等於） `value` 的最小元素的迭代器。
- 如果元素不是紅黑樹中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

```c++
[[nodiscard]] ConstIterator LowerBound(const T& value) const;
```
- 取得紅黑樹中不小於（大於等於） `value` 的最小元素的常量迭代器。
- 如果元素不是紅黑樹中的元素，則返回末常量迭代器；否則返回指向對應元素的常量迭代器。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

```c++
template<class K>
[[nodiscard]] Iterator LowerBound(const K& value);
```
- 取得紅黑樹中不小於（大於等於） `value` 的最小元素的迭代器。
- 如果元素不是紅黑樹中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

```c++
template<class K>
[[nodiscard]] ConstIterator LowerBound(const K& value) const;
```
- 取得紅黑樹中不小於（大於等於） `value` 的最小元素的常量迭代器。
- 如果元素不是紅黑樹中的元素，則返回末常量迭代器；否則返回指向對應元素的常量迭代器。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

### <span id="UpperBound">`UpperBound`</span>
```c++
[[nodiscard]] Iterator UpperBound(const T& value);
```
- 取得紅黑樹中大於 `value` 的最小元素的迭代器。
- 如果元素不是紅黑樹中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

```c++
[[nodiscard]] ConstIterator UpperBound(const T& value) const;
```
- 取得紅黑樹中大於 `value` 的最小元素的常量迭代器。
- 如果元素不是紅黑樹中的元素，則返回末常量迭代器；否則返回指向對應元素的常量迭代器。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

```c++
template<class K>
[[nodiscard]] Iterator UpperBound(const K& value);
```
- 取得紅黑樹中大於 `value` 的最小元素的迭代器。
- 如果元素不是紅黑樹中的元素，則返回末迭代器；否則返回指向對應元素的迭代器。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

```c++
template<class K>
[[nodiscard]] ConstIterator UpperBound(const K& value) const;
```
- 取得紅黑樹中大於 `value` 的最小元素的常量迭代器。
- 如果元素不是紅黑樹中的元素，則返回末常量迭代器；否則返回指向對應元素的常量迭代器。
- 注意：`K` 必須符合 `Compare::is_transparent` 類別。
- 時間複雜度： $O(\log n)$。（$n$ 是樹中元素個數）

### <span id="Begin">`Begin`</span>
```c++
[[nodiscard]] Iterator Begin() noexcept;
```
- 取得紅黑樹中第一個元素的迭代器。
- 時間複雜度： $O(1)$。

```c++
[[nodiscard]] ConstIterator Begin() const noexcept;
```
- 取得紅黑樹中第一個元素的常量迭代器。
- 時間複雜度： $O(1)$。

### <span id="begin">`begin`</span>
```c++
[[nodiscard]] Iterator begin() noexcept;
```
- 取得紅黑樹中第一個元素（最小元素）的迭代器。
- 時間複雜度： $O(1)$。

```c++
[[nodiscard]] ConstIterator begin() const;
```
- 取得紅黑樹中第一個元素（最小元素）的常量迭代器。
- 時間複雜度： $O(1)$。

### <span id="ConstBegin">`ConstBegin`</span>
```c++
[[nodiscard]] ConstIterator ConstBegin() const noexcept;
```
- 取得紅黑樹中第一個元素（最小元素）的常量迭代器。
- 時間複雜度： $O(1)$。

### <span id="End">`End`</span>
```c++
[[nodiscard]] Iterator End() noexcept;
```
- 取得紅黑樹的末迭代器。
- 注意：末迭代器並不指向紅黑樹中的元素，而是指向紅黑樹的最後一個元素後的位置。
- 時間複雜度： $O(1)$。

```c++
[[nodiscard]] ConstIterator End() const noexcept;
```
- 取得紅黑樹的末常量迭代器。
- 注意：末常量迭代器並不指向紅黑樹中的元素，而是指向紅黑樹的最後一個元素後的位置。
- 時間複雜度： $O(1)$。

### <span id="end">`end`</span>
```c++
[[nodiscard]] Iterator end() noexcept;
```
- 取得紅黑樹的末迭代器。
- 注意：末迭代器並不指向紅黑樹中的元素，而是指向紅黑樹的最後一個元素後的位置。
- 時間複雜度： $O(1)$。

```c++
[[nodiscard]] ConstIterator end() const noexcept;
```
- 取得紅黑樹的末常量迭代器。
- 注意：末常量迭代器並不指向紅黑樹中的元素，而是指向紅黑樹的最後一個元素後的位置。
- 時間複雜度： $O(1)$。

### <span id="ConstEnd">`ConstEnd`</span>
```c++
[[nodiscard]] ConstIterator ConstEnd() const;
```
- 取得紅黑樹的末常量迭代器。
- 注意：末常量迭代器並不指向紅黑樹中的元素，而是指向紅黑樹的最後一個元素後的位置。
- 時間複雜度： $O(1)$。

### <span id="SwapNonmember">`Swap`</span>
```c++
template <class T, class Compare, class Allocator>
void Swap(RBTree<T, Compare, Allocator>& lhs,
          RBTree<T, Compare, Allocator>& rhs) noexcept;
```
- 交換兩個紅黑樹。
- 時間複雜度： $O(1)$。

### <span id="iterator">迭代器</span>
成員類型：
- `difference_type`: `std::ptrdiff_t`
- `value_type`: `T`
- `pointer`: `T*`
- `reference`: `T&`
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
