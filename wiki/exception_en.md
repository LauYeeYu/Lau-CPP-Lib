# lau/exception.h

Switch to Other Languages: [繁體中文（香港）](exception_zh.md)

The `lau/eception.h` header file is intended to process exceptions.

## Classes (in Alphabetical Order)
- [Exception](#Exception): a base class dedicated for exceptions
- [IndexOutOfBound](#IndexOutOfBound): indicate that the index is out of bound
- [RuntimeError](#RuntimeError): indicate a runtime error occurs
- [InvalidIterator](#InvalidIterator): indicate this is an invalid iterator
- [EmptyContainer](#EmptyContainer): indicate this is an empty container

## Details
### <span id="Exception">lau::Exception</span>
A class (derived from `std::exception`) that is dedicated for exceptions.
All exception class is derived from this class.

Overview:
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

### <span id="IndexOutOfBound">lau::IndexOutOfBound</span>
A class to indicate that this access is valid because it accesses something
out of the bound.

Overview:
```c++
namespace lau {

class IndexOutOfBound : public Exception {
public:
    explicit IndexOutOfBound(const char* message = "Index Out of Bound") : Exception(message) {}

    ~IndexOutOfBound() override = default;

    [[nodiscard]] const char* what() const noexcept override { return message_; }
};

} // namespace lau
```

### <span id="RuntimeError">lau::RuntimeError</span>
A class to indicate that an error during runtime occurs.

Overview:
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
A class to indicate this operation is done on an invalid iterator.

Overview:
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
A class to indicate this operation is done on an empty container.

Overview:
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
