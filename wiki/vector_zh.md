# lau::Vector

切換到其他語言： [English](vector_en.md)

包含於標頭檔 `lau/vector.h` 中。

```c++
namespace lau {
template<class T,
         class Allocator = std::allocator<T>>
class Vector;
} // namespace lau
```

`lau::Vector` 是一個綫性容器， 一個可變長度的數組。與 `std::vector`
不同的是，此類是將指向對象的指針併列排佈。因此寫入數據以及改變容量變得更加容易。

如欲瞭解更多訊息，請檢視[數據結構詳情](#DataStructure)。

## 概覽
```c++
namespace lau {
template<class T,
         class Allocator = std::allocator<T>>
class Vector {
public:
    // 類型
    using AllocatorType = Allocator;
    using PointerAllocatorType = typename std::allocator_traits<Allocator>::template rebind_alloc<T*>;
    class Iterator;
	class ConstIterator;

    // 構造函數
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

    // 賦值運算符
    Vector& operator=(const Vector& obj);
    Vector& operator=(Vector&& obj) noexcept;

    // 析構函數
    ~Vector();

    // 獲取器
    [[nodiscard]] AllocatorType GetAllocator() const noexcept;

    // 元素訪問
    T& At(SizeT index);
    const T& At(SizeT index) const;
    T& operator[](SizeT index);
    const T& operator[](SizeT index) const;
    T& Front();
    const T& Front() const;
    T& Back();
    const T& Back() const;

    // 迭代器
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

    // 容量
    [[nodiscard]] SizeT Capacity() const noexcept;
    [[nodiscard]] bool Empty() const noexcept;
    [[nodiscard]] long MaxSize() const noexcept;
    [[nodiscard]] SizeT Size() const noexcept;
    Vector& ShrinkToFit();
    Vector& Reserve(SizeT newCapacity);

    // 修改
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

// 非成員函數
template<class T, class Allocator>
void Swap(Vector<T, Allocator>& vector1, Vector<T, Allocator>& vector2) noexcept;
} // namespace lau
```
## 模板
- `T`：類內嵌的類型
- `Allocator`：記憶體分配器（默認爲 `std::allocator`）。

## 成員類型
- `AllocatorType`：記憶體分配器類型，與 `Allocator` 一致
- `PointerAllocatorType`：`T*` 的記憶體分配器類型
- [`Iterator`](#iterator)：迭代器 `lau::vector` 之類型
- [`ConstIterator`](#iterator)：常量迭代器 `lau::Vector` 之類型

## 成員函數
- [（構造函數）](#Constructors)
- [`operator=`](#operator=)
- （析構函數）

### 獲取器
- [`GetAllocator`](#GetAllocator)：獲得記憶體分配器之副本

### 元素訪問
- [`At`](#At)：以下標訪問
- [`operator[]`](#operator[])：以下標訪問
- [`Front`](#Front)：訪問首個元素
- [`Back`](#Back)：訪問末尾元素

### 迭代器
- [`Begin`](#Begin)：獲取首迭代器
- [`begin`](#begin)：獲取首迭代器
- [`ConstBegin`](#ConstBegin)：獲取首常量迭代器
- [`cbegin`](#cbegin)：獲取首常量迭代器
- [`End`](#End)：獲取末迭代器
- [`end`](#end)：獲取末迭代器
- [`ConstEnd`](#ConstEnd)：獲取末常量迭代器
- [`cend`](#cend)：獲取末常量迭代器

### 容量
- [`Capacity`](#Capacity)：獲取容量
- [`Empty`](#Empty)：檢查容器是否爲空
- [`MaxSize`](#MaxSize)：獲取最大可存放元素個數
- [`Size`](#Size)：獲取所存之元素數量
- [`ShrinkToFit`](#ShrinkToFit)：減少容量
- [`Reserve`](#Reserve)：保留容量

### 修改
- [`Clear`](#Clear)：清空容器
- [`Insert`](#Insert)：插入元素
- [`Erase`](#Erase)：移除元素
- [`PushBack`](#PushBack)：從尾部加入元素
- [`EmplaceBack`](#EmplaceBack)：從尾部原位構造元素
- [`PushFront`](#PushFront)：從頭部加入元素
- [`EmplaceFront`](#EmplaceFront)：從頭部原位構造元素
- [`PopBack`](#PopBack)：從尾部移除元素
- [`PopFront`](#PopFront)：從頭部移除元素
- [`Swap`](#Swap)：交換內容
- [`SwapElement`](#SwapElement)：交换元素
- [`Resize`](#Resize)：更改容器之元素個數
- [`Sort`](#Sort)：將容器排序

## 非成員函數
- [`Swap`](#SwapNonmember)：交換內容

## 詳情
### <span id="DataStructure">數據結構</span>
`Vector` 即是一個動態大小之數組。

時間複雜度：
- 訪問： $O(1)$
- 插入： $O(n)$
- 移除： $O(n)$

### <span id="Constructors">構造函數</span>
```c++
Vector() noexcept(noexcept(Allocator()));
```
- 默認構造函數
- 使用默認記憶體分配器構造空容器。

```c++
explicit Vector(const Allocator& allocator);
```
- 使用指定記憶體分配器構造空容器。

```c++
Vector(SizeT count,
       const T& value,
       const Allocator& allocator = Allocator());
```
- 使用指定記憶體分配器或默認記憶體分配器（如缺省）構造一個含有 `count` 個 `value` 的容器。

```c++
explicit Vector(SizeT count,
                const Allocator& allocator = Allocator());
```
- 使用指定記憶體分配器或默認記憶體分配器（如缺省）構造一個含有 `count` 個默認元素的容器。

```c++
template<class InputIterator>
Vector(const InputIterator& begin,
       const InputIterator& end,
       const Allocator& allocator = Allocator());
```
- 使用指定記憶體分配器或默認記憶體分配器（如缺省）構造一個含有位於 [begin, end) 內元素的容器。

```c++
Vector(const Vector& obj);
```
- 複製構造函數
- 構造一個與 `obj` 含有相同元素的容器。

```c++
Vector(Vector&& obj) noexcept;
```
- 移動構造函數
- 從右值構造一個與 `obj` 含有相同元素的容器。

```c++
Vector(std::initializer_list<T> init,
       const Allocator& allocator = Allocator());
```
- 使用指定記憶體分配器或默認記憶體分配器（如缺省）從 `std::initializer_list<T>` 構造一個容器。

### <span id="operator=">`operator=`</span>
```c++
Vector& operator=(const Vector& obj);
```
- 以 `obj` 賦值到此容器。

```c++
Vector& operator=(Vector&& obj) noexcept;
```
- 以右值 `obj` 賦值到此容器。

### <span id="GetAllocator">`GetAllocator`</span>
```c++
[[nodiscard]] AllocatorType GetAllocator() const noexcept;
```
- 獲取此類中記憶體分配器的副本。

### <span id="At">`At`</span>
```c++
T& At(SizeT index);
```
- 以下標訪問元素。
- 如果下標并不在範圍 [0, size) 中，將抛出 `lau::OutOfRange` 異常類。
- 你可以讀寫此元素。
- 時間複雜度： $O(1)$。

```c++
const T& At(SizeT index) const;
```
- 以下標訪問元素。
- 如果下標并不在範圍 [0, size) 中，將抛出 `lau::OutOfRange` 異常類。
- 你僅可以讀取此元素。
- 時間複雜度： $O(1)$。

### <span id="operator[]">`operator[]`</span>
```c++
T& operator[](SizeT index);
```
- 以下標訪問元素。
- 如果下標并不在範圍 [0, size) 中，將抛出 `lau::OutOfRange` 異常類。
- 你可以讀寫此元素。
- 時間複雜度： $O(1)$。

```c++
const T& operator[](SizeT index) const;
```
- 以下標訪問元素。
- 如果下標并不在範圍 [0, size) 中，將抛出 `lau::OutOfRange` 異常類。
- 你僅可以讀取此元素。
- 時間複雜度： $O(1)$。

### <span id="Front">`Front`</span>
```c++
T& Front();
```
- 訪問首個元素。
- 如果此爲空容器，將抛出 `lau::EmptyContainer` 異常類。
- 你可以讀寫此元素。
- 時間複雜度： $O(1)$。

```c++
const T& Front() const;
```
- 訪問首個元素。
- 如果此爲空容器，將抛出 `lau::EmptyContainer` 異常類。
- 你僅可以讀取此元素。
- 時間複雜度： $O(1)$。

### <span id="Back">`Back`</span>
```c++
T& Back();
```
- 訪問最後元素。
- 如果此爲空容器，將抛出 `lau::EmptyContainer` 異常類。
- 你可以讀寫此元素。
- 時間複雜度： $O(1)$。

```c++
const T& Back() const;
```
- 訪問最後元素。
- 如果此爲空容器，將抛出 `lau::EmptyContainer` 異常類。
- 你僅可以讀取此元素。
- 時間複雜度： $O(1)$。

### <span id="Begin">`Begin`</span>
```c++
[[nodiscard]] Iterator Begin() noexcept;
```
- 獲取首迭代器。

```c++
[[nodiscard]] ConstIterator Begin() const noexcept;
```
- 獲取首常量迭代器。

### <span id="begin">`begin`</span>
```c++
[[nodiscard]] Iterator begin() noexcept;
```
- 獲取首迭代器。

```c++
[[nodiscard]] ConstIterator Begin() const noexcept;
```
- 獲取首常量迭代器。

### <span id="ConstBegin">`ConstBegin`</span>
```c++
[[nodiscard]] ConstIterator ConstBegin() const noexcept;
```
- 獲取首常量迭代器。

### <span id="End">`End`</span>
```c++
[[nodiscard]] Iterator End() noexcept;
```
- 獲取末迭代器。
- 請注意：此迭代器是最後一個元素對應迭代器的後一個。

```c++
[[nodiscard]] ConstIterator End() const noexcept;
```
- 獲取末常量迭代器。
- 請注意：此迭代器是最後一個元素對應迭代器的後一個。

### <span id="end">`end`</span>
```c++
[[nodiscard]] Iterator end() noexcept;
```
- 獲取末迭代器。
- 請注意：此迭代器是最後一個元素對應迭代器的後一個。

```c++
[[nodiscard]] ConstIterator end() const noexcept;
```
- 獲取末常量迭代器。
- 請注意：此迭代器是最後一個元素對應迭代器的後一個。

### <span id="ConstEnd">`ConstEnd`</span>
```c++
[[nodiscard]] ConstIterator ConstEnd() const noexcept;
```
- 獲取末常量迭代器。
- 請注意：此迭代器是最後一個元素對應迭代器的後一個。

### <span id="Capacity">`Capacity`</span>
```c++
[[nodiscard]] SizeT Capacity() noexcept;
```
- 獲取容器的容量。
- 請注意：由於此容器可以從前部移除元素，因此可用空間可能比此結果小。

### <span id="Empty">`Empty`</span>
```c++
[[nodiscard]] bool Empty() const noexcept;
```
- 檢查容器是否爲空。

### <span id="MaxSize">`MaxSize`</span>
```c++
[[nodiscard]] long MaxSize() const noexcept;
```
- 獲取此容器的最大容量。

### <span id="Size">`Size`</span>
```c++
[[nodiscard]] SizeT Size() const noexcept;
```
- 獲取容器中的元素個數。

### <span id="ShrinkToFit">`ShrinkToFit`</span>
```c++
Vector& ShrinkToFit();
```
- 將容量減少到最小（即元素個數）。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度：不確定。多數情況下爲 $O(n)$。

### <span id="Reserve">`Reserve`</span>
```c++
Vector& Reserve(SizeT newCapacity);
```
- 保留至少 `newCapacity` 的空間。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 請注意：保留空間操作并**不會**構造新的元素，因此不可訪問新的空間。如需訪問新增的空間，請使用成員函數
  [`Resize`](#Resize)。
- 時間複雜度：不確定。多數情況下爲 $O(n)$。

### <span id="Clear">`Clear`</span>
```c++
Vector& Clear() noexcept;
```
- 清空容器內所有的元素。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 請注意：此操作也會一並清除所有此前分配給該容器的空間。此操作后，此容器容量爲 0。
- 時間複雜度： $O(n)$。

### <span id="Insert">`Insert`</span>
```c++
Iterator Insert(const Iterator& position, const T& value);
```
- 在對應位置前插入元素。
- 返回指向被插入元素的迭代器。
- 時間複雜度：不確定。多數情況下爲 $O(n)$。

```c++
Iterator Insert(const ConstIterator& position, const T& value);
```
- 在對應位置前插入元素。
- 返回指向被插入元素的迭代器。
- 時間複雜度：不確定。多數情況下爲 $O(n)$。

```c++
Iterator Insert(SizeT index, const T& value);
```
- 在下標處插入元素。
- 如若 `index > size`，將抛出 `lau::OutOfRange` 異常類。
- 此操作后，`this->at(ind)` 將爲插入的值。
- 返回指向被插入元素的迭代器。
- 時間複雜度：不確定。多數情況下爲 $O(n)$。

### <span id="Erase">`Erase`</span>
```c++
Iterator Erase(const Iterator& position);
```
- 在對應位置移除元素。
- 如若此迭代器爲末迭代器，將抛出 `lau::OutOfRange` 異常類。
- 返回指向下一元素的迭代器。
- 如若此迭代器指向最後一個元素，則返回 `End()` 迭代器。
- 時間複雜度：不確定。多數情況下爲 $O(n)$。

```c++
Iterator Erase(const Iterator& position);
```
- 在對應位置移除元素。
- 如若此迭代器爲末迭代器，將抛出 `lau::OutOfRange` 異常類。
- 返回指向下一元素的迭代器。
- 如若此迭代器指向最後一個元素，則返回 `End()` 迭代器。
- 時間複雜度：不確定。多數情況下爲 $O(n)$。

```c++
Iterator Erase(SizeT index);
```
- 在下標處刪除元素。
- 如若 `index >= size`，將抛出 `lau::OutOfRange` 異常類。
- 返回指向下一元素的迭代器。
- 如若此迭代器指向最後一個元素，則返回 `End()` 迭代器。
- 時間複雜度：不確定。多數情況下爲 $O(n)$。

### <span id="PushBack">`PushBack`</span>
```c++
Vector& PushBack(const T& value);
```
- 將 `value` 添加至容器末尾。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度：不確定。多數情況下爲 $O(1)$。僅當容器需要擴容時爲 $O(n)$。

### <span id="PushFront">`PushFront`</span>
```c++
Vector& PushFront(const T& value);
```
- 將 `value` 添加至容器頭部。Add an element (`value`) to the front.
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度：不確定。多數情況下爲 $O(n)$。僅當容器前部有空間時爲 $O(1)$。

### <span id="PopBack">`PopBack`</span>
```c++
Vector& PopBack() noexcept;
```
- 移除最後一個元素。
- 如若此爲空容器，將抛出 `lau::EmptyContainer` 異常類。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度：$O(1)$。

### <span id="PopBack">`PopBack`</span>
```c++
Vector& PopFront();
```
- 移除首個元素。
- 如若此爲空容器，將抛出 `lau::EmptyContainer` 異常類。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度：$O(1)$。

### <span id="Swap">`Swap`</span>
```c++
Vector& Swap(Vector& other) noexcept;
```
- 交換容器内容。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。

### <span id="Resize">`Resize`</span>
```c++
Vector& Resize(SizeT count);
```
- 將容器的元素個數調整至 `count`。
- 如若當前元素個數多於 `count`，容器將減少到前 `count` 個元素。
- 如若當前元素個數少於 `count`，容器將以默認元素添加至尾部。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。

```c++
Vector& Resize(SizeT count, const T& value);
```
- 將容器的元素個數調整至 `count`。
- 如若當前元素個數多於 `count`，容器將減少到前 `count` 個元素。
- 如若當前元素個數少於 `count`，容器將以 `value` 添加至尾部。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。

### <span id="SwapElement">`SwapElement`</span>
```c++
Vector& SwapElement(SizeT index1, SizeT index2);
```
- 交換兩個下標對應的元素。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。

```c++
Vector& SwapElement(const Iterator& iterator1, const Iterator& iterator2);
```
- 交換兩個迭代器對應的元素。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。

### <span id="EmplaceBack">`EmplaceBack`</span>
```c++
template<class... Args>
Vector& EmplaceBack(Args&&... args);
```
- 將元素從尾部加入。
- 將會以參數原位構造元素。
- 構造函數與此函數提供的參數一致。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度：不確定。多數情況下爲 $O(1)$。僅當容器需要擴容時爲 $O(n)$。

### <span id="EmplaceFront">`EmplaceFront`</span>
```c++
template<class... Args>
Vector& EmplaceFront(Args&&... args);
```
- 將元素從頭部加入。
- 將會以參數原位構造元素。
- 構造函數與此函數提供的參數一致。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度：不確定。多數情況下爲 $O(n)$。僅當容器前部有空間時爲 $O(1)$。

### <span id="Sort">`Sort`</span>
```c++
template<class Compare = std::less<T>>
Vector& Sort(const Compare& compare = Compare());
```
- 將此容器以特定比較規則排序。
- 此函數使用[歸併排序 (merge sort)](https://zh.wikipedia.org/wiki/%E5%BD%92%E5%B9%B6%E6%8E%92%E5%BA%8F)
  算法來將此容器排序。
- `Compare` 類必須有 `bool operator()(const T&, const T&) const` 函數。
- 此函數以 `operator<` (`std::less`) 爲缺省值。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 時間複雜度： $O(n \log n)$。

### <span id="SwapNonmember">`Swap`</span>
```c++
template<class T, class Allocator>
void Swap(Vector<T, Allocator>& vector1, Vector<T, Allocator>& vector2) noexcept;
```
- 交換兩個容器的內容。

### <span id="iterator">iterator</span>
類型：
- `difference_type`: `std::ptrdiff_t`
- `value_type`: `T`
- `pointer`: `T*`
- `reference`: `T&`
- `iterator_category`: `std::output_iterator_tag`

支援的操作：（以下的迭代器可以是 `Iterator` 或 `ConstIterator`）
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

非法化：
- 插入：插入位置后的所有迭代器
- 移除：移除位置后的所有迭代器
- 擴容：所有迭代器
- 從尾部插入元素：僅末迭代器
- 從尾部移除元素：僅末迭代器
- 從頭部插入元素：當頭部無空間時，除首迭代器外的所有迭代器；當頭部有空間時，僅首迭代器
- 從頭部移除：僅首迭代器。
