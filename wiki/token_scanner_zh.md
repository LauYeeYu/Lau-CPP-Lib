# lau::TokenScanner

切換到其他語言： [English](token_scanner_en.md)

包含於標頭檔 `lau/token_scanner.h` 和 `lau/utility.h`中。

```c++
namespace lau {
class TokenScanner;
} // namespace lau
```

此類將一特定字符串以客製化分隔符掃描並分割爲多個字段。分割有兩種方式。連續方式 (multiple)
將連續分隔符視作與一個相同。逐一方式 (single) 將字符串以每個單獨的分隔符隔開。

## 概覽
```c++
namespace lau {
class TokenScanner {
public:
    // 枚舉
    enum TokenScannerMode {multiple, single};

    // 構造函數
    TokenScanner() = default;
    explicit TokenScanner(std::string input,
                          char delimiter = ' ',
                          lau::TokenScanner::TokenScannerMode mode = multiple);

    // 賦值運算符
    TokenScanner& operator=(const TokenScanner& obj);
    TokenScanner& operator=(TokenScanner&& obj) noexcept;

    // 析構函數
    ~TokenScanner() = default;

    // 元素訪問
    [[nodiscard]] char GetDelimiter() const noexcept;
    [[nodiscard]] const std::string& GetInputString() const noexcept;
    [[nodiscard]] TokenScannerMode GetMode() const noexcept;
    TokenScanner& SetDelimiter(char delimiter) noexcept;

    // 容量
    bool HasMoreToken() noexcept;
    long TotalLength() noexcept;

    // 操作
    TokenScanner& ChangeMode(TokenScannerMode mode) noexcept;
    TokenScanner& NewLine() noexcept;
    std::string NextToken() noexcept;
    std::string PeekNextToken() noexcept;
    TokenScanner& Read(std::string newInput) noexcept;
    TokenScanner& ResetState() noexcept;
    TokenScanner& SkipDelimiter() noexcept;
};
} // namespace lau
```

## 枚舉成員
```c++
enum TokenScannerMode {multiple, single};
```
- 兩種掃描方式
- `multiple`：連續分隔符視爲與一個相同
- `single`：以每個單獨的分隔符隔開

## 成員函數
- [（構造函數）](#Constructors)
- [`operator=`](#operator=)
- （析構函數）

### 成員訪問
- [`GetDelimiter`](#GetDelimiter)：獲取分隔符
- [`GetInputString`](#GetInputString)：獲取輸入字符串
- [`GetMode`](#GetMode)：獲取掃描方式
- [`SetDelimiter`](#SetDelimiter)：設定分隔符

### 容量
- [`HasMoreToken`](#HasMoreToken)：表明是否存在更多字段
- [`TotalLength`](#TotalLength)：輸入字符串全長

### 操作
- [`ChangeMode`](#ChangeMode)：更改掃描方式
- [`NewLine`](#NewLine)：從 `std::cin` 讀取一行數據
- [`NextToken`](#NextToken)：獲取下一個字段
- [`PeekNextToken`](#PeekNextToken)：檢視下一個字段
- [`Read`](#Read)：讀取内容
- [`ResetState`](#ResetState)：重置掃描態
- [`SkipDelimiter`](#SkipDelimiter)：跳過目前遇到的分隔符

## 例子
```c++
#include <iostream>

#include "lau/token_scanner.h"

int main() {
    lau::TokenScanner a;

    a.Read("first second third fourth ...");
    std::cout << a.TotalLength() << std::endl;
    while (a.HasMoreToken()) {
        std::cout << a.NextToken() << " " << a.PeekNextToken() << std::endl;
    }

    lau::TokenScanner b("first||second third|fourth|fifth|sixth",
                        '|',
                        lau::TokenScanner::single);
    while (b.HasMoreToken()) {
        std::cout << b.PeekNextToken() << std::endl;
        std::cout << b.NextToken() << std::endl;
        std::cout << b.PeekNextToken() << std::endl;
    }
    b.ResetState();
    std::cout << b.NextToken() << std::endl;
    b.ChangeMode(lau::TokenScanner::multiple).SkipDelimiter();
    while (b.HasMoreToken()) {
        std::cout << b.PeekNextToken() << " " << b.NextToken() << " " << b.PeekNextToken() << std::endl;
    }
    b.Read("second third|fourth|fifth|sixth").SetDelimiter(' ');
    while (b.HasMoreToken()) {
        std::cout << b.PeekNextToken() << " " << b.NextToken() << " " << b.PeekNextToken() << std::endl;
    }
    b = a;
    std::cout << "b now at ";
    if (b.GetMode() == lau::TokenScanner::multiple) std::cout << "multiple mode";
    else std::cout << "single mode";
    std::cout << "with delimiter [" << b.GetDelimiter() << "] and string" << b.GetInputString() << std::endl;
    while (b.HasMoreToken()) {
        std::cout << b.PeekNextToken() << " " << b.NextToken() << " " << b.PeekNextToken() << std::endl;
    }
    b = std::move(a);
    std::cout << b.GetInputString() << std::endl;
    return 0;
}
```

上述程序之輸出爲：

```
29
first second
second third
third fourth
fourth ...
... 
first
first



second third
second third
second third
fourth
fourth
fourth
fifth
fifth
fifth
sixth
sixth
sixth

first
second third second third fourth
fourth fourth fifth
fifth fifth sixth
sixth sixth 
second second third|fourth|fifth|sixth
third|fourth|fifth|sixth third|fourth|fifth|sixth 
b now at multiple modewith delimiter [ ] and stringfirst second third fourth ...
first second third fourth ...
```

## 詳情
### <span id="Constructors">構造函數</span>
```c++
lau::TokenScanner();
```
- 默認構造函數，空輸入字符串，空格爲分隔符，連續掃描模式。

```c++
explicit lau::TokenScanner(std::string input,
                           char delimiter = ' ',
                           TokenScannerMode mode = multiple);
```
- 建構一個以 `input` 爲輸入串，以 `delimiter` 爲分隔符，以 `mode` 爲分割方式的 `TokenScanner` 類。

### <span id="NextToken">NextToken</span>
```c++
std::string NextToken() noexcept;
```
- 掃描並獲取下一個字段， `lau::TokenScanner::NextToken` 和
  `lau::TokenScanner::PeekNextToken` 都不在會顯示這個字段（除非調用了
  `lau::TokenScanner::ResetState`）。

### <span id="PeekNextToken">PeekNextToken</span>
```c++
std::string PeekNextToken() noexcept;
```
- 檢視下一個字段，但不移動掃描指針，不會造成任何宏觀影響。

### <span id="HasMoreToken">HasMoreToken</span>
```c++
bool HasMoreToken() noexcept;
```
- 表明是否有仍有未掃描到的字段。

### <span id="NewLine">NewLine</span>
```c++
TokenScanner& NewLine() noexcept;
```
- 以 `std::getline` 函數從 `std::cin` 讀取一行。
- 鑒於此函數透過 `std::getline` 獲取一行，請參閲
  [`std::getline`](https://en.cppreference.com/w/cpp/string/basic_string/getline)
  瞭解更多訊息。

### <span id="TotalLength">TotalLength</span>
```c++
long TotalLength() noexcept;
```
- 返回輸入串的長度。
- 鑒於 [Lau Yee-Yu's C++ Style Guide](https://github.com/LauYeeYu/Code-Style) ，
  此函數并不會以 `std::string::length()` 的返回類型 (`size_t`) 返回。而是以有符號的 `long` 返回。

### <span id="ChangeMode">ChangeMode</span>
- `lau::TokenScanner& lau::TokenScanner::ChangeMode(TokenScannerMode mode) noexcept;`
- 改變掃描方式
- `mode` 必須是 `TokenScanner::single` 或 `TokenScanner::multiple`。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。
- 由於兩種掃描方式有不同的實現方式，如在掃描已經開始后更改掃描方式將導致**未定義行爲**！强烈建議使用
  `lau::TokenScanner::SkipDelimiter()` 或 `lau::TokenScanner::ResetState()`
  以避免意料之外的問題。

### <span id="ResetState">ResetState</span>
```c++
TokenScanner& ResetState() noexcept;
```
- 重置掃描狀態，將掃描指針還原到字符串起點。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。

### <span id="Read">Read</span>
```c++
TokenScanner& Read(std::string newInput) noexcept;
```
- 讀取一個新的字符串，將掃描指針置於新串之起點。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。

### <span id="SkipDelimiter">SkipDelimiter</span>
```c++
TokenScanner& SkipDelimiter() noexcept;
```
- 掠過下一個字段前的所有間隔符。
- 若遇到輸入串的末尾，掃描指針將在末尾處停止。
- 爲使代碼更加整潔，訪問更加簡便，此函數返回原類的引用。

### <span id="SetDelimiter">SetDelimiter</span>
```c++
TokenScanner& SetDelimiter(char delimiter) noexcept;
```
- 設定新的分隔符，之前的掃描會被留下。
- 由於兩種掃描方式有不同的實現方式，如在掃描已經開始后更改分隔符將導致**未定義行爲**！强烈建議使用
  `lau::TokenScanner::SkipDelimiter()` 或 `lau::TokenScanner::ResetState()`
  以避免意料之外的問題。

### <span id="GetInputString">GetInputString</span>
```c++
[[nodiscard]] const std::string& GetInputString() const noexcept;
```
- 獲取輸入字符串。

### <span id="GetDelimiter">GetDelimiter</span>
```c++
[[nodiscard]] char GetDelimiter() const noexcept;
```
- 獲取分隔符。

### <span id="GetMode">GetMode</span>
```c++
[[nodiscard]] TokenScannerMode GetMode() const noexcept;
```
- 獲取掃描方式。

### <span id="operator=">operator=</span>
```c++
TokenScanner& operator=(const TokenScanner& obj);
TokenScanner& operator=(TokenScanner&& obj) noexcept;
```
