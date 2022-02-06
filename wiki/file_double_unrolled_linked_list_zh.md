# lau::FileDoubleUnrolledLinkedList
包含於標頭檔 `lau/file_double_unrolled_linked_list.h` 和
`lau/file_data_structure.h` 中。

```c++
namespace lau {
template<class KeyType1,
         class KeyType2,
         class ValueType>
class FileDoubleUnrolledLinkedList;
} // namespace lau
```

此模板類作爲基於檔案系統且可以在程式關閉時保留數據的雙鍵映射表。此類允許在可能會被多次關閉的程式中儲存並訪問數據。

此類使用名爲塊狀鏈結串列的數據結構減少訪問和寫入的時間。欲瞭解更多關於塊狀鏈結串列，請點擊[此處](#Data_Structure)。

此類支援垃圾回收，因此不必擔心儲存空間浪費。

<span id="Notice">注意：</span>
1. 兩個鍵類型**必須**含有有效的 `operator<` 和 `operator==` 運算符。
2. 兩個鍵類型和值類型**必須**有默認構造函數和平凡的移動構造和複製構造函數。
3. 此類無法建立檔案，請一定要在使用此類之前自行建立。
4. 用於儲存數據的檔案**必須**爲空或爲此前使用儲存相同鍵值對的此類。
5. 在此類之外對同一檔案寫入數據是**未定義行爲**，請一定避免！

## 概覽
```c++
namespace lau {
template<class KeyType1,
         class KeyType2,
         class ValueType>
class FileDoubleUnrolledLinkedList {
public:
    // 類型
    typedef long Ptr;
    typedef long SizeT;
    struct Node {
        KeyType1  key1;
        KeyType2  key2;
        ValueType value;
    };

    // 構造函數
    explicit FileDoubleUnrolledLinkedList(const char* fileName, SizeT nodeSize = 500) noexcept;
    explicit FileDoubleUnrolledLinkedList(const std::string& fileName, SizeT nodeSize = 500) noexcept;
    FileDoubleUnrolledLinkedList(FileDoubleUnrolledLinkedList&&) noexcept;
    FileDoubleUnrolledLinkedList(const FileDoubleUnrolledLinkedList&) = delete;

    // 賦值運算符
    FileDoubleUnrolledLinkedList& operator=(FileDoubleUnrolledLinkedList&&) noexcept;
    FileDoubleUnrolledLinkedList& operator=(const FileDoubleUnrolledLinkedList&) = delete;

    // 析構函數
    ~FileDoubleUnrolledLinkedList() = default;

    // 修改
    FileDoubleUnrolledLinkedList& Clear();
    bool Erase(const KeyType1& key1, const KeyType2& key2);
    bool Insert(const KeyType1& key1, const KeyType2& key2, const ValueType& value);
    bool Modify(const KeyType1& key1, const KeyType2& key2, const ValueType& value);

    // 操作
    FileDoubleUnrolledLinkedList& Flush();

    // 容量
    [[nodiscard]] bool Empty();

    // 尋找
    [[nodiscard]] bool Exist(const KeyType1& key1, const KeyType2& key2);
    [[nodiscard]] ValueType Get(const KeyType1& key1, const KeyType2& key2);
    [[nodiscard]] ValueType* GetWithPointer(const KeyType1& key1, const KeyType2& key2);

    // 遍歷
    std::vector<Node> Traverse();
    std::vector<Node> Traverse(const KeyType1& key1);
};
} // namespace lau
```

## 成員類型
- `Ptr`： `long` 函數指針之類型
- `SizeT`： `long`, 表徵大小之類型
- `Node`：一個包含鍵值對的結構體
    - `KeyType1 key1`
    - `keyType2 key2`
    - `ValueType value`

## 成員函數
- [（構造函數）](#Constructors)
- [`operator=`](#operator=)
- （析構函數）

### 修改
- [`Clear`](#Clear)：清除全部數據
- [`Erase`](#Erase)：移除對應的鍵值對
- [`Insert`](#Insert)：插入新的鍵值對
- [`Modify`](#Modify)：修改現存鍵值對的值

### 操作
- [`Flush`](#Flush)：與儲存空間同步

### 容量
- [`Empty`](#Empty)：檢查列表是否爲空

### 尋找
- [`Exist`](#Exist)：檢查是否存在含有特定鍵的節點
- [`Get`](#Get)：尋找鍵所對應的值
- [`GetWithPointer`](#GetWithPointer)：獲取鍵對應的值的指針

### 遍歷
- [`Traverse`](#Traverse)：遍歷列表

## 細節
### <span id="Data_Structure">數據結構：塊狀鏈結串列</span>
藉由使用數組之鏈結串列，名爲塊狀鏈結串列的數據結構有著 $O\left(\sqrt{n}\right)$
的讀寫時間複雜度。此類中，鏈結串列爲雙向鏈結串列。

時間複雜度：
- 訪問： $O\left(\sqrt{n}\right)$
- 修改： $O\left(\sqrt{n}\right)$
- 搜尋： $O\left(\sqrt{n}\cdot\log n\right)$

### <span id="Constructors">構造函數</span>
- `explicit lau::FileDoubleUnrolledLinkedList(const char* fileName, lau::FileDoubleUnrolledLinkedListSizeT nodeSize = 500) noexcept;`
    - 構造與以 `fileName` 爲名的檔案相關聯的類。注意：請**確保**檔案存在，否則將導致未定義行爲。
    - 數組大小被定爲 `nodeSize`。
    - 此後，將會檢查檔案是否爲空。如果爲空，一個標頭將會在檔案的最初位置被創建；若不爲空，標頭會被加載。爲保證相容性，數組的大小會依照標頭確定，因此其未必是
      `nodeSize`。
    - 請閲讀此類的[注意事項](#Notice)以避免意料之外的情況。

- `explicit FileDoubleUnrolledLinkedList(const std::string& fileName, SizeT nodeSize = 500) noexcept;`
    - 構造與以 `fileName` 爲名的檔案相關聯的類。注意：請**確保**檔案存在，否則將導致未定義行爲。
    - 數組大小被定爲 `nodeSize`。
    - 此後，將會檢查檔案是否爲空。如果爲空，一個標頭將會在檔案的最初位置被創建；若不爲空，標頭會被加載。爲保證相容性，數組的大小會依照標頭確定，因此其未必是
      `nodeSize`。
    - 請閲讀此類的[注意事項](#Notice)以避免意料之外的情況。

- `FileDoubleUnrolledLinkedList(FileDoubleUnrolledLinkedList&&) noexcept;`
    - 從右值構造此類。

- `FileDoubleUnrolledLinkedList(const FileDoubleUnrolledLinkedList&) = delete;`
  - 由於在兩個對象中讀寫數據係未定義行爲，複製構造函數被禁止使用。

### <span id="operator=">`operator=`</span>
- `FileDoubleUnrolledLinkedList& operator=(FileDoubleUnrolledLinkedList&&) noexcept;`
  - 檢視有關[構造函數](#Constructors)的內容以獲取更多訊息。

- `FileDoubleUnrolledLinkedList& operator=(const FileDoubleUnrolledLinkedList&) = delete;`
  - 檢視有關[構造函數](#Constructors)的內容以獲取更多訊息。

### <span id="Clear">Clear</span>
- `FileDoubleUnrolledLinkedList& Clear();`
  - 清除鏈結串列中的全部內容。

### <span id="Erase">Erase</span>
- `bool Erase(const KeyType1& key1, const KeyType2& key2);`
  - 移除輸入鍵所對應之節點。
  - 如若存在該點，則會移除對應之節點並返回真；
  - 如若不存在該點，則會放棄此次操作並返回假。

### <span id="Insert">Insert</span>
- `bool Insert(const KeyType1& key1, const KeyType2& key2, const ValueType& value);`
  - 插入一個新的鍵值對。
  - 如若不存在與其鍵值相同之節點（即操作成功），則返回值爲真；
  - 如若存在與其鍵值相同之節點，則會放棄此次插入操作並返回假。

### <span id="Modify">Modify</span>
- `bool Modify(const KeyType1& key1, const KeyType2& key2, const ValueType& value);`
  - 修改既有鍵值對。
  - 如若存在與其鍵值相同之節點，則會以 `value` 修改對應節點之值並返回真；
  - 如若不存在與其鍵值相同之節點，則會放棄此次操作並返回假。

### <span id="Flush">Flush</span>
- `FileDoubleUnrolledLinkedList& Flush();`
  - 將爲寫入儲存的緩衝區中之內容寫入與之關聯之檔案內。
  - 鑒於本函數使用 `std::fstream::flush()` ，請檢視更多關於
    [`std::fstream::flush()`](https://zh.cppreference.com/w/cpp/io/basic_ostream/flush)
    的詳情。

### <span id="Empty">Empty</span>
- `[[nodiscard]] bool Empty();`
  - 檢視此列表是否爲空。

### <span id="Exist">Exist</span>
- `[[nodiscard]] bool Exist(const KeyType1& key1, const KeyType2& key2);`
  - 檢查一個含有特定鍵之節點是否存在。
  - 若存在此節點，此節點將會被載入快取，因此不必擔心同時使用 `Exist` 和 `Get`
    成員函數會帶來時間損失。建議在使用 `Get` 前先使用 `Exist` 以檢查是否存在該節點。

### <span id="Get">Get</span>
- `[[nodiscard]] ValueType Get(const KeyType1& key1, const KeyType2& key2);`
  - 獲取特定鍵對應值。
  - 若不存在該節點，將會返回 `ValueType` 之默認構造函數。
  - 爲避免不存在該節點之情況，建議在使用此函數前先行使用
    `Exist` 函數檢查是否存在該節點。在此過程中，搜尋工作僅會執行一次。請檢視[成員函數 `Exist`](#Exist)
    獲取更多訊息。

### <span id="GetWithPointer">GetWithPointer</span>
- `[[nodiscard]] ValueType* GetWithPointer(const KeyType1& key1, const KeyType2& key2);`
  - 獲取特定鍵對應值之指針。
  - 如若該節點不存在，將會返回空指針。
  - 注意：修改此指針所指對象并不會修改節點中的內容。
  - 爲避免記憶體流失，若函數并不返回空指針，請一定在不需要此數據後釋放此空間。

### <span id="Traverse">Traverse</span>
- `std::vector<Node> Traverse();`
  - 獲得所有列表中的節點。
  - 此函數將返回一個包含所有列表中內容的
    `std::vector<lau::FileDoubleUnrolledLinkedList::Node>`。

- `std::vector<Node> Traverse(const KeyType1& key1);`
  - 獲得所有列表中包含特定第一鍵的鍵值對。
  - 此函數將返回一個包含所有特定第一鍵之節點的
    `std::vector<lau::FileDoubleUnrolledLinkedList::Node>`。
