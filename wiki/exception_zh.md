# lau/exception.h

切換到其他語言： [English](exception_en.md)

`lau/eception.h` 標頭檔用於處理異常。

## 類（以字母順序排列）
- [Exception](#Exception)：專門處理異常的基類
- [OutOfRange](#OutOfRange)：表明操作越界
- [RuntimeError](#RuntimeError)：表明運行時發成錯誤
- [InvalidIterator](#InvalidIterator)：表明非法迭代器
- [EmptyContainer](#EmptyContainer)：表明此爲空容器
- - [InvalidArgument](#InvalidArgument)：表明存在非法參數

## 詳情
### <span id="Exception">lau::Exception</span>
此類專門用於處理異常（繼承自 `std::exception`）。此庫中所有類都繼承自此類。

概覽：
```c++
namespace lau {

class Exception : std::exception {
public:
    explicit Exception(const char* message) : message_(message) {}

    ~Exception() override = default;

    [[nodiscard]] const char* what() const noexcept override { return message_; }

protected:
    const char* message_;
};

} // namespace lau
```

### <span id="OutOfRange">lau::OutOfRange</span>
此類用以表明此操作由於下標越界而非法。

概覽：
```c++
namespace lau {

class OutOfRange : public Exception {
public:
    explicit OutOfRange(const char* message = "Index Out of Bound") : Exception(message) {}

    ~OutOfRange() override = default;

    [[nodiscard]] const char* what() const noexcept override { return message_; }
};

} // namespace lau
```

### <span id="RuntimeError">lau::RuntimeError</span>
此類用以表明發生運行時錯誤。

概覽：
```c++
namespace lau {

class RuntimeError : public Exception {
public:
    explicit RuntimeError(const char* message = "Runtime Error") : Exception(message) {}

    ~RuntimeError() override = default;

    [[nodiscard]] const char* what() const noexcept override { return message_; }
};

} // namespace lau
```

### <span id="InvalidIterator">lau::InvalidIterator</span>
此類用以表明操作對象爲非法迭代器。

概覽：
```c++
namespace lau {

class InvalidIterator : public Exception {
public:
    explicit InvalidIterator(const char* message = "Invalid Iterator") : Exception(message) {}

    ~InvalidIterator() override = default;

    [[nodiscard]] const char* what() const noexcept override { return message_; }
};

} // namespace lau
```

### <span id="EmptyContainer">lau::EmptyContainer</span>
此類用於表明操作對象爲空容器。

概覽：
```c++
namespace lau {

class EmptyContainer : public Exception {
public:
    explicit EmptyContainer(const char* message = "Empty Container") : Exception(message) {}

    ~EmptyContainer() override = default;

    [[nodiscard]] const char* what() const noexcept override { return message_; }
};

} // namespace lau
```

### <span id="InvalidArgument">lau::InvalidArgument</span>
此類用於表明存在非法參數。

概覽：
```c++
namespace lau {
class InvalidArgument : public Exception {
public:
    explicit InvalidArgument(const char* message = "Invalid Argument") : Exception(message){}

    ~InvalidArgument() override = default;

    [[nodiscard]] const char* what() const noexcept override { return message_; }
};
} // namespace lau
```
