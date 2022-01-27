#ifndef LAU_CPP_LIB_LAU_FILE_DOUBLE_UNROLLED_LINKED_LIST_H
#define LAU_CPP_LIB_LAU_FILE_DOUBLE_UNROLLED_LINKED_LIST_H

#include <fstream>
#include <vector>

namespace lau {

/**
 * @class DoubleUnrolledLinkedList
 *
 * This is a template class of double unrolled linked list on disk storage.
 *
 * @tparam keyType1 Type of First key
 * @tparam keyType2 Type of Second key
 * @tparam valueType Type of Value
 *
 * WARNING:
 * <br>
 * 1. The key type MUST have valid operator< and operator== !
 * <br>
 * 2. The two key and value types must have default constructor and
 * trivial move and copy constructor.
 */
template<class keyType1, class keyType2, class valueType>
class FileDoubleUnrolledLinkedList {
private:
    typedef long Ptr;

    std::fstream list_;

    /// The following are private components of this linked list

    /**
     * @struct FirstNode_{next, pre, nodeSize, maxNodeSize}
     *
     * This is the node to pointer to the data, and metadata of the list
     */
    struct FirstNode_ {
        Ptr next; // the first main node

        Ptr pre; // the last main node

        int nodeSize;

        int maxNodeSize;
    } head_;

    /**
     * @struct MainNode_{key1, key2, value, target, count, next, pre}
     *
     * This is the node to store the pointer and data, and
     * link other main nodes.
     */
    struct MainNode_ {
        keyType1 key1;

        keyType2 key2;

        valueType value;

        Ptr target;

        int count;

        Ptr next;

        Ptr pre;
    };

    /**
     * @struct Node_{key1, key2, value}
     *
     * This is the Node to store data.
     */
    struct Node_ {
        keyType1 key1;

        keyType2 key2;

        valueType value;
    } emptyNode_;

    /**
     * This function return a pair of the pointer to the main node
     * and the number of node the target is right after, for the case
     * of the main node, the second member of the pair is -1.  If the
     * key doesn't belongs to the unrolled linked list, it will the
     * node ahead of its place (except the case that the key is less than
     * any other keys, and in such case, it will return the first main node).
     * @param key
     * @return a pair of the pointer to the main node and the offset
     */
    std::pair<Ptr, int> Find_(const keyType1& key1, const keyType2& key2) {
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
        int leftIndex = 0, rightIndex = tmp.count - 1;

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
    std::pair<Ptr, int> FindExact_(const keyType1& key1, const keyType2& key2) {
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
        int leftIndex = 0, rightIndex = tmp.count - 1;

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
    std::pair<Ptr, int> SingleFind_(const keyType1& key1) {
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
        int leftIndex = 0, rightIndex = tmp.count - 1;

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

        // The case that the only main node is to be deleted
        if (mainNode.pre == 0 && mainNode.next == 0) {
            head_.pre = 0;
            head_.next = 0;
            list_.seekp(0);
            list_.write(reinterpret_cast<char*>(&head_), sizeof(FirstNode_));
            return;
        }

        // The case that the main node is the first main Node
        if (mainNode.pre == 0) {
            head_.next = mainNode.next;
            list_.seekg(mainNode.next);
            list_.read(reinterpret_cast<char*>(&next), sizeof(MainNode_));
            next.pre = 0;
            list_.seekp(mainNode.next);
            list_.write(reinterpret_cast<char*>(&next), sizeof(MainNode_));
            list_.seekp(0);
            list_.write(reinterpret_cast<char*>(&head_), sizeof(FirstNode_));
            return;
        }

        // The case that the main node is the last main node
        if (mainNode.next == 0) {
            head_.pre = mainNode.pre;
            list_.seekp(mainNode.pre);
            list_.read(reinterpret_cast<char*>(&pre), sizeof(MainNode_));
            pre.next = 0;
            list_.seekp(mainNode.pre);
            list_.write(reinterpret_cast<char*>(&pre), sizeof(MainNode_));
            list_.seekp(0);
            list_.write(reinterpret_cast<char*>(&head_), sizeof(FirstNode_));
            return;
        }

        // The regular case
        list_.seekp(mainNode.pre);
        list_.read(reinterpret_cast<char*>(&pre), sizeof(MainNode_));
        list_.seekg(mainNode.next);
        list_.read(reinterpret_cast<char*>(&next), sizeof(MainNode_));
        pre.next = mainNode.next;
        next.pre = mainNode.pre;
        list_.seekp(mainNode.pre);
        list_.write(reinterpret_cast<char*>(&pre), sizeof(MainNode_));
        list_.seekp(mainNode.next);
        list_.write(reinterpret_cast<char*>(&next), sizeof(MainNode_));
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
        if (target != 0) { // the case that the list isn't empty
            // Get the previous main node
            MainNode_ pre, next;
            list_.seekg(target);
            list_.read(reinterpret_cast<char*>(&pre), sizeof(MainNode_));

            if (pre.next != 0) { // the node isn't the last main node
                // Get the next main node
                list_.seekg(pre.next);
                list_.read(reinterpret_cast<char*>(&next), sizeof(MainNode_));

                // Change the data of both the previous and next node
                list_.seekp(0, std::ios::end);
                mainNode.next = pre.next;
                mainNode.pre = next.pre;
                pre.next = list_.tellp();
                next.pre = list_.tellp();

                // Set the new main node
                mainNode.target = pre.next + sizeof(MainNode_);
                list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

                // Set a new space
                list_.seekp(head_.maxNodeSize * sizeof(Node_), std::ios::end);
                list_.write(reinterpret_cast<char*>(&emptyNode_), sizeof(Node_));

                // Put back both the previous and next node
                list_.seekp(mainNode.next);
                list_.write(reinterpret_cast<char*>(&next), sizeof(MainNode_));
                list_.seekp(mainNode.pre);
                list_.write(reinterpret_cast<char*>(&pre), sizeof(MainNode_));
                return pre.next;
            } else {
                // Change the data of both the previous and next node
                list_.seekp(0, std::ios::end);
                mainNode.next = 0;
                mainNode.pre = target;
                pre.next = list_.tellp();
                head_.pre = list_.tellp();

                // Set the new main node
                mainNode.target = pre.next + sizeof(MainNode_);
                list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

                // Set a new space
                list_.seekp(head_.maxNodeSize * sizeof(Node_), std::ios::end);
                list_.write(reinterpret_cast<char*>(&emptyNode_), sizeof(Node_));

                // Put back both the previous and next node
                list_.seekp(mainNode.pre);
                list_.write(reinterpret_cast<char*>(&pre), sizeof(MainNode_));
                list_.seekp(0);
                list_.write(reinterpret_cast<char*>(&head_), sizeof(FirstNode_));
                return pre.next;
            }
        } else { // the case of an empty list
            list_.seekp(0, std::ios::end);
            head_.next = list_.tellp();
            head_.pre = list_.tellp();
            mainNode.target = head_.next + sizeof(MainNode_);
            mainNode.next = 0;
            mainNode.pre = 0;
            list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

            // to reserve the space for the array of the main node
            list_.seekp(head_.maxNodeSize * sizeof(Node_), std::ios::end);
            list_.write(reinterpret_cast<char*>(&emptyNode_), sizeof(Node_));
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
        for (int i = 0; i < mainNode.count - head_.nodeSize; ++i) {
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
        for (int i = 1; i <= newMainNode.count; ++i) {
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
    char* Read_(Ptr target, int length) {
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
    void Write_(char* source, Ptr target, int length) {
        list_.seekp(target);
        list_.write(source, length);
    }

public:
    explicit FileDoubleUnrolledLinkedList(const std::string& fileName, int nodeSize = 316)
        : list_(fileName), head_{0, 0, nodeSize, 2 * nodeSize} {
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

    ~FileDoubleUnrolledLinkedList() = default;

    /**
     * This function inserts a new key-value pair.
     * <br><br>
     * WARNING: the new node to be inserted CANNOT be the same of the
     * node in the unrolled linked list, or it may cause severe and
     * unexpected problem.
     * @param key1 the new key1
     * @param key2 the new key2
     * @param value the value of the new key
     */
    void Insert(const keyType1& key1, const keyType2& key2, const valueType& value) {
        std::pair<Ptr, int> position = Find_(key1, key2);
        if (position.first == 0) {
            MainNode_ mainNode{key1, key2, value, 0, 0, 0, 0};
            NewNode_(mainNode, 0);
            return;
        }

        // Get the main node
        MainNode_ mainNode; // the place to place the new node
        list_.seekg(position.first);
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
            list_.seekp(position.first);
            list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
            return;
        } else {
            // Move the node(s) after the node to be inserted
            char* buffer = Read_(mainNode.target + (position.second + 1) * sizeof(Node_),
                                 (mainNode.count - position.second - 1) * sizeof(Node_));
            Write_(buffer, mainNode.target + (position.second + 2) * sizeof(Node_),
                   (mainNode.count - position.second - 1) * sizeof(Node_));
            delete[] buffer;

            // Put the new node
            Node_ newNode{key1, key2, value};
            list_.seekp(mainNode.target + (position.second + 1) * sizeof(Node_));
            list_.write(reinterpret_cast<char*>(&newNode), sizeof(Node_));

            // Change the main node
            ++(mainNode.count);
            list_.seekp(position.first);
            list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
        }

        // Split the main node if it is larger its expected size
        if (mainNode.count >= head_.maxNodeSize) {
            Split_(mainNode, position.first);
        }
    }

    void Erase(const keyType1& key1, const keyType2& key2) {
        std::pair<Ptr, int> position = FindExact_(key1, key2);
        if (position.first == -1) return; // no such node

        // Get the main node
        MainNode_ mainNode;
        Node_ tmpNode;
        list_.seekg(position.first);
        list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

        if (position.second == -1) { // the case that the data is in the main node
            if (mainNode.count == 0) { // the case that the main node has no other members
                DeleteNode_(mainNode, position.first);
            } else { // the case that the main node has other members
                // Set the main node
                list_.seekg(mainNode.target);
                list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
                mainNode.key1 = tmpNode.key1;
                mainNode.key2 = tmpNode.key2;
                mainNode.value = tmpNode.value;
                --(mainNode.count);

                // Put the main Node
                list_.seekp(position.first);
                list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

                // Move forward the other nodes
                char* buffer = Read_(mainNode.target + sizeof(Node_), mainNode.count * sizeof(Node_));
                Write_(buffer, mainNode.target, mainNode.count * sizeof(Node_));
                delete[] buffer;
            }
        } else { // the case that the data is in the array of the main node
            // Set and put the main node
            --(mainNode.count);
            list_.seekp(position.first);
            list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

            // Move forward the other nodes
            char* buffer = Read_(mainNode.target + (position.second + 1) * sizeof(Node_),
                                 (mainNode.count - position.second) * sizeof(Node_));
            Write_(buffer, mainNode.target + position.second * sizeof(Node_),
                   (mainNode.count - position.second) * sizeof(Node_));
            delete[] buffer;
        }
    }

    void Modify(const keyType1& key1, const keyType2& key2, const valueType& value) {
        std::pair<Ptr, int> position = FindExact_(key1, key2);
        if (position.first == -1) return; // no such node\

        // Get the main node
        MainNode_ mainNode;
        list_.seekg(position.first);
        list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));

        if (position.second == -1) { // the case that the data is in the main node
            mainNode.value = value;
            list_.seekp(position.first);
            list_.write(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
        } else { // the case that the data is in the array of the main node
            Node_ tmpNode;
            list_.seekg(mainNode.target + position.second * sizeof(Node_));
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
            tmpNode.value = value; // Modify the value
            list_.seekp(mainNode.target + position.second * sizeof(Node_));
            list_.write(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
        }
    }

    /**
     * The function clears all the data in the unrolled linked list
     */
    void Clear() {
        head_.next = 0;
        head_.pre = 0;
        list_.seekp(0);
        list_.write(reinterpret_cast<char*>(&head_), sizeof(FirstNode_));
    }

    /**
     * This function gets the point of the value of a certain key pair.
     * If the key pair doesn't exist, a nullptr will be returned instead.
     * <br><br>
     * WARNING: If the function doesn't return a nullptr, ALWAYS free
     * the memory whenever you don't need it.
     * @param key1
     * @param key2
     * @return the point of the value of a certain key.
     * <br>
     * If the key pair doesn't exist, a nullptr will be returned instead.
     */
    valueType* Get(const keyType1& key1, const keyType2& key2) {
        std::pair<Ptr, int> position = FindExact_(key1, key2);
        if (position.first == -1) return nullptr; // no such node

        MainNode_ mainNode;
        list_.seekg(position.first);
        list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
        if (position.second == -1) {
            return new valueType(mainNode.value);
        } else {
            Node_ tmpNode;
            list_.seekg(mainNode.target + position.second * sizeof(Node_));
            list_.read(reinterpret_cast<char*>(&tmpNode), sizeof(Node_));
            return new valueType(tmpNode.value);
        }
    }

    std::vector<valueType> Traverse() {
        std::vector<valueType> values; // can be optimized
        MainNode_ mainNode;
        Node_ node;
        Ptr mainPtr = head_.next;
        while (mainPtr != 0) {
            list_.seekg(mainPtr);
            list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
            values.emplace_back(mainNode.value);
            for (int i = 0; i < mainNode.count; ++i) {
                list_.seekg(mainNode.target + i * sizeof(Node_));
                list_.read(reinterpret_cast<char*>(&node), sizeof(Node_));
                values.emplace_back(node.value);
            }
            mainPtr = mainNode.next;
        }
        return std::move(values);
    }

    std::vector<valueType> Traverse(const keyType1& key1) {
        std::vector<valueType> values; // can be optimized

        std::pair<Ptr, int> position = SingleFind_(key1);

        // The case of there are no such key
        if (position.first == -1) {
            return std::move(values);
        }

        Ptr ptr = position.first;
        MainNode_ mainNode;
        Node_ node;
        list_.seekg(ptr);
        list_.read(reinterpret_cast<char*>(&mainNode), sizeof(MainNode_));
        if (position.second != -1) {
            // Traverse all the data in the array of the main node
            for (int i = position.second; i < mainNode.count; ++i) {
                list_.seekg(mainNode.target + i * sizeof(Node_));
                list_.read(reinterpret_cast<char*>(&node), sizeof(Node_));
                if (!(node.key1 == key1)) break;
                values.emplace_back(node.value);
            }

            // Move to the next node
            ptr = mainNode.next;
            list_.seekg(ptr);
            list_.read(reinterpret_cast<char*>(&mainNode), sizeof(mainNode));
        }
        while (mainNode.key1 == key1) {
            values.emplace_back(mainNode.value);

            // Traverse all the data in the array of the main node
            for (int i = 0; i < mainNode.count; ++i) {
                list_.seekg(mainNode.target + i * sizeof(Node_));
                list_.read(reinterpret_cast<char*>(&node), sizeof(Node_));
                if (!(node.key1 == key1)) break;
                values.emplace_back(node.value);
            }

            // Move to the next node
            ptr = mainNode.next;
            list_.seekg(ptr);
            list_.read(reinterpret_cast<char*>(&mainNode), sizeof(mainNode));
        }
        return std::move(values);
    }

    void Flush() {
        list_.flush();
    }
};

}

#endif // LAU_CPP_LIB_LAU_FILE_DOUBLE_UNROLLED_LINKED_LIST_H
