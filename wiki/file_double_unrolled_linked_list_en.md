# lau::FileDoubleUnrolledLinkedList

Switch to Other Languages: [繁體中文（香港）](file_double_unrolled_linked_list_zh.md)

Included in header `lau/file_double_unrolled_linked_list.h` and
`lau/file_data_structure.h`.

```c++
namespace lau {
template<class KeyType1,
         class KeyType2,
         class ValueType>
class FileDoubleUnrolledLinkedList;
} // namespace lau
```

This template class is intended to preform as a double key map list on disk
storage that can keep the data even if the program is closed. This class
allows you to store and access data in programs that might be closed and
opened for several times.

This class uses the data structure called unrolled linked list to reduce
the time consumption on accessing and writing.  For more details, please
go to [the introduction part of the unrolled linked list](#Data_Structure).

Garbage collection is also available in this class.

<span id="Notice">Notice:</span>
1. The two key type **must** have valid operator< and operator==.
2. The two key and value types **must** have default constructor and trivial
   move and copy constructor.
3. This class cannot create a file itself, so **always** create the file before
   using the class.
4. The file that is used to store data **must** be either empty or storing the
   same kind of key-value pair.
5. Writing data of the same file in other classes is **undefined behaviour**!
   Always avoid it!

## Overview
```c++
namespace lau {
template<class KeyType1,
         class KeyType2,
         class ValueType>
class FileDoubleUnrolledLinkedList {
public:
    // Types
    typedef long Ptr;
    struct Node {
        KeyType1  key1;
        KeyType2  key2;
        ValueType value;
    };

    // Constructors
    explicit FileDoubleUnrolledLinkedList(const char* fileName,
                                          SizeT nodeSize = 500) noexcept;
    explicit FileDoubleUnrolledLinkedList(const std::string& fileName,
                                          SizeT nodeSize = 500) noexcept;
    FileDoubleUnrolledLinkedList(FileDoubleUnrolledLinkedList&&) noexcept;
    FileDoubleUnrolledLinkedList(const FileDoubleUnrolledLinkedList&) = delete;

    // Assignment Operators
    FileDoubleUnrolledLinkedList& operator=(FileDoubleUnrolledLinkedList&&) noexcept;
    FileDoubleUnrolledLinkedList& operator=(const FileDoubleUnrolledLinkedList&) = delete;

    // Destructor
    ~FileDoubleUnrolledLinkedList() = default;

    // Modifiers
    FileDoubleUnrolledLinkedList& Clear();
    bool Erase(const KeyType1& key1,
               const KeyType2& key2);
    bool Insert(const KeyType1& key1,
                const KeyType2& key2,
                const ValueType& value);
    bool Modify(const KeyType1& key1,
                const KeyType2& key2,
                const ValueType& value);

    // Operation
    FileDoubleUnrolledLinkedList& Flush();

    // Capacity
    [[nodiscard]] bool Empty() const;

    // Lookup
    [[nodiscard]] bool Exist(const KeyType1& key1,
                             const KeyType2& key2) const;
    [[nodiscard]] ValueType Get(const KeyType1& key1,
                                const KeyType2& key2) const;
    [[nodiscard]] ValueType* GetWithPointer(const KeyType1& key1,
                                            const KeyType2& key2) const;

    // Traversal
    std::vector<Node> Traverse() const;
    std::vector<Node> Traverse(const KeyType1& key1) const;
};
} // namespace lau
```

## Member Type
- `Ptr`: `long`, the type of file pointers
- `Node`: a `struct` making the key-value pair
    - `KeyType1 key1`
    - `keyType2 key2`
    - `ValueType value`

## Member Functions
- [(constructors)](#Constructors)
- [`operator=`](#operator=)
- (destructors)

### Modifiers
- [`Clear`](#Clear): clear all the data
- [`Erase`](#Erase): erase the corresponding key-value pair
- [`Insert`](#Insert): insert a new key-value pair
- [`Modify`](#Modify): modify the value of an existing key-value pair

### Operation
- [`Flush`](#Flush): synchronize with the underlying storage device

### Capacity
- [`Empty`](#Empty): tell whether the list is empty

### Lookup
- [`Exist`](#Exist): check whether a node with a certain key exists
- [`Get`](#Get): get the value of a key
- [`GetWithPointer`](#GetWithPointer): get the point of the value of a key

### Traversal
- [`Traverse`](#Traverse): traverse the list

## Details
### <span id="Data_Structure">Data Structure: Unrolled Linked List</span>
The data structure called unrolled linked list have a $O\left(\sqrt{n}\right)$
time complexity by using a linked list of arrays.  In this class, the linked
list is a doubly linked list.

Time complexity:
- Accessing: $O\left(\sqrt{n}\right)$
- Modifying: $O\left(\sqrt{n}\right)$
- Searching: $O\left(\sqrt{n}\cdot\log n\right)$

### <span id="Constructors">Constructors</span>
- `explicit lau::FileDoubleUnrolledLinkedList(const char* fileName, lau::FileDoubleUnrolledLinkedListSizeT nodeSize = 500) noexcept;`
    - Construct the list that is linked to the file with `fileName`.  Please
      **make sure** that this file **does exist**, or **undefined behaviour**
      will be encountered.
    - The number of array members is set as `nodeSize`.
    - Then it will check whether this file is empty or not.  If it is empty,
      a header will be created. If it isn't empty, then header will be loaded.
      For the sake of compatibility, the size of array might be set according
      to the header.  Therefore, the node size may not be `nodeSize`.
    - Please also read the [notice](#Notice) to avoid unexpected situation.

- `explicit FileDoubleUnrolledLinkedList(const std::string& fileName, SizeT nodeSize = 500) noexcept;`
    - Construct the list that is linked to the file with `fileName`.  Please
      **make sure** that this file **does exist**, or **undefined behaviour**
      will be encountered.
    - The number of array members is set as `nodeSize`.
    - Then it will check whether this file is empty or not.  If it is empty,
      a header will be created. If it isn't empty, then header will be loaded.
      For the sake of compatibility, the size of array might be set according
      to the header.  Therefore, the node size may not be `nodeSize`.
    - Please also read the [notice](#Notice) to avoid unexpected situation.

- `FileDoubleUnrolledLinkedList(FileDoubleUnrolledLinkedList&&) noexcept;`
    - Construct the class with a rvalue.

- `FileDoubleUnrolledLinkedList(const FileDoubleUnrolledLinkedList&) = delete;`
  - Since opening the same file in two place will cause undefined behaviour,
    the copy constructor is explicitly set invalid.

### <span id="operator=">`operator=`</span>
- `FileDoubleUnrolledLinkedList& operator=(FileDoubleUnrolledLinkedList&&) noexcept;`
  - See details about [constructors](#Constructors) for more information.

- `FileDoubleUnrolledLinkedList& operator=(const FileDoubleUnrolledLinkedList&) = delete;`
  - See details about [constructors](#Constructors) for more information.

### <span id="Clear">Clear</span>
- `FileDoubleUnrolledLinkedList& Clear();`
  - Clear all the data in the unrolled linked list.

### <span id="Erase">Erase</span>
- `bool Erase(const KeyType1& key1, const KeyType2& key2);`
  - Erase the corresponding key-value pair of the input key.
  - If there exists such a node, this function will erase the pair and return
    true;
  - If there is no such nodes, then it will abort this operation and return
    false.

### <span id="Insert">Insert</span>
- `bool Insert(const KeyType1& key1, const KeyType2& key2, const ValueType& value);`
  - Insert a new key-value pair.
  - If there is no such nodes with the same key (then this operation is
    successful), this function will return true;
  - If there exist a node with exactly the same key, it will abort this
    insert operation and return false.

### <span id="Modify">Modify</span>
- `bool Modify(const KeyType1& key1, const KeyType2& key2, const ValueType& value);`
  - Modify the value of an existing key-value pair.
  - If there exists such a node with the input key (both must match), this
    function will modify the value of such node and return true;
  - If there is no such nodes, then it will abort this operation and return
    false.

### <span id="Flush">Flush</span>
- `FileDoubleUnrolledLinkedList& Flush();`
  - Writes uncommitted changes to make everything in the buffer stored in the
    file.
  - See [more details](https://en.cppreference.com/w/cpp/io/basic_ostream/flush)
    about `std::fstream::flush()` that this function uses.

### <span id="Empty">Empty</span>
- `[[nodiscard]] bool Empty() const;`
  - Tell whether the list is empty.

### <span id="Exist">Exist</span>
- `[[nodiscard]] bool Exist(const KeyType1& key1, const KeyType2& key2) const;`
  - Check whether there exists a node with a certain key pair or not.
  - If a node is found existing, this node will be cached, so don't worry
    that using `Exist` and `Get`  member functions together will waste time.
    It's recommended to use `Exist` first to check whether such node exists
    and then use `Get` member function to get the value of the node.

### <span id="Get">Get</span>
- `[[nodiscard]] ValueType Get(const KeyType1& key1, const KeyType2& key2) const;`
  - Get the corresponding value of a certain key pair.
  - If there is no such node, it will return the default value (using the
    default constructor).
  - To avoid the case of not existing such a node, it's advisable to use
    `Exist` member function before using this function.  In this procedure,
    finding the node will only happen once.  View
    [`Exist` member function](#Exist) for more detail.

### <span id="GetWithPointer">GetWithPointer</span>
- `[[nodiscard]] ValueType* GetWithPointer(const KeyType1& key1, const KeyType2& key2) const;`
  - Get the point of the value of a certain key pair.
  - If the node doesn't exist, a nullptr will be returned instead.
  - Note that modifying the content this pointer is pointing **won't**
    actually modify the contents in this list.
  - To avoid memory leak, if the function doesn't return a nullptr, always
    free the memory (by using `delete` operator) whenever you don't need it.

### <span id="Traverse">Traverse</span>
- `std::vector<Node> Traverse() const;`
  - Get all the key-value pair in the list.
  - Return a `std::vector<lau::FileDoubleUnrolledLinkedList::Node>` containing
    all the Nodes in the list.

- `std::vector<Node> Traverse(const KeyType1& key1) const;`
  - Get all the key-value pair with a certain key1.
  - Return a `std::vector<lau::FileDoubleUnrolledLinkedList::Node>` containing
    all the Nodes with key1.
