# lau::KeyValuePair

切換到其他語言： [English](key_value_pair_en.md)

包含於標頭檔 `lau/ultility.h`。

```c++
nasmepace lau {
template<class Key, class Value>
class KeyValuePair;
}
```

`lau::KeyValuePair` 用於儲存 `key` 和 `value` 兩個元素，爲一鍵值對。

## 概覽
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

## 模板
- `Key`：`key` 的類型
- `Value`：`value` 的類型

## 成員函數
- [構造函數](#Constructors)
- [`operator=`](#operator=)
- （析構函數）

### 比較
- [`operator==`](#operator==)
- [`operator!=`](#operator!=)

### 非成員函數
- [`MakeKeyValuePair`](#MakeKeyValuePair)：建立一個 `KeyValuePair` 實體

## 詳情
### <span id="Constructors">Constructors</span>
```c++
KeyValuePair() noexcept(noexcept(Key()) && noexcept(Value())) : key(), value() {}
```
- 默認構造函數

```c++
KeyValuePair(const KeyValuePair& obj) = default;
```
- 複製構造函數

```c++
KeyValuePair(KeyValuePair&& obj)
    noexcept(noexcept(Key(std::move(obj.key))) && noexcept(Value(std::move(obj.value))))
    = default;
```
- 移動構造函數

```c++
KeyValuePair(const Key& key, const Value& value) noexcept(noexcept(Key(key)) && noexcept(Value(value)))
    : key(key), value(value) {}
```
- 以 `key` 和 `value` 構造此鍵值對。

```c++
template<class KeyIn, class ValueIn>
KeyValuePair(KeyIn&& key, ValueIn&& value) noexcept(noexcept(Key(key)) && noexcept(Value(value)))
    : key(key), value(value) {}
```
- 以 `key` 和 `value` 構造此鍵值對。
- `KetIn` 和 `ValueIn` 必須可轉換爲 `Key` 和 `Value` 各自的類型。

```c++
template<class KeyIn, class ValueIn>
KeyValuePair(const KeyValuePair<KeyIn, ValueIn>& obj)
    noexcept(noexcept(Key(obj.key)) && noexcept(Value(obj.value)))
    : key(obj.key), value(obj.value) {}
```
- 以 `lau::KeyValuePair` 構造此鍵值對。
- `KetIn` 和 `ValueIn` 必須可轉換爲 `Key` 和 `Value` 各自的類型。

```c++
template<class KeyIn, class ValueIn>
KeyValuePair(KeyValuePair<KeyIn, ValueIn>&& obj)
    noexcept(noexcept(Key(std::move(obj.key))) && noexcept(Value(std::move(obj.value))))
    : key(obj.key), value(obj.value) {}
```
- 以右值 `lau::KeyValuePair` 構造此鍵值對。
- `KetIn` 和 `ValueIn` 必須可轉換爲 `Key` 和 `Value` 各自的類型。

```c++
template<class KeyIn, class ValueIn>
KeyValuePair(const Pair<KeyIn, ValueIn>& obj) noexcept(noexcept(Key(obj.first)) && noexcept(Value(obj.second)))
    : key(obj.first), value(obj.second) {}
```
- 以 `lau::Pair` 的 `first` 和 `second` 構造此鍵值對。
- `KetIn` 和 `ValueIn` 必須可轉換爲 `Key` 和 `Value` 各自的類型。

```c++
template<class KeyIn, class ValueIn>
KeyValuePair(Pair<KeyIn, ValueIn>&& obj)
    noexcept(noexcept(Key(std::move(obj.first))) && noexcept(Value(std::move(obj.second))))
    : key(obj.first), value(obj.second) {}
```
- 以右值 `lau::Pair` 的 `first` 和 `second` 構造此鍵值對。
- `KetIn` 和 `ValueIn` 必須可轉換爲 `Key` 和 `Value` 各自的類型。

### <span id="operator=">operator=</span>
```c++
KeyValuePair& operator=(const KeyValuePair& obj) = default;
```
- 將此鍵值對設定爲 `obj` 的值。

```c++
KeyValuePair& operator=(KeyValuePair&& obj) = default;
```
- 將此鍵值對設定爲右值 `obj` 的值。

### <span id="operator==">operator==</span>
```c++
bool operator==(const KeyValuePair& obj) const;
```
- 比較此鍵值是否均對應相等。

### <span id="operator!=">operator!=</span>
```c++
bool operator!=(const KeyValuePair& obj) const;
```
- 比較鍵值對是否有任一不相等。

### <span id="MakeKeyValuePair">MakeKeyValuePair</span>
```c++
template<class Key, class Value>
KeyValuePair<Key, Value> MakeKeyValuePair(const Key& key, const Value& value);
```
- 構造一個鍵值對，其鍵爲 `key`，其值爲 `value`
- `Key` 和 `Value` 可透過模板類型推斷得出。
