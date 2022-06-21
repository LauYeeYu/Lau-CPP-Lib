# lau::KeyValuePair

Switch to Other Languages: [繁體中文（香港）](KeyValuePair_zh.md)

Included in header `lau/ultility.h`.

```c++
nasmepace lau {
template<class Key, class Value>
class KeyValuePair;
}
```

`lau::KeyValuePair` is a class that stores two elements `key` and `value`.

## Overview
```c++
namespace lau {

template<class Key, class Value>
struct KeyValuePair {
    KeyValuePair() noexcept(noexcept(Key()) && noexcept(Value())) : key(), value() {}
    KeyValuePair(const KeyValuePair& obj) = default;
    KeyValuePair(KeyValuePair&& obj)
        noexcept(noexcept(Key(std::move(obj.key))) && noexcept(Value(std::move(obj.value))))
        = default;

    KeyValuePair(const Key& key, const Value& value) noexcept(noexcept(Key(key)) && noexcept(Value(value)))
        : key(key), value(value) {}

    template<class KeyIn, class ValueIn>
    KeyValuePair(KeyIn&& key, ValueIn&& value) noexcept(noexcept(Key(key)) && noexcept(Value(value)))
        : key(key), value(value) {}

    template<class KeyIn, class ValueIn>
    KeyValuePair(const KeyValuePair<KeyIn, ValueIn>& obj)
        noexcept(noexcept(Key(obj.key)) && noexcept(Value(obj.value)))
        : key(obj.key), value(obj.value) {}

    template<class KeyIn, class ValueIn>
    KeyValuePair(KeyValuePair<KeyIn, ValueIn>&& obj)
        noexcept(noexcept(Key(std::move(obj.key))) && noexcept(Value(std::move(obj.value))))
        : key(obj.key), value(obj.value) {}

    template<class KeyIn, class ValueIn>
    KeyValuePair(const Pair<KeyIn, ValueIn>& obj) noexcept(noexcept(Key(obj.first)) && noexcept(Value(obj.second)))
        : key(obj.first), value(obj.second) {}

    template<class KeyIn, class ValueIn>
    KeyValuePair(Pair<KeyIn, ValueIn>&& obj)
        noexcept(noexcept(Key(std::move(obj.first))) && noexcept(Value(std::move(obj.second))))
        : key(obj.first), value(obj.second) {}

    KeyValuePair& operator=(const KeyValuePair& obj) = default;
    KeyValuePair& operator=(KeyValuePair&& obj) = default;

    ~KeyValuePair() = default;

    bool operator==(const KeyValuePair& obj) const;
    bool operator!=(const KeyValuePair& obj) const;

    Key   key;
    Value value;
};

template<class Key, class Value>
KeyValuePair<Key, Value> MakeKeyValuePair(const Key& key, const Value& value);

} // namespace lau
```

## Template
- `Key`: the type of the key
- `Value`: the type of the value

## Member Functions
- [(constructors)](#Constructors)
- [`operator=`](#operator=)
- (destructors)

### Comparison
- [`operator==`](#operator==)
- [`operator!=`](#operator!=)

### Non-member Functions
- [`MakeKeyValuePair`](#MakeKeyValuePair)

## Details
### <span id="Constructors">Constructors</span>
```c++
KeyValuePair() noexcept(noexcept(Key()) && noexcept(Value())) : key(), value() {}
```
- Default constructor

```c++
KeyValuePair(const KeyValuePair& obj) = default;
```
- Copy constructor

```c++
KeyValuePair(KeyValuePair&& obj)
    noexcept(noexcept(Key(std::move(obj.key))) && noexcept(Value(std::move(obj.value))))
    = default;
```
- Move constructor

```c++
KeyValuePair(const Key& key, const Value& value) noexcept(noexcept(Key(key)) && noexcept(Value(value)))
    : key(key), value(value) {}
```
- Initialize the pair with the given key and value.

```c++
template<class KeyIn, class ValueIn>
KeyValuePair(KeyIn&& key, ValueIn&& value) noexcept(noexcept(Key(key)) && noexcept(Value(value)))
    : key(key), value(value) {}
```
- Initialize the pair with the given key and value.
- The type `KeyIn` and `ValueIn` must be convertible to `Key` and `Value`
  respectively.

```c++
template<class KeyIn, class ValueIn>
KeyValuePair(const KeyValuePair<KeyIn, ValueIn>& obj)
    noexcept(noexcept(Key(obj.key)) && noexcept(Value(obj.value)))
    : key(obj.key), value(obj.value) {}
```
- Initialize the pair with the given rvalue key and value.
- The type `KeyIn` and `ValueIn` must be convertible to `Key` and `Value`
  respectively.

```c++
template<class KeyIn, class ValueIn>
KeyValuePair(KeyValuePair<KeyIn, ValueIn>&& obj)
    noexcept(noexcept(Key(std::move(obj.key))) && noexcept(Value(std::move(obj.value))))
    : key(obj.key), value(obj.value) {}
```
- Initialize the pair with the given rvalue key and value.
- The type `KeyIn` and `ValueIn` must be convertible to `Key` and `Value`
  respectively.

```c++
template<class KeyIn, class ValueIn>
KeyValuePair(const Pair<KeyIn, ValueIn>& obj) noexcept(noexcept(Key(obj.first)) && noexcept(Value(obj.second)))
    : key(obj.first), value(obj.second) {}
```
- Initialize the pair with the given `lau::Pair`.
- The type `KeyIn` and `ValueIn` must be convertible to `Key` and `Value`
  respectively.

```c++
template<class KeyIn, class ValueIn>
KeyValuePair(Pair<KeyIn, ValueIn>&& obj)
    noexcept(noexcept(Key(std::move(obj.first))) && noexcept(Value(std::move(obj.second))))
    : key(obj.first), value(obj.second) {}
```
- Initialize the pair with the given rvalue `lau::Pair`.

### <span id="operator=">operator=</span>
```c++
KeyValuePair& operator=(const KeyValuePair& obj) = default;
```
- Assign the key value pair with the given key value pair.

```c++
KeyValuePair& operator=(KeyValuePair&& obj) = default;
```
- Assign the key value pair with the given rvalue key value pair.

### <span id="operator==">operator==</span>
```c++
bool operator==(const KeyValuePair& obj) const;
```
- Return whether the key and value is equal to the given key and value.

### <span id="operator!=">operator!=</span>
```c++
bool operator!=(const KeyValuePair& obj) const;
```
- Return whether the key and value is not equal to the given key and value.

### <span id="MakeKeyValuePair">MakeKeyValuePair</span>
```c++
template<class Key, class Value>
KeyValuePair<Key, Value> MakeKeyValuePair(const Key& key, const Value& value);
```
- Return a key value pair with the given key and value.
- The type `Key` and `Value` can be deduced from the arguments.
