# lau::RehashPolicy

切換到其他語言： [English](rehash_policy_en.md)

包含於標頭檔 `lau/rehash_policy.h` 中。

```c++
nasmepace lau {
class RehashPolicy;
}
```

`lau::RehashPolicy` 爲哈希表提供桶數量服務。`lau::LinkedHashTable` 和
`lau::LinkedHashMap` 均用到了此類實現重新整理桶數量的功能。

## 概覽
```c++
namespace lau {
class RehashPolicy {
public:
    // 構造函數
    RehashPolicy() noexcept;
    explicit RehashPolicy(SizeT minimumSize);
    RehashPolicy(const RehashPolicy&) noexcept = default;

    // 賦值運算符
    RehashPolicy& operator=(const RehashPolicy&) noexcept = default;

    // 析構函數
    ~RehashPolicy() = default;

    // 修改
    [[nodiscard]] SizeT NextSize() noexcept;
    [[nodiscard]] SizeT PreviousSize() noexcept;
    SizeT ReserveAtLeast(SizeT size);
    void SetSize(SizeT size);

    // 獲取
    [[nodiscard]] SizeT GetSize() const noexcept;
    [[nodiscard]] constexpr static SizeT MaxSize() noexcept;
};
} // namespace lau
```

## 成員函數
- [（構造函數）](#Constructors)
- [`operator=`](#operator=)
- （析構函數）

### 修改
- [`NextSize`](#NextSize)：設定並獲取更大的桶數量
- [`PreviousSize`](#PreviousSize)：設定並獲取更小的桶數量
- [`ReserveAtLeast`](#ReserveAtLeast)：預留至少給定的桶數量
- [`SetSize`](#SetSize)：設定桶數量

### 獲取
- [`GetSize`](#GetSize)：獲取桶數量
- [`MaxSize`](#MaxSize)：獲取最大桶數量

## 細節
### <span id="Constructors">構造函數</span>
```c++
RehashPolicy() noexcept;
```
- 默認構造函數
- 哈希表的桶數量將被設定爲 `0`。

```c++
explicit RehashPolicy(SizeT minimumSize);
```
- 保留至少 `minimumSize` 的桶數量。
- 注意：如果 `minimumSize` 大於 `MaxSize()`，則會被設定爲 `MaxSize()`。

```c++
RehashPolicy(const RehashPolicy&) noexcept = default;
```
- 拷貝構造函數
- 對應類中所有元素都將會被拷貝。
- 鑒於此類是*平凡可複製、頻繁可移動*的，因此移動構造函數與拷貝構造函數一致。

### <span id="operator=">`operator=`</span>
```c++
RehashPolicy& operator=(const RehashPolicy&) noexcept = default;
```
- 賦值運算符
- 對應類中所有元素都將會被拷貝。
- 鑒於此類是*平凡可複製、頻繁可移動*的，因此移動構造函數與拷貝構造函數一致。

### <span id="NextSize">`NextSize`</span>
```c++
[[nodiscard]] SizeT NextSize() noexcept;
```
- 增大桶數量。
- 返回新的桶數量。
- 注意：如果新的桶數量大於 `MaxSize()`，則會被設定爲 `MaxSize()`。
- 時間複雜度： $O(1)$。

### <span id="PreviousSize">`PreviousSize`</span>
```c++
[[nodiscard]] SizeT PreviousSize() noexcept;
```
- 減少桶數量。
- 返回新的桶數量。
- 注意：如果新的桶數量小於 `0`，則會被設定爲 `0`。
- 時間複雜度： $O(1)$。

### <span id="ReserveAtLeast">`ReserveAtLeast`</span>
```c++
SizeT ReserveAtLeast(SizeT size);
```
- 預留至少給定的桶數量。
- 如果輸入的桶數量小於 0，`lau::InvalidArgument` 將會被拋出。
- 返回新的桶數量。
- 注意：如果新的桶數量大於 `MaxSize()`，則會被設定爲 `MaxSize()`。
- 時間複雜度： $O(\log n)$。

### <span id="SetSize">`SetSize`</span>
```c++
void SetSize(SizeT size);
```
- 設定桶數量。
- 如果輸入的桶數量小於 0 或並不屬於類中定義的數量，`lau::InvalidArgument` 將會被拋出。
- 建議僅在需要回滾桶數量時使用。如果不知道需要的桶數量，推薦使用
  [`ReserveAtLeast`](#ReserveAtLeast) 成員函數。
- 時間複雜度： $O(\log n)$。

### <span id="GetSize">`GetSize`</span>
```c++
[[nodiscard]] SizeT GetSize() const noexcept;
```
- 獲取桶數量。
- 時間複雜度： $O(1)$。

### <span id="MaxSize">`MaxSize`</span>
```c++
[[nodiscard]] constexpr static SizeT MaxSize() noexcept;
```
- 獲取最大桶數量。
- 時間複雜度： $O(1)$。
