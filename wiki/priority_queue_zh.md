# lau::PriorityQueue

切換到其他語言： [English](priority_queue_en.md)

包含於標頭檔 `lau/priority_queue.h` 中。

```c++
namespace lau {
template<class T,
         class Compare = std::less<T>,
         class Allocator = std::allocator<T>>
class PriorityQueue;
} // namespace lau
```

優先佇列 `lau::PriorityQueue` 保證優先級高者後於優先級低者出棧。因此棧頂元素是優先級最低的元素，將會首個出棧。

此類可以使用客製化比較類來確定優先級（默認爲 `std::less<T>`）注意
`Compare` 必須重載具有兩個參量的運算符 `operator()`。

此類使用左偏樹作爲數據結構。如需瞭解更多相關的信息，請檢視數據結構詳情頁[數據結構詳情頁](#DataStructure)。

請注意：鑒於此類絕大多數操作使用遞迴函數，請爲此類的函數保留足夠多的空間以避免棧空間溢出。

## 概覽
```c++
namespace lau {
template<class T,
         class Compare = std::less<T>,
         class Allocator = std::allocator<T>>
class PriorityQueue {
public:
    // 構造函數
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

    // 賦值運算符
    PriorityQueue& operator=(const PriorityQueue& obj);
    PriorityQueue& operator=(PriorityQueue&& obj) noexcept;

    // 析構函數
    ~PriorityQueue();

    // 元素訪問
    const T& Top() const;

    // 容量
    [[nodiscard]] SizeT Size() const;
    [[nodiscard]] bool Empty() const;
    [[nodiscard]] Compare GetCompare() const;

    // 修改
    PriorityQueue& Clear();
    PriorityQueue& Push(const T& value);
    template<class... Args>
    PriorityQueue& Emplace(Args&&... args);
    PriorityQueue& Pop();
    PriorityQueue& Merge(PriorityQueue& other);
    PriorityQueue& Swap(PriorityQueue& other);
};

// 非成員函數
template<class T, class Compare, class Allocator>
void Swap(PriorityQueue<T, Compare, Allocator> priorityQueue1,
          PriorityQueue<T, Compare, Allocator> priorityQueue2);
} // namespace lau
```
## 模板
- `T`：元素之類型
- `Compare`：比較類之類型（默認爲 `std::less<T>`）
  - 需要符合 C++ 對於比較類之要求（參見 [C++ 具名要求：比较 (Compare)](https://zh.cppreference.com/w/cpp/named_req/Compare)）
    - 必須具有合法的 `bool Compare(const T& lhs, const T& rhs)`。
    - 保證唯一性（若 `Compare(a, b) = false` 且 `Compare(a, b) = false`，或
      `Compare(a, b) = true` 且 `Compare(a, b) = true`，則 `a` 與 `b` 相同）
- `Allocator`：記憶體分配器之類型（默認爲 `std::allocator`）
  - 需要符合 C++ 對於記憶體分配器之要求（參見 [C++ 具名要求：分配器 (Allocator)](https://zh.cppreference.com/w/cpp/named_req/Allocator)）
  - 默認爲 `std::allocator<T>`。

## 成員函數
- [（構造函數）](#Constructors)
- [`operator=`](#operator=)
- （析構函數）

### 元素訪問
- [`Top`](#Top)：獲取棧頂元素

### 容量
- [`Size`](#Size)：獲取元素個數
- [`Empty`](#Empty)：檢查容器是否爲空
- [`GetCompare`](#GetCompare)：獲取比較函數副本

### 修改
- [`Clear`](#Clear)：清空所有數據
- [`Push`](#Push)：加入新元素
- [`Emplace`](#Emplace)：原位構造新元素
- [`Pop`](#Pop)：移除棧頂元素
- [`Merge`](#Merge)：與另一優先佇列合併
- [`Swap`](#Swap)：交換内容

## 非成員函數
- [`Swap`](#SwapNonmember)：交換内容

## 詳情
### <span id="DataStructure">數據結構</span>
此優先佇列使用左偏樹作爲數據結構。相較於完全二叉樹，左偏樹并不平衡。除此以外，左偏樹的右邊部分比左邊部分有著更少的空路徑長度。

每次修改都仰賴合併操作。加入元素等同於將一個僅含有一個節點的樹與原樹合併。移除棧頂元素等同於合併左右兩棵子樹。合併兩個佇列等同於將兩棵樹合併起來。

時間複雜度：
- 入列： $O(\log n)$
- 出列： $O(\log n)$
- 合併： $O(\log n)$

### <span id="Constructors">構造函數</span>
```c++
PriorityQueue();
```
- 默認構造函數
- 以默認比較類以及默認記憶體分配器構造空佇列。

```c++
explicit PriorityQueue(const Compare& compare);
```
- 以指定比較類以及默認記憶體分配器構造空佇列。

```c++
explicit PriorityQueue(const Allocator& allocator);
```
- 以默認比較類以及指定記憶體分配器構造空佇列。

```c++
PriorityQueue(const Compare& compare, const Allocator& allocator);
```
- 以指定比較類以及指定記憶體分配器構造空佇列。

```c++
PriorityQueue(const PriorityQueue& obj);
```
- 複製構造函數
- 從另一優先佇列複製内容。
- 採用遞迴，請確保足夠棧空間。
- 時間複雜度： $O(n)$。

```c++
PriorityQueue(PriorityQueue&& obj) noexcept;
```
- 移動構造函數
- 從右值轉移數據到此優先佇列。
- 時間複雜度： $O(1)$。

```c++
template<class Iterator>
PriorityQueue(Iterator first, Iterator last,
              const Compare& compare = Compare());
```
- 以迭代器中元素作爲初始數據，並以默認（如果缺省）或指定比較類，默認記憶體分配器構造此優先佇列。
- 採用遞迴，請確保足夠棧空間。
- 時間複雜度： $O(n\cdot \log n)$。

```c++
template<class Iterator>
PriorityQueue(Iterator first, Iterator last,
              const Allocator& allocator);
```
- 以迭代器中元素作爲初始數據，並以默認比較類，指定記憶體分配器構造此優先佇列。
- 採用遞迴，請確保足夠棧空間。
- 時間複雜度： $O(n\cdot \log n)$。

```c++
template<class Iterator>
PriorityQueue(Iterator first, Iterator last,
              const Compare& compare,
              const Allocator& allocator);
```
- 以迭代器中元素作爲初始數據，並以指定比較類，指定記憶體分配器構造此優先佇列。
- 採用遞迴，請確保足夠棧空間。
- 時間複雜度： $O(n\cdot \log n)$。

### <span id="operator=">`operator=`</span>
```c++
PriorityQueue& operator=(const PriorityQueue& obj);
```
- 以 `obj` 向此優先佇列賦值。
- 採用遞迴，請確保足夠棧空間。

```c++
PriorityQueue& operator=(PriorityQueue&& obj) noexcept;
```
- 以右值 `obj` 向此優先佇列賦值。

### <span id="Top">`Top`</span>
```c++
const T& Top() const;
```
- 獲取佇列的頂部元素。（頂部元素是優先級最低者）
- 時間複雜度： $O(1)$。

### <span id="Size">`Size`</span>
```c++
[[nodiscard]] SizeT Size() const;
```
- 獲取佇列元素個數。
- 時間複雜度： $O(1)$。

### <span id="Empty">`Empty`</span>
```c++
[[nodiscard]] bool Empty() const;
```
- 檢查佇列是否爲空。
- 時間複雜度： $O(1)$。

### <span id="GetCompare">`GetCompare`</span>
```c++
[[nodiscard]] Compare GetCompare() const;
```
- 獲取比較類的副本。

### <span id="Clear">`Clear`</span>
```c++
PriorityQueue& Clear();
```
- 清空佇列。
- 採用遞迴，請確保足夠棧空間。
- 時間複雜度： $O(n)$。

### <span id="Push">`Push`</span>
```c++
PriorityQueue& Push(const T& value);
```
- 將新元素加入優先佇列。
- 採用遞迴，請確保足夠棧空間。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(\log n)$。

### <span id="Pop">`Pop`</span>
```c++
PriorityQueue& Pop();
```
- 移除頂部元素。
- 採用遞迴，請確保足夠棧空間。
- 如果佇列爲空，將會抛出 `lau::EmptyContainer`。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(\log n)$。

### <span id="Merge">`Merge`</span>
```c++
PriorityQueue& Merge(PriorityQueue& other);
```
- 將另一優先佇列合併到此處。
- 另一優先佇列之元素將會被清除。
- 採用遞迴，請確保足夠棧空間。
- 請注意：兩個類的比較類**必須**相同。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(\log n)$。

### <span id="Swap">`Swap`</span>
```c++
PriorityQueue& Swap(PriorityQueue& other);
```
- 與另一優先佇列交換所有内容。
- 請注意：兩個類必須有相同的模板參數。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(1)$。

### <span id="Emplace">`Emplace`</span>
```c++
template<class... Args>
PriorityQueue& Emplace(Args&&... args);
```
- 將新元素加入優先佇列。
- 在原位構造元素。
- 構造函數與此函數提供的參數一致。
- 採用遞迴，請確保足夠棧空間。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(\log n)$。

### <span id="SwapNonmember">`Swap`</span>
```c++
template<class T, class Compare, class Allocator>
void Swap(PriorityQueue<T, Compare, Allocator> priorityQueue1,
          PriorityQueue<T, Compare, Allocator> priorityQueue2);
```
- 交換兩個優先佇列的所有内容。
- 請注意：兩個類必須有相同的模板參數。
- 時間複雜度： $O(1)$。
