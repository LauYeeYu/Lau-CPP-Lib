# lau/algorithm.h

切換到其他語言： [English](algorithm_en.md)

`lau/algorithm.h` 標頭檔包含了 Lau CPP Lib 中所有算法。

## 算法
- [Knuth-Morris-Pratt 算法（KMP 算法）](#Knuth-Morris-Pratt_Algorithm)：高效的字符串匹配算法

## 函數（以字母順序排列）
- [`PartialMatchTable`](#PartialMatchTable)：計算
  [KMP 算法](#Knuth-Morris-Pratt_Algorithm)中部分匹配表
- [`StringMatching`](#StringMatching)：以
  [KMP 算法](#Knuth-Morris-Pratt_Algorithm)匹配字符串
- [`StringMatchingWithPartialMatchTable`](#StringMatchingWithPartialMatchTable)：透過已經處理完畢的部分匹配表作字符串匹配

## 詳情
### <span id="Knuth-Morris-Pratt_Algorithm">Knuth-Morris-Pratt 算法</span>
Knuth-Morris-Pratt 算法（KMP 算法）在 1977 發佈。與樸素算法（時間複雜度爲
$O(m \times n)$）不同的是，此算法相當高效（時間複雜度爲 $O(m + n)$）。

此算法透過部分匹配表來避免重複匹配。如需瞭解更多關於此算法的信息，請檢視[維基百科頁](https://zh.wikipedia.org/wiki/KMP%E7%AE%97%E6%B3%95) 。

時間複雜度： $O(m \times n)$

相關内容：
- [`PartialMatchTable`](#PartialMatchTable)：計算
  [KMP 算法](#Knuth-Morris-Pratt_Algorithm)中部分匹配表
- [`StringMatching`](#StringMatching)：以
  [KMP 算法](#Knuth-Morris-Pratt_Algorithm)匹配字符串
- [`StringMatchingWithPartialMatchTable`](#StringMatchingWithPartialMatchTable)：透過已經處理完畢的部分匹配表作字符串匹配


### <span id="PartialMatchTable">`PartialMatchTable`</span>
```c++
template<class T, class Container>
Container& PartialMatchTable(const T& target, SizeT size, Container& table);
```
- 計算 [Knuth-Morris-Pratt 算法](#Knuth-Morris-Pratt_Algorithm)所需要之部分匹配表。
- 返回容器的引用。
- 請注意：`table` 必須不小於 `target` 的大小。

### <span id="StringMatchingWithPartialMatchTable">`StringMatchingWithPartialMatchTable`</span>
```c++
template<class T, class Container>
[[nodiscard]] SizeT StringMatchingWithPartialMatchTable(const T& source,
                                                        SizeT sourceLength,
                                                        const T& target,
                                                        SizeT targetLength,
                                                        const Container& table);
```
- 以已處理完畢之部分匹配表查找首個目標串與源字符串匹配之處。可以使用
  [`lau::PartialMatchTable`](#PartialMatchTable) 來預處理。
- 輸入之部分匹配表必須與 [`lau::PartialMatchTable`](#PartialMatchTable) 處理結果相一致。
- 此函數使用 [Knuth-Morris-Pratt 算法](#Knuth-Morris-Pratt_Algorithm)。
- 返回首個匹配之處；如不存在匹配之處，返回 `-1`。
- 請注意：`T` 必須含有 `operator[]` 運算符。

### <span id="StringMatching">`StringMatching`</span>
```c++
template<class T, class Allocator = std::allocator<SizeT>>
[[nodiscard]] SizeT StringMatching(const T& source, SizeT sourceLength,
                                   const T& target, SizeT targetLength,
                                   Allocator allocator = Allocator());
```
- 查找首個目標串與源字符串匹配之處。
- 此函數使用 [Knuth-Morris-Pratt 算法](#Knuth-Morris-Pratt_Algorithm)。
- 返回首個匹配之處；如不存在匹配之處，返回 `-1`。
- 請注意：`T` 必須含有 `operator[]` 運算符。

```c++
template<class Allocator = std::allocator<SizeT>>
[[nodiscard]] SizeT StringMatching(const char* source,
                                   const char* target,
                                   Allocator allocator = Allocator());
```
- 查找首個目標串與源字符串匹配之處。
- 此函數使用 [Knuth-Morris-Pratt 算法](#Knuth-Morris-Pratt_Algorithm)。
- 返回首個匹配之處；如不存在匹配之處，返回 `-1`。
- 請注意：`T` 必須含有 `operator[]` 運算符。

```c++
template<class Allocator = std::allocator<SizeT>>
[[nodiscard]] SizeT StringMatching(const std::string& source,
                                   const std::string& target,
                                   Allocator allocator = Allocator());
```
- 查找首個目標串與源字符串匹配之處。
- 此函數使用 [Knuth-Morris-Pratt 算法](#Knuth-Morris-Pratt_Algorithm)。
- 返回首個匹配之處；如不存在匹配之處，返回 `-1`。
- 請注意：`T` 必須含有 `operator[]` 運算符。
