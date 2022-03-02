# lau::SparseTable

切換到其他語言： [English](sparse_table_en.md)

包含於標頭檔 `lau/algorithm.h`中。

```c++
namespace lau {
template<class T>
class SparseTable;
} // namespace lau
```

`lau::SparseTable` 是一種透過預處理數據減少多次詢問花費的時間的表。

欲瞭解更多關於此數據結構的資訊，請檢視[數據結構詳情](#DataStructure).

注意此預處理所使用的函數必須有一下性質：（一下的函數是數學概念，類中只需要一個雙參量函數）
1. Function(a) = a;
2. Function(a, b) = Function(b, a);
3. Function(a, b, ...) = Function(Function(a, b), ...).

簡單例子如：最大公約數、最大值、最小值。

## 概覽
```c++
namespace lau {
template<class T>
class SparseTable;
public:
    // 構造函數
    SparseTable() noexcept;
    template<class Iterator>
    SparseTable(Iterator begin, Iterator end, std::function<T(T, T)> Function);
    template<class Iterator>
    SparseTable(Iterator begin, SizeT size, std::function<T(T, T)> Function);
    SparseTable(const SparseTable& obj);
    SparseTable(SparseTable&& obj) noexcept;

    // 賦值運算符
    SparseTable& operator=(const SparseTable& obj);
    SparseTable& operator=(SparseTable&& obj) noexcept;

    // 析構函數
    ~SparseTable();

    // 獲取器
    [[nodiscard]] std::function<T(T, T)> GetFunction() const;
    [[nodiscard]] SizeT Size() const noexcept;

    // 容量
    [[nodiscard]] bool Empty() const noexcept;

    // 修改器
    SparseTable& Clear();
    template<class Iterator>
    SparseTable& Calculate(Iterator begin,
                           Iterator end,
                           std::function<T(T, T)> Function);
    template<class Iterator>
    SparseTable& Calculate(Iterator begin,
                           SizeT size,
                           std::function<T(T, T)> Function);
    
    //操作
    T Query(SizeT beginIndex, SizeT endIndex);
};
} // namespace lau
```
## 模板
- `T`：函數的輸入輸出類型

## 成員函數
- [（構造函數）](#Constructors)
- [`operator=`](#operator=)
- （析構函數）

### 獲取器
- [`GetFunction`](#GetFunction)：獲得函數的副本
- [`Size`](#Size)：獲取輸入數據數量

### 容量
- [`Empty`](#Empty)：檢查是否含有數據

### 修改器
- [`Clear`](#Clear)：清除所有數據
- [`Calculate`](#Calculate)：以新數據計算

### 操作
- [`Query`](#Query)：獲得某一範圍内的結果

## 詳情
### <span id="DataStructure">數據結果</span>
稀疏表是一種多層結構的表。每一層儲存了從當前位置開始 $2^i$
個數據的函數結果。因此僅需找到合適的前綴與後綴（二者合起來覆蓋整個查詢的範圍）便可得到數據。

爲了達成這一要求，函數必須符合以下之性質：
1. Function(a) = a;
2. Function(a, b) = Function(b, a);
3. Function(a, b, ...) = Function(Function(a, b), ...).

簡單例子如：最大公約數、最大值、最小值。

時間複雜度：
- 預處理： $O(n \times \log (n))$
- 查詢： $O(1)$

### <span id="Constructors">構造函數</span>
```c++
SparseTable() noexcept;
```
- 默認構造函數
- 構造空表

```c++
template<class Iterator>
SparseTable(Iterator begin, Iterator end, std::function<T(T, T)> Function);
```
- 在 `[begin, end)` 範圍內計算。
- 如若 `begin` 大於 `end`，將抛出
  [`lau::InvalidArgument`](exception_zh.md) 異常類。
- 注意必須帶有 `end - begin` 函數。

```c++
template<class Iterator>
SparseTable(Iterator begin, SizeT size, std::function<T(T, T)> Function);
```
- 在 `[begin, begin + size)` 範圍内計算。

```c++
SparseTable(const SparseTable& obj);
```
- 複製構造函數
- 以 `obj` 構造此表。
```c++
SparseTable(SparseTable&& obj) noexcept;
```
- 移動構造函數
- 從右值 `obj` 接過此表。

### <span id="operator=">`operator=`</span>
```c++
SparseTable& operator=(const SparseTable& obj);
```
- 以 `obj` 賦值構造此表。
- 此操作將刪除此前的數據。

```c++
- SparseTable& operator=(SparseTable&& obj) noexcept;
```
- 從右值 `obj` 賦值構造此表。
- 此操作將刪除此前的數據。

### <span id="GetFunction">`GetFunction`</span>
```c++
[[nodiscard]] std::function<T(T, T)> GetFunction() const;
```
- 獲得函數副本。
- 請注意：即使此類數據被清除了，函數亦不會被刪除。

### <span id="Size">`Size`</span>
```c++
[[nodiscard]] SizeT Size() const noexcept;
```
- 獲得原始數據的數量。

### <span id="Empty">`Empty`</span>
```c++
[[nodiscard]] bool Empty() const noexcept;
```
- 檢查此類是否含有數據。

### <span id="Clear">`Clear`</span>
```c++
SparseTable& Clear();
```
- 清除此前計算的數據。
- 請注意：由於缺少對應之接口，類内儲存的函數并不會被刪除。

### <span id="Calculate">`Calculate`</span>
```c++
template<class Iterator>
    SparseTable& Calculate(Iterator begin,
                           Iterator end,
                           std::function<T(T, T)> Function);
```
- 重新計算新的數據。
- 如參數存在邏輯問題，將會抛出 [`lau::InvalidArgument`](exception_zh.md) 異常類。
- 無論是否抛出異常，此操作將會清除先前表的數據。

```c++
    template<class Iterator>
    SparseTable& Calculate(Iterator begin,
                           SizeT size,
                           std::function<T(T, T)> Function);
```
- 重新計算新的數據。
- 如參數存在邏輯問題，將會抛出 [`lau::InvalidArgument`](exception_zh.md) 異常類。
- 無論是否抛出異常，此操作將會清除先前表的數據。

### <span id="Query">`Query`</span>
```c++
T Query(SizeT beginIndex, SizeT endIndex);
```
- 查詢此稀疏表。
- 如參數存在邏輯問題，將會抛出 [`lau::InvalidArgument`](exception_zh.md) 異常類。
- 如果此表沒有數據，將會抛出 [`lau::EmptyContainer`](exception_zh.md) 異常類。
