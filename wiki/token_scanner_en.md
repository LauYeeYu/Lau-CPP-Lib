# lau::TokenScanner
included in header `lau/token_scanner.h` and `lau/utility.h`.

```c++
namespace lau {
class TokenScanner;
} // namespace lau
```

This class is intended to separate an input string into pieces with customized
delimiter.  There are two method of cutting the input string. In the single
mode, the class cuts the string with every single delimiter, while in the
multiple mode, it treats multiple delimiters as one delimiter.

## Overview
```c++
namespace lau {
class TokenScanner {
public:
    // Enum
    enum TokenScannerMode {multiple, single};

    // Constructors
    TokenScanner() = default;
    explicit TokenScanner(std::string input,
                          char delimiter = ' ',
                          lau::TokenScanner::TokenScannerMode mode = multiple);

    // Assignment Operators
    TokenScanner& operator=(const TokenScanner& obj);
    TokenScanner& operator=(TokenScanner&& obj) noexcept;

    // Destructor
    ~TokenScanner() = default;

    // Element Accessing
    TokenScanner& SetDelimiter(char delimiter) noexcept;
    [[nodiscard]] const std::string& GetInputString() const noexcept;
    [[nodiscard]] char GetDelimiter() const noexcept;
    [[nodiscard]] TokenScannerMode GetMode() const noexcept;

    // Capacity
    bool HasMoreToken() noexcept;
    long TotalLength() noexcept;

    // Operations
    std::string NextToken() noexcept;
    std::string PeekNextToken() noexcept;
    TokenScanner& NewLine() noexcept;
    TokenScanner& ChangeMode(TokenScannerMode mode) noexcept;
    TokenScanner& Read(std::string newInput) noexcept;
    TokenScanner& ResetState() noexcept;
    TokenScanner& SkipDelimiter() noexcept;
};
} // namespace lau
```

## Member Enum
- `enum TokenScannerMode {multiple, single};`
    - The two scanning mode in token scanner.
    - `multiple`: Multiple delimiters are treated the same as one delimiter.
    - `single`: The input string is cut with every delimiter.

## Member Functions
- [(constructors)](#Constructors)
- [`operator=`](#operator=)
- (destructors)

### Element Accessing
- [`SetDelimiter`](#SetDelimiter): set the delimiter.
- [`GetInputString`](#GetInputString): get the input string.
- [`GetDelimiter`](#GetDelimiter): get the delimiter.
- [`GetMode`](#GetMode): get the scanning mode.

### Capacity
- [`HasMoreToken`](#HasMoreToken): indicate whether there are more tokens.
- [`TotalLength`](#TotalLength): get the total length of the input string.

### Operations
- [`NextToken`](#NextToken): get next token.
- [`PeekNextToken`](#PeekNextToken): peek next token.
- [`NewLine`](#NewLine): read a new line from `std::cin`.
- [`ChangeMode`](#ChangeMode): change the scanning mode.
- [`ResetState`](#ResetState): reset the scanning state.
- [`Read`](#Read): read a new input string.
- [`SkipDelimiter`](#SkipDelimiter): skip the upcoming delimiter.

## Example

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

The output of the program above is:

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

## Details
### <span id="Constructors">Constructors</span>
- `lau::TokenScanner();`
    - default constructor, with an empty buffer and a default delimiter (space);

- `explicit lau::TokenScanner(std::string input, char delimiter = ' ', lau::TokenScanner::TokenScannerMode mode = lau::TokenScanner::multiple);`
    - construct a class with `input` string, delimiter (`delimiter`) and scanning mode (`mode`).

### <span id="NextToken">`NextToken`</span>
- `std::string lau::TokenScanner::NextToken() noexcept;`
    - Get the new token, if `lau::TokenScanner::NextToken` or
      `lau::TokenScanner::PeekNextToken` is called then, neither will return
      this token again (except the case that `lau::TokenScanner::ResetState`
      is called).

### <span id="PeekNextToken">`PeekNextToken`</span>
- `std::string lau::TokenScanner::PeekNextToken() noexcept;`
    - Get the new token, without moving the scanning pointer. Different from
      `TokenScanner::NextToken`, it won't cause any difference whether this
      function is called or not. Namely, it just peeks the next token without
      causing any major differences.

### <span id="HasMoreToken">`HasMoreToken`</span>
- `bool lau::TokenScanner::HasMoreToken() noexcept;`
    - Tell whether there remains token(s) or not.

### <span id="NewLine">`NewLine`</span>
- `lau::TokenScanner& lau::TokenScanner::NewLine() noexcept;`
    - Read a new line from `std::cin` with `std::getline` function.
    - Since this function uses `std::getline` to get a new line, please view
      [std::getline](https://en.cppreference.com/w/cpp/string/basic_string/getline)
      for more information.

### <span id="TotalLength">`TotalLength`</span>
- `long lau::TokenScanner::TotalLength() noexcept;`
    - Tell the length of the input string.
    - Due to [Lau Yee-Yu's C++ Style Guide](https://github.com/LauYeeYu/Code-Style),
      this function doesn't use the return type of
      `std::string::length()` (`size_t`), but the signed `long` type.

### <span id="ChangeMode">`ChangeMode`</span>
- `lau::TokenScanner& lau::TokenScanner::ChangeMode(TokenScannerMode mode) noexcept;`
    - Change the scanning mode of the token scanner.
    - `mode` have to be either `TokenScanner::single` or `TokenScanner::multiple`
    - To make the class visiting easier, the function returns a reference
      pointing to this class.
    - Since the two scanning modes may use different implementations, changing
      the scanning mode in the scanning process may cause **undefined behaviour**!
      So it is highly recommended calling the `lau::TokenScanner::SkipDelimiter()`
      or `lau::TokenScanner::ResetState()` to avoid unexpected result.

### <span id="ResetState">`ResetState`</span>
- `lau::TokenScanner& lau::TokenScanner::ResetState() noexcept;`
    - Reset the scanning state, making the scanning pointer back
      to the front of the string.
    - To make the class visiting easier, the function returns a reference
      pointing to this class.

### <span id="Read">`Read`</span>
- `lau::TokenScanner& lau::TokenScanner::Read(std::string newInput) noexcept;`
    - Read a new input string, and set the scanning pointer to
      the front of the new input string.
    - To make the class visiting easier, the function returns a reference
      pointing to this class.

### <span id="SkipDelimiter">`SkipDelimiter`</span>
- `lau::TokenScanner& lau::TokenScanner::SkipDelimiter() noexcept;`
    - Skip the delimiter(s) before the next token.
    - If this function meets the end of the input string, it will stop at the
      end of the input string.
    - To make the class visiting easier, the function returns a reference
      pointing to this class.

### <span id="SetDelimiter">`SetDelimiter`</span>
- `lau::TokenScanner& lau::TokenScanner::SetDelimiter(char delimiter) noexcept;`
    - Set the new delimiter, the previous scanned result will be preserved.
    - Since the two scanning modes may use different implementations, setting a
      new delimiter halfway in the scanning process may cause **undefined behaviour**!
      So it is highly recommended calling the `lau::TokenScanner::SkipDelimiter()`
      or `lau::TokenScanner::ResetState()` to avoid unexpected result.

### <span id="GetInputString">`GetInputString`</span>
- `[[nodiscard]] const std::string& GetInputString() const noexcept;`
    - Get the input string.

### <span id="GetDelimiter">`GetDelimiter`</span>
- `[[nodiscard]] char GetDelimiter() const noexcept;`
    - Get the delimiter.

### <span id="GetMode">`GetMode`</span>
- `[[nodiscard]] TokenScannerMode GetMode() const noexcept;`
    - Get the scanning mode.

### <span id="operator=">`operator=`</span>
- `TokenScanner& operator=(const TokenScanner& obj);`
- `TokenScanner& operator=(TokenScanner&& obj) noexcept;`
