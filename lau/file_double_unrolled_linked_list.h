/**
 * @file lau/file_double_unrolled_linked_list.h
 *
 * This is a external header file, including a
 * <code>lau::FileDoubleUnrolledLinkedList</code> class.
 */

#ifndef LAU_CPP_LIB_LAU_FILE_DOUBLE_UNROLLED_LINKED_LIST_H
#define LAU_CPP_LIB_LAU_FILE_DOUBLE_UNROLLED_LINKED_LIST_H

#include <fstream>
#include <vector>

namespace lau {

/**
 * @class FileDoubleUnrolledLinkedList
 *
 * This is a template class of double unrolled linked list on disk storage.
 *
 * @tparam KeyType1 Type of First key
 * @tparam KeyType2 Type of Second key
 * @tparam ValueType Type of Value
 *
 * WARNING:
 * <br>
 * 1. The two key type MUST have valid operator< and operator== !
 * <br>
 * 2. The two key and value types must have default constructor and
 * trivial move and copy constructor.
 */
template<class KeyType1, class KeyType2, class ValueType>
class FileDoubleUnrolledLinkedList {
public:
    typedef long Ptr;
    typedef long SizeT;

    /**
     * @struct Node{key1, key2, value}
     *
     * This is the Node to store data.
     */
    struct Node {
        KeyType1  key1;
        KeyType2  key2;
        ValueType value;
    };

    explicit FileDoubleUnrolledLinkedList(const std::string& fileName, SizeT nodeSize = 500) noexcept
        : list_(fileName), head_{0, 0, 0, nodeSize, 2 * nodeSize} {
        list_.seekg(0);
        list_.seekp(0);
        if (list_.peek() == EOF) {
            list_.seekg(0, std::ios::beg);
            list_.seekp(0, std::ios::beg);
            list_.write(reinterpret_cast<char*>(&head_), sizeof(FirstNode_));
        } else {
            list_.read(reinterpret_cast<char*>(&head_), sizeof(FirstNode_));
        }
    }

    explicit FileDoubleUnrolledLinkedList(const char* fileName, SizeT nodeSize = 500) noexcept
        : list_(fileName), head_{0, 0, 0, nodeSize, 2 * nodeSize} {
        list_.seekg(0);
        list_.seekp(0);
        if (list_.peek() == EOF) {
            list_.seekg(0, std::ios::beg);
            list_.seekp(0, std::ios::beg);
            list_.write(reinterpret_cast<char*>(&head_), sizeof(FirstNode_));
        } else {
            list_.read(reinterpret_cast<char*>(&head_), sizeof(FirstNode_));
        }
    }

    FileDoubleUnrolledLinkedList(FileDoubleUnrolledLinkedList&&) noexcept = default;

    FileDoubleUnrolledLinkedList(const FileDoubleUnrolledLinkedList&) = delete;

    FileDoubleUnrolledLinkedList& operator=(FileDoubleUnrolledLinkedList&&) noexcept = default;

    FileDoubleUnrolledLinkedList& operator=(const FileDoubleUnrolledLinkedList&) = delete;

    ~FileDoubleUnrolledLinkedList() = default;

    /**
     * Insert a new key-value pair.
     * @param key1 the new key1
     * @param key2 the new key2
     * @param value the value of the new key
     * @return whether the operation is successful or not
     */
    bool Insert(const KeyType1& key1, const KeyType2& key2, const ValueType& value) {
        // Search the place to accommodate to new pair
        auto [mainNodePtr, index] = Find_(key1, key2);

        // The case of no nodes at all
        if (mainNodePtr == 0) {
            MainNode_ mainNode{key1, key2, value, 0, 0, 0, 0};
            NewNode_(mainNode, 0);

            // put the new node into cache (store the data in memory to boost efficiency)
            cachedNode_.key1 = key1;
            cachedNode_.key2 = key2;
            cachedNode_.value = value;
            cached = true;

            return true;
        }

        // Get the main node
        MainNode_ mainNode; // the place to place the new node
        list_.seekg(mainNodePtr);
        list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

        Node_ tmpNode;
        if (mainNode.pre == 0 && (key1 < mainNode.key1 || (key1 == mainNode.key1 && key2 < mainNode.key2))) {
            // Push back the data
            char* buffer = Read_(mainNode.target, mainNode.count * sizeof(Node_));
            Write_(buffer, mainNode.target + sizeof(Node_), mainNode.count * sizeof(Node_));
            delete[] buffer;

            // Move the data in main node to the first node of its array
            tmpNode.key1 = mainNode.key1;
            tmpNode.key2 = mainNode.key2;
            tmpNode.value = mainNode.value;
            list_.seekp(mainNode.target);
            list_.write(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));

            // set the new data in the main node
            mainNode.key1 = key1;
            mainNode.key2 = key2;
            mainNode.value = value;
            ++(mainNode.count);
            list_.seekp(mainNodePtr);
            list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

        } else {
            // Rule out the case of the same node
            if (index == -1 && mainNode.key1 == key1 && mainNode.key2 == key2) {
                return false;
            }
            if (index != -1) {
                list_.seekg(mainNode.target + index * sizeof(Node_));
                list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
                if (tmpNode.key1 == key1 && tmpNode.key2 == key2) return false;
            }

            // Move the node(s) after the node to be inserted
            char* buffer = Read_(mainNode.target + (index + 1) * sizeof(Node_),
                                 (mainNode.count - index - 1) * sizeof(Node_));
            Write_(buffer, mainNode.target + (index + 2) * sizeof(Node_),
                   (mainNode.count - index - 1) * sizeof(Node_));
            delete[] buffer;

            // Put the new node
            Node_ newNode{key1, key2, value};
            list_.seekp(mainNode.target + (index + 1) * sizeof(Node_));
            list_.write(reinterpret_cast<char*>(&newNode), sizeof(Node_));

            // Change the main node
            ++(mainNode.count);
            list_.seekp(mainNodePtr);
            list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
        }

        // Split the main node if it is larger its expected size
        if (mainNode.count >= head_.maxNodeSize) {
            Split_(mainNode, mainNodePtr);
        }

        // put the new node into cache (store the data in memory to boost efficiency)
        cachedNode_.key1 = key1;
        cachedNode_.key2 = key2;
        cachedNode_.value = value;
        cached = true;

        return true;
    }

    /**
     * Erase the corresponding key-value pair of the input key.
     * @param key1
     * @param key2
     * @return whether the operation is successful or not
     */
    bool Erase(const KeyType1& key1, const KeyType2& key2) {
        // De-cache the node if it is really in cache
        if (cached && cachedNode_.key1 == key1 && cachedNode_.key2 == key2) {
            cached = false;
        }

        // Find the exact place of the node to be erased
        auto [mainNodePtr, index] = FindExact_(key1, key2);
        if (mainNodePtr == -1) return false; // no such node

        // Get the main node
        MainNode_ mainNode;
        Node_ tmpNode;
        list_.seekg(mainNodePtr);
        list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

        if (index == -1) { // the case that the data is in the main node
            if (mainNode.count == 0) { // the case that the main node has no other members
                DeleteNode_(mainNode, mainNodePtr);
            } else { // the case that the main node has other members
                // Set the main node
                list_.seekg(mainNode.target);
                list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
                mainNode.key1 = tmpNode.key1;
                mainNode.key2 = tmpNode.key2;
                mainNode.value = tmpNode.value;
                --(mainNode.count);

                // Put the main Node
                list_.seekp(mainNodePtr);
                list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

                // Move forward the other nodes
                char* buffer = Read_(mainNode.target + sizeof(Node_), mainNode.count * sizeof(Node_));
                Write_(buffer, mainNode.target, mainNode.count * sizeof(Node_));
                delete[] buffer;
            }
        } else { // the case that the data is in the array of the main node
            // Set and put the main node
            --(mainNode.count);
            list_.seekp(mainNodePtr);
            list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

            // Move forward the other nodes
            char* buffer = Read_(mainNode.target + (index + 1) * sizeof(Node_),
                                 (mainNode.count - index) * sizeof(Node_));
            Write_(buffer, mainNode.target + index * sizeof(Node_),
                   (mainNode.count - index) * sizeof(Node_));
            delete[] buffer;
        }

        return true;
    }

    /**
     * Modify the value of an existing key-value pair.
     * @param key1
     * @param key2
     * @param value
     * @return whether the operation is successful or not
     */
    bool Modify(const KeyType1& key1, const KeyType2& key2, const ValueType& value) {
        // Change cache the node if it is really in cache
        if (cached && cachedNode_.key1 == key1 && cachedNode_.key2 == key2) {
            cachedNode_.value = value;
        }

        // Find the Node
        auto [mainNodePtr, index] = FindExact_(key1, key2);
        if (mainNodePtr == -1) return false; // no such node

        // Get the main node
        MainNode_ mainNode;
        list_.seekg(mainNodePtr);
        list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

        if (index == -1) { // the case that the data is in the main node
            mainNode.value = value;
            list_.seekp(mainNodePtr);
            list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
        } else { // the case that the data is in the array of the main node
            Node_ tmpNode;
            list_.seekg(mainNode.target + index * sizeof(Node_));
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
            tmpNode.value = value; // Modify the value
            list_.seekp(mainNode.target + index * sizeof(Node_));
            list_.write(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
        }

        return true;
    }

    /**
     * Clear all the data in the unrolled linked list
     * @return reference of the current class
     */
    FileDoubleUnrolledLinkedList& Clear() {
        head_.nextGarbage = head_.next;
        head_.next = 0;
        head_.pre = 0;
        list_.seekp(0);
        list_.write(reinterpret_cast<char*>(&head_), sizeof(FirstNode_));
        cached = false;
        return *this;
    }

    /**
     * Tell whether there exists a node with a certain key or not.
     * @param key1
     * @param key2
     * @return The boolean of whether there exists a node with a certain key
     * or not
     */
    [[nodiscard]] bool Exist(const KeyType1& key1, const KeyType2& key2) {
        // Check if it is cached
        if (cached && cachedNode_.key1 == key1 && cachedNode_.key2 == key2) {
            return true;
        }

        auto [mainNodePtr, offset] = FindExact_(key1, key2);
        if (mainNodePtr == -1) return false; // no such node

        MainNode_ mainNode;
        list_.seekg(mainNodePtr);
        list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
        if (offset == -1) {
            cachedNode_.key1 = mainNode.key1;
            cachedNode_.key2 = mainNode.key2;
            cachedNode_.value = mainNode.value;
            cached = true;
            return true;

        } else {
            Node_ tmpNode;
            list_.seekg(mainNode.target + offset * sizeof(Node_));
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));

            cachedNode_.key1 = tmpNode.key1;
            cachedNode_.key2 = tmpNode.key2;
            cachedNode_.value = tmpNode.key;
            cached = true;
            return true;
        }
    }

    /**
     * Get the corresponding value of the key.  If there is no such node,
     * it will return the default value (using the default constructor).
     * @param key1
     * @param key2
     * @return the corresponding value of the key.
     * <br>
     * If there is no such node, it will return the default value (calling the
     * default constructor).
     */
    [[nodiscard]] ValueType Get(const KeyType1& key1, const KeyType2& key2) {
        // Check if it is cached
        if (cached && cachedNode_.key1 == key1 && cachedNode_.key2 == key2) {
            return cachedNode_.value;
        }

        auto [mainNodePtr, offset] = FindExact_(key1, key2);
        if (mainNodePtr == -1) return ValueType(); // no such node

        MainNode_ mainNode;
        list_.seekg(mainNodePtr);
        list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
        if (offset == -1) {
            cachedNode_.key1 = mainNode.key1;
            cachedNode_.key2 = mainNode.key2;
            cachedNode_.value = mainNode.value;
            cached = true;
            return cachedNode_.value;

        } else {
            Node_ tmpNode;
            list_.seekg(mainNode.target + offset * sizeof(Node_));
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));

            cachedNode_.key1 = tmpNode.key1;
            cachedNode_.key2 = tmpNode.key2;
            cachedNode_.value = tmpNode.value;
            cached = true;
            return cachedNode_.value;
        }
    }

    /**
     * Get the point of the value of a certain key pair.
     * If the key pair doesn't exist, a nullptr will be returned instead.
     * <br><br>
     * WARNING: If the function doesn't return a nullptr, ALWAYS free
     * the memory whenever you don't need it to avoid memory leak.
     * @param key1
     * @param key2
     * @return the point of the value of a certain key.
     * <br>
     * If there is no such node, a nullptr will be returned instead.
     */
    [[nodiscard]] ValueType* GetWithPointer(const KeyType1& key1, const KeyType2& key2) {
        // Check if it is cached
        if (cached && cachedNode_.key1 == key1 && cachedNode_.key2 == key2) {
            return new ValueType(cachedNode_.value);
        }

        auto [mainNodePtr, offset] = FindExact_(key1, key2);
        if (mainNodePtr == -1) return nullptr; // no such node

        MainNode_ mainNode;
        list_.seekg(mainNodePtr);
        list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
        if (offset == -1) {
            cachedNode_.key1 = mainNode.key1;
            cachedNode_.key2 = mainNode.key2;
            cachedNode_.value = mainNode.value;
            cached = true;
            return new ValueType(mainNode.value);
        } else {
            Node_ tmpNode;
            list_.seekg(mainNode.target + offset * sizeof(Node_));
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));

            cachedNode_.key1 = tmpNode.key1;
            cachedNode_.key2 = tmpNode.key2;
            cachedNode_.value = tmpNode.value;
            cached = true;
            return new ValueType(tmpNode.value);
        }
    }

    /**
     * Get all the key-value pair in the list.
     * @return the <code>std::vector</code> class containing all the
     * <code>Node</code>s in the list
     */
    std::vector<Node> Traverse() {
        std::vector<Node> values; // can be optimized
        MainNode_ mainNode;
        Node node;
        Ptr mainPtr = head_.next;
        while (mainPtr != 0) {
            list_.seekg(mainPtr);
            list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
            values.emplace_back(Node{mainNode.key1, mainNode.key2, mainNode.value});
            for (SizeT i = 0; i < mainNode.count; ++i) {
                list_.seekg(mainNode.target + i * sizeof(Node_));
                list_.read(reinterpret_cast<char*>(&node), sizeof(Node_));
                values.emplace_back(node);
            }
            mainPtr = mainNode.next;
        }
        return std::move(values);
    }

    /**
     * Get all the key-value pair with a certain key1.
     * @param key1
     * @return the <code>std::vector</code> class containing all the
     * <code>Node</code>s with key1
     */
    std::vector<Node> Traverse(const KeyType1& key1) {
        std::vector<Node> values; // can be optimized

        auto [mainNodePtr, offset] = SingleFind_(key1);

        // The case of there are no such key
        if (mainNodePtr == -1) {
            return std::move(values);
        }

        MainNode_ mainNode;
        Node node;
        list_.seekg(mainNodePtr);
        list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
        if (offset != -1) {
            // Traverse all the data in the array of the main node
            for (SizeT i = offset; i < mainNode.count; ++i) {
                list_.seekg(mainNode.target + i * sizeof(Node_));
                list_.read(reinterpret_cast<char*>(&node), sizeof(Node_));
                if (!(node.key1 == key1)) break;
                values.emplace_back(node);
            }

            // Move to the next node
            mainNodePtr = mainNode.next;
            list_.seekg(mainNodePtr);
            list_.read(reinterpret_cast<char*>(&mainNode), sizeof(mainNode));
        }
        while (mainNode.key1 == key1) {
            values.emplace_back(Node{mainNode.key1, mainNode.key2, mainNode.value});

            // Traverse all the data in the array of the main node
            for (SizeT i = 0; i < mainNode.count; ++i) {
                list_.seekg(mainNode.target + i * sizeof(Node_));
                list_.read(reinterpret_cast<char*>(&node), sizeof(Node_));
                if (!(node.key1 == key1)) break;
                values.emplace_back(node);
            }

            // Move to the next node
            if (mainNode.next == 0) break;
            mainNodePtr = mainNode.next;
            list_.seekg(mainNodePtr);
            list_.read(reinterpret_cast<char*>(&mainNode), sizeof(mainNode));
        }
        return std::move(values);
    }

    /**
     * Flush to make everything in the buffer stored in the file
     * @return reference of the current class
     */
    FileDoubleUnrolledLinkedList& Flush() {
        list_.flush();
        return *this;
    }

    /**
     * Tell whether the list is empty.
     * @return the boolean of whether the list is empty
     */
    bool Empty() {
        return (head_.next == 0);
    }

private:
    typedef Node Node_;

    /**
     * @struct FirstNode_{next, pre, nextGarbage, nodeSize, maxNodeSize}
     *
     * This is the node to pointer to the data, and metadata of the list
     */
    struct FirstNode_ {
        Ptr   next; // the first main node
        Ptr   pre; // the last main node
        Ptr   nextGarbage; // the first garbage node
        SizeT nodeSize;
        SizeT maxNodeSize;
    };

    /**
     * @struct MainNode_{key1, key2, value, target, count, next, pre}
     *
     * This is the node to store the pointer and data, and
     * link other main nodes.
     */
    struct MainNode_ {
        KeyType1  key1;
        KeyType2  key2;
        ValueType value;
        Ptr       target;
        SizeT     count;
        Ptr       next;
        Ptr       pre;
    };


    /**
     * This function return a pair of the pointer to the main node
     * and the index number of node the target is right after.  For the case
     * of the main node, the second member of the pair is -1.  If the
     * key doesn't belongs to the unrolled linked list, it will return the
     * node ahead of its place (except the case that the key is less than
     * any other keys, and in such case, it will return the first main node).
     * @param key
     * @return a pair of the pointer to the main node and the offset
     */
    std::pair<Ptr, SizeT> Find_(const KeyType1& key1, const KeyType2& key2) {
        if (head_.next == 0) return std::make_pair(0, -1);

        MainNode_ tmp;
        Ptr Ptr = head_.pre;
        list_.seekg(head_.pre);
        list_.read(reinterpret_cast<char*>(&tmp), sizeof(MainNode_));

        // Searching for the approximate place (only the main node)
        while ((key1 < tmp.key1 || (key1 == tmp.key1 && key2 < tmp.key2)) && tmp.pre != 0) {
            Ptr = tmp.pre;
            list_.seekg(Ptr);
            list_.read(reinterpret_cast<char*>(&tmp), sizeof(MainNode_));
        }

        // Searching for the exact place

        if (tmp.pre == 0 && (key1 < tmp.key1 || (key1 == tmp.key1 && key2 < tmp.key2))) return std::make_pair(Ptr, -1);

        if (key1 == tmp.key1 && key2 == tmp.key2 || tmp.count == 0) return std::make_pair(Ptr, -1);

        Node_ tmpNode;
        SizeT leftIndex = 0, rightIndex = tmp.count - 1;

        // the case that the key is between the main node and the first node
        list_.seekg(tmp.target);
        list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
        if (key1 < tmpNode.key1 || (key1 == tmpNode.key1 && key2 < tmpNode.key2)) {
            return std::make_pair(Ptr, -1);
        }

        // the case that the key is right after the last node
        list_.seekg(tmp.target + (tmp.count - 1) * sizeof(Node_));
        list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
        if (tmpNode.key1 < key1 || (tmpNode.key1 == key1 && (tmpNode.key2 < key2 || tmpNode.key2 == key2))) {
            return std::make_pair(Ptr, tmp.count - 1);
        }

        while (rightIndex - leftIndex > 1) {
            list_.seekg(tmp.target + ((rightIndex + leftIndex) / 2) * sizeof(Node_));
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
            if (key1 < tmpNode.key1 || (key1 == tmpNode.key1 && key2 < tmpNode.key2)) {
                rightIndex = (rightIndex + leftIndex) / 2;
            } else {
                leftIndex = (rightIndex + leftIndex) / 2;
            }
        }

        return std::make_pair(Ptr, leftIndex);
    }

    /**
     * This function return a pair of the pointer to the main node
     * and the number of node the target is right after, for the case
     * of the main node, the second member of the pair is -1.  If the
     * key doesn't belongs to the unrolled linked list, it will return
     * {-1, -1}.
     * @param key1
     * @param key2
     * @return a pair of the pointer to the main node and the offset
     */
    std::pair<Ptr, SizeT> FindExact_(const KeyType1& key1, const KeyType2& key2) {
        if (head_.next == 0) return std::make_pair(-1, -1);

        MainNode_ tmp;
        Ptr Ptr = head_.pre;
        list_.seekg(head_.pre);
        list_.read(reinterpret_cast<char*>(&tmp), sizeof(MainNode_));

        // Searching for the approximate place (only the main node)
        while ((key1 < tmp.key1 || (key1 == tmp.key1 && key2 < tmp.key2)) && tmp.pre != 0) {
            Ptr = tmp.pre;
            list_.seekg(Ptr);
            list_.read(reinterpret_cast<char*>(&tmp), sizeof(MainNode_));
        }

        // Searching for the exact place

        if (tmp.pre == 0 && (key1 < tmp.key1 || (key1 == tmp.key1 && key2 < tmp.key2))) return std::make_pair(-1, -1);

        if (key1 == tmp.key1 && key2 == tmp.key2) return std::make_pair(Ptr, -1);

        if (tmp.count == 0) return std::make_pair(-1, -1);

        Node_ tmpNode;
        SizeT leftIndex = 0, rightIndex = tmp.count - 1;

        // the case that the key is between the main node and the first node
        list_.seekg(tmp.target);
        list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
        if (key1 < tmpNode.key1 || (key1 == tmpNode.key1 && key2 < tmpNode.key2)) return std::make_pair(-1, -1);

        // the case that the key is right after the last node
        list_.seekg(tmp.target + (tmp.count - 1) * sizeof(Node_));
        list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
        if (tmpNode.key1 < key1 || (tmpNode.key1 == key1 && tmpNode.key2 < key2)) return std::make_pair(-1, -1);

        if (tmpNode.key1 == key1 && tmpNode.key2 == key2) return std::make_pair(Ptr, tmp.count - 1);

        while (rightIndex - leftIndex > 1) {
            list_.seekg(tmp.target + ((rightIndex + leftIndex) / 2) * sizeof(Node_));
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
            if (key1 < tmpNode.key1 || (key1 == tmpNode.key1 && key2 < tmpNode.key2)) {
                rightIndex = (rightIndex + leftIndex) / 2;
            } else {
                leftIndex = (rightIndex + leftIndex) / 2;
            }
        }

        list_.seekg(tmp.target + leftIndex * sizeof(Node_));
        list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
        if (tmpNode.key1 == key1 && tmpNode.key2 == key2) return std::make_pair(Ptr, leftIndex);
        else return std::make_pair(-1, -1);
    }

    /**
     * This function return a pair of the pointer to the main node
     * and the number of node the target is right after, for the case
     * of the main node, the second member of the pair is -1.  If the
     * key doesn't belongs to the unrolled linked list, it will return
     * {-1, -1}.
     * @param key1
     * @param key2
     * @return a pair of the pointer to the main node and the offset
     */
    std::pair<Ptr, SizeT> SingleFind_(const KeyType1& key1) {
        if (head_.next == 0) return std::make_pair(-1, -1);

        MainNode_ tmp;
        Ptr ptr = head_.pre;
        list_.seekg(head_.pre);
        list_.read(reinterpret_cast<char*>(&tmp), sizeof(MainNode_));
        bool exist = false;

        // Searching for the approximate place (only the main node)
        while ((key1 < tmp.key1 || key1 == tmp.key1) && tmp.pre != 0) {
            if (!exist && key1 == tmp.key1) exist = true;
            ptr = tmp.pre;
            list_.seekg(ptr);
            list_.read(reinterpret_cast<char*>(&tmp), sizeof(MainNode_));
        }
        if (exist) {
            while (key1 == tmp.key1 && tmp.pre != 0) {
                ptr = tmp.pre;
                list_.seekg(ptr);
                list_.read(reinterpret_cast<char*>(&tmp), sizeof(MainNode_));
            }
        }

        // Searching for the exact place

        if (tmp.pre == 0 && key1 < tmp.key1) return std::make_pair(-1, -1);

        if (tmp.pre == 0 && key1 == tmp.key1) return std::make_pair(ptr, -1);

        if (tmp.count == 0) {
            if (exist) return std::make_pair(tmp.next, -1);
            else return std::make_pair(-1, -1);
        }

        Node_ tmpNode;
        SizeT leftIndex = 0, rightIndex = tmp.count - 1;

        if (exist) {
            // for the last node
            list_.seekg(tmp.target + (tmp.count - 1) * sizeof(Node_));
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
            if (tmpNode.key1 < key1) return std::make_pair(tmp.next, -1);

            // for the first node
            list_.seekg(tmp.target);
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
            if (tmpNode.key1 == key1) return std::make_pair(ptr, 0);

            while (rightIndex - leftIndex > 1) {
                list_.seekg(tmp.target + ((rightIndex + leftIndex) / 2) * sizeof(Node_));
                list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
                if (key1 == tmpNode.key1) rightIndex = (rightIndex + leftIndex) / 2;
                else leftIndex = (rightIndex + leftIndex) / 2;
            }
            return std::make_pair(ptr, rightIndex);
        } else {
            // the case that the key is between the main node and the first node
            list_.seekg(tmp.target);
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
            if (key1 < tmpNode.key1) return std::make_pair(-1, -1);
            if (key1 == tmpNode.key1) return std::make_pair(ptr, 0);

            // the case that the key is right after the last node
            list_.seekg(tmp.target + (tmp.count - 1) * sizeof(Node_));
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
            if (tmpNode.key1 < key1) return std::make_pair(-1, -1);

            while (rightIndex - leftIndex > 1) {
                list_.seekg(tmp.target + ((rightIndex + leftIndex) / 2) * sizeof(Node_));
                list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
                if (tmpNode.key1 < key1) leftIndex = (rightIndex + leftIndex) / 2;
                else rightIndex = (rightIndex + leftIndex) / 2;
            }

            list_.seekg(tmp.target + rightIndex * sizeof(Node_));
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
            if (tmpNode.key1 == key1) return std::make_pair(ptr, rightIndex);
            else return std::make_pair(-1, -1);
        }
    }

    /**
     * This function delete a main node
     * @param mainNode the main node to be deleted
     * @param target the place where the main node to be deleted is
     */
    void DeleteNode_(MainNode_& mainNode, Ptr target) {
        MainNode_ pre, next;

        // Change the node to be deleted
        mainNode.pre = 0;
        mainNode.next = head_.nextGarbage;
        mainNode.count = 0;
        list_.seekp(target);
        list_.write(reinterpret_cast<const char*>(&mainNode), sizeof(MainNode_));

        // Change the first node
        head_.nextGarbage = target;


        // The case that the only main node is to be deleted
        if (mainNode.pre == 0 && mainNode.next == 0) {
            head_.pre = 0;
            head_.next = 0;
            list_.seekp(0);
            list_.write(reinterpret_cast<const char*>(&head_), sizeof(FirstNode_));
            return;
        }

        // The case that the main node is the first main Node
        if (mainNode.pre == 0) {
            head_.next = mainNode.next;
            list_.seekg(mainNode.next);
            list_.read(reinterpret_cast<char*>(&next), sizeof(MainNode_));
            next.pre = 0;
            list_.seekp(mainNode.next);
            list_.write(reinterpret_cast<const char*>(&next), sizeof(MainNode_));
            list_.seekp(0);
            list_.write(reinterpret_cast<const char*>(&head_), sizeof(FirstNode_));
            return;
        }

        // The case that the main node is the last main node
        if (mainNode.next == 0) {
            head_.pre = mainNode.pre;
            list_.seekp(mainNode.pre);
            list_.read(reinterpret_cast<char*>(&pre), sizeof(MainNode_));
            pre.next = 0;
            list_.seekp(mainNode.pre);
            list_.write(reinterpret_cast<const char*>(&pre), sizeof(MainNode_));
            list_.seekp(0);
            list_.write(reinterpret_cast<const char*>(&head_), sizeof(FirstNode_));
            return;
        }

        // The regular case
        list_.seekg(mainNode.pre);
        list_.read(reinterpret_cast<char*>(&pre), sizeof(MainNode_));
        list_.seekg(mainNode.next);
        list_.read(reinterpret_cast<char*>(&next), sizeof(MainNode_));
        pre.next = mainNode.next;
        next.pre = mainNode.pre;
        list_.seekp(mainNode.pre);
        list_.write(reinterpret_cast<const char*>(&pre), sizeof(MainNode_));
        list_.seekp(mainNode.next);
        list_.write(reinterpret_cast<const char*>(&next), sizeof(MainNode_));
        list_.seekp(0);
        list_.write(reinterpret_cast<const char*>(&head_), sizeof(FirstNode_));
    }

    /**
     * This function makes a new main node right after the target.
     * <br><br>
     * WARNING: If the target is 0, that means its a empty list.
     * @param mainNode the new main node to be added to the unrolled
     * linked list
     * @param target the place where the new node is added (it MUST
     * be a main node).
     * @return the new place of the new main node.
     */
    Ptr NewNode_(MainNode_& mainNode, Ptr target) {
        if (target != 0) { // for a not empty list
            MainNode_ pre, next;
            list_.seekg(target);
            list_.read(reinterpret_cast<char*>(&pre), sizeof(MainNode_));
            if (pre.next != 0) { // the case that the next main node is not the first node
                // Get the next
                list_.seekg(pre.next);
                list_.read(reinterpret_cast<char*>(&next), sizeof(MainNode_));

                // Set the previous and next node
                if (head_.nextGarbage == 0) {
                    list_.seekp(0, std::ios::end);
                    mainNode.next = pre.next;
                    mainNode.pre = next.pre;
                    pre.next = list_.tellp();
                    next.pre = list_.tellp();

                    // Put the new node
                    mainNode.target = pre.next + sizeof(MainNode_);
                    list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

                    // Get a new space for its array
                    list_.seekp(head_.maxNodeSize * sizeof(Node_), std::ios::end);
                    list_.write(reinterpret_cast<char*>(&emptyNode_), sizeof(Node_));

                } else {
                    MainNode_ tmpMainNode;
                    list_.seekg(head_.nextGarbage);
                    list_.read(reinterpret_cast<char*>(&tmpMainNode), sizeof(MainNode_));

                    mainNode.next = pre.next;
                    mainNode.pre = next.pre;
                    mainNode.target = tmpMainNode.target;
                    pre.next = head_.nextGarbage;
                    next.pre = head_.nextGarbage;
                    list_.seekp(head_.nextGarbage);
                    list_.write(reinterpret_cast<const char*>(&mainNode), sizeof(MainNode_));
                    head_.nextGarbage = tmpMainNode.next;
                    list_.seekp(0);
                    list_.write(reinterpret_cast<const char*>(&head_), sizeof(FirstNode_));
                }

                // Put back the previous and next main node
                list_.seekp(mainNode.next);
                list_.write(reinterpret_cast<char*>(&next), sizeof(MainNode_));
                list_.seekp(mainNode.pre);
                list_.write(reinterpret_cast<char*>(&pre), sizeof(MainNode_));
                return pre.next;
            } else { // the case that the next main node is the first node
                // Set the previous and next node
                if (head_.nextGarbage == 0) {
                    list_.seekp(0, std::ios::end);
                    mainNode.next = 0;
                    mainNode.pre = target;
                    pre.next = list_.tellp();
                    head_.pre = list_.tellp();

                    // Put the new node
                    mainNode.target = pre.next + sizeof(MainNode_);
                    list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

                    // Get a new space for its array
                    list_.seekp(head_.maxNodeSize * sizeof(Node_), std::ios::end);
                    list_.write(reinterpret_cast<char*>(&emptyNode_), sizeof(Node_));

                } else {
                    MainNode_ tmpMainNode;
                    list_.seekg(head_.nextGarbage);
                    list_.read(reinterpret_cast<char*>(&tmpMainNode), sizeof(MainNode_));

                    mainNode.next = 0;
                    mainNode.pre = target;
                    mainNode.target = tmpMainNode.target;
                    pre.next = head_.nextGarbage;
                    head_.pre = head_.nextGarbage;
                    list_.seekp(head_.nextGarbage);
                    list_.write(reinterpret_cast<const char*>(&mainNode), sizeof(MainNode_));
                    head_.nextGarbage = tmpMainNode.next;
                }

                // Put back the previous and next main node
                list_.seekp(mainNode.pre);
                list_.write(reinterpret_cast<char*>(&pre), sizeof(MainNode_));
                list_.seekp(0);
                list_.write(reinterpret_cast<char*>(&head_), sizeof(FirstNode_));
                return pre.next;
            }
        } else { // For an empty list
            if (head_.nextGarbage == 0) {
                list_.seekp(0, std::ios::end);
                head_.next = list_.tellp();
                head_.pre = list_.tellp();
                mainNode.target = head_.next + sizeof(MainNode_);
                mainNode.next = 0;
                mainNode.pre = 0;
                list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

                // to reserve the space for the array of the main node
                list_.seekp(head_.maxNodeSize * sizeof(Node_), std::ios::end);
                list_.write(reinterpret_cast<const char*>(&emptyNode_), sizeof(Node_));
            } else {
                MainNode_ tmpMainNode;
                list_.seekg(head_.nextGarbage);
                list_.read(reinterpret_cast<char*>(&tmpMainNode), sizeof(MainNode_));

                head_.next = head_.nextGarbage;
                head_.pre = head_.nextGarbage;
                mainNode.next = 0;
                mainNode.pre = 0;
                mainNode.target = tmpMainNode.target;
                list_.seekp(head_.nextGarbage);
                list_.write(reinterpret_cast<const char*>(&mainNode), sizeof(MainNode_));
                head_.nextGarbage = tmpMainNode.next;
            }
            list_.seekp(0);
            list_.write(reinterpret_cast<char*>(&head_), sizeof(FirstNode_));
            return head_.next;
        }
    }

    /**
     * This function split a main node into two.
     * @param mainNode
     * @param mainNodePtr
     * @return the ptr of the main node
     */
    Ptr Split_(MainNode_& mainNode, Ptr mainNodePtr) {
        // Copy the extra string of nodes
        auto* nodeBuffer = new Node_[mainNode.count - head_.nodeSize];
        for (SizeT i = 0; i < mainNode.count - head_.nodeSize; ++i) {
            list_.seekg(mainNode.target + sizeof(Node_) * (head_.nodeSize + i));
            list_.read(reinterpret_cast<char*>(&nodeBuffer[i]), sizeof(Node_));
        }

        // Create a new node
        Ptr newMainNodePtr;
        MainNode_ newMainNode{nodeBuffer[0].key1, nodeBuffer[0].key2, nodeBuffer[0].value,
                              0, mainNode.count - head_.nodeSize - 1, 0, 0};

        // Get a new space for the nodes
        newMainNodePtr = NewNode_(newMainNode, mainNodePtr);

        // Change the count of the original node
        list_.seekg(mainNodePtr);
        list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
        mainNode.count = head_.nodeSize;
        list_.seekp(mainNodePtr);
        list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

        // Get the new main node
        list_.seekg(newMainNodePtr);
        list_.read(reinterpret_cast<char*>(&newMainNode), sizeof(MainNode_));

        // write the extra string of nodes
        list_.seekp(newMainNode.target);
        for (SizeT i = 1; i <= newMainNode.count; ++i) {
            list_.write(reinterpret_cast<char*>(&nodeBuffer[i]), sizeof(Node_));
        }
        delete[] nodeBuffer;
        return newMainNodePtr;
    }

    /**
     * This function reads a string of stuff and returns a pointer to the string.
     * @param target the place to get the data
     * @param length
     * @return a pointer to the string
     */
    char* Read_(Ptr target, SizeT length) {
        char* buffer = new char[length];
        list_.seekg(target);
        list_.read(buffer, length);
        return buffer;
    }

    /**
     * This function writes a string of stuff.
     * @param source the source pointer
     * @param target the place to place the data
     * @param length
     */
    void Write_(char* source, Ptr target, SizeT length) {
        list_.seekp(target);
        list_.write(source, length);
    }

    FirstNode_    head_;
    std::fstream  list_;
    Node_         emptyNode_;
    mutable Node_ cachedNode_;
    mutable bool  cached = false;
};

}

#endif // LAU_CPP_LIB_LAU_FILE_DOUBLE_UNROLLED_LINKED_LIST_H
