#ifndef LAU_CPP_LIB_LAU_VECTOR_H
#define LAU_CPP_LIB_LAU_VECTOR_H

#include <climits>
#include <cstddef>
#include <memory>

#include "exception.h"

namespace lau {

/**
 * @class Vector
 *
 * A container that store data in a linear order.  But different from
 * <code>std::vector</code>, this class only store the pointer(s) to data
 * together, making it easier and faster to write data and to change capacity.
 * @tparam T the value type in the vector
 * @tparam Allocator the memory allocator
 */
template<class T, class Allocator = std::allocator<T>>
class Vector {
public:
    using SizeT         = long;
    using AllocatorType = Allocator;

    class Iterator;
	class ConstIterator;

	class Iterator {
        friend ConstIterator;
        friend Vector<T>;

	public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // For instance, for an iterator, iterator::value_type is the type that the
        // iterator points to.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
		using difference_type   = std::ptrdiff_t;
		using value_type        = T;
		using pointer           = T*;
		using reference         = T&;
		using iterator_category = std::output_iterator_tag;

        using SizeT             = long;

        Iterator(T** objPtr, const Vector<T>* vectorPtr) : objPtr_(objPtr), vectorPtr_(vectorPtr) {}

		Iterator operator+(SizeT n) const { return Iterator(objPtr_ + n, vectorPtr_); }

		Iterator operator-(SizeT n) const { return Iterator(objPtr_ - n, vectorPtr_); }

		 /**
		  * Return the distance between two iterators.  If these two iterators
		  * point to different vectors, throw invalid_iterator.
		  * @param rhs
		  * @return the distance between two iterators
		  */
		difference_type operator-(const Iterator &rhs) const {
			if (this->vectorPtr_ != rhs.vectorPtr_) throw InvalidIterator();
            return this->objPtr_ - rhs.objPtr_;
		}

		Iterator& operator+=(SizeT n) {
			objPtr_ += n;
            return *this;
		}

		Iterator& operator-=(SizeT n) {
			objPtr_ -= n;
            return *this;
		}

		Iterator operator++(int) {
            Iterator tmp = *this;
            ++tmp;
            return tmp;
        }

		Iterator& operator++() {
            ++objPtr_;
            return *this;
        }

		Iterator operator--(int) {
            Iterator tmp = *this;
            --tmp;
            return tmp;
        }

		Iterator& operator--() {
            --objPtr_;
            return *this;
        }

		T& operator*() const { return **objPtr_; }
        T* operator->() const { return *objPtr_; }

		/**
		 * Check whether two iterators are same (pointing to the same memory
		 * address) or not.
		 */
		bool operator==(const Iterator &rhs) const { return (this->objPtr_ == rhs.objPtr_); }
		bool operator==(const ConstIterator &rhs) const { return (this->objPtr_ == rhs.objPtr_); }
		bool operator!=(const Iterator &rhs) const { return (this->objPtr_ != rhs.objPtr_); }
		bool operator!=(const ConstIterator &rhs) const { return (this->objPtr_ != rhs.objPtr_); }

    private:
        T** objPtr_;
        const Vector<T>* vectorPtr_;
	};

	class ConstIterator {
        friend Iterator;
        friend Vector<T>;

	public:
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a type.
        // For instance, for an iterator, iterator::value_type is the type that the
        // iterator points to.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About iterator_category: https://en.cppreference.com/w/cpp/iterator
		using difference_type   = std::ptrdiff_t;
		using value_type        = T;
		using pointer           = T*;
		using reference         = T&;
		using iterator_category = std::output_iterator_tag;

        using SizeT             = long;

        ConstIterator(T** objPtr, const Vector<T>* vectorPtr) : objPtr_(objPtr), vectorPtr_(vectorPtr) {}

        ConstIterator operator+(SizeT n) const { return ConstIterator(objPtr_ + n, vectorPtr_); }

        ConstIterator operator-(SizeT n) const { return ConstIterator(objPtr_ - n, vectorPtr_); }

        /**
		  * Return the distance between two iterators.  If these two iterators
		  * point to different vectors, throw invalid_iterator.
		  * @param rhs
		  * @return the distance between two iterators
		  */
        difference_type operator-(const ConstIterator &rhs) const {
            if (this->vectorPtr_ != rhs.vectorPtr_) throw InvalidIterator();
            return this->objPtr_ - rhs.objPtr_;
        }

        ConstIterator& operator+=(SizeT n) {
            objPtr_ += n;
            return *this;
        }

        ConstIterator& operator-=(SizeT n) {
            objPtr_ -= n;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            ++tmp;
            return tmp;
        }

        ConstIterator& operator++() {
            ++objPtr_;
            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator tmp = *this;
            --tmp;
            return tmp;
        }

        ConstIterator& operator--() {
            --objPtr_;
            return *this;
        }

        const T& operator*() const {
            return **objPtr_;
        }

        const T* operator->() const {
            return *objPtr_;
        }

        /**
         * Check whether two iterators are same (pointing to the same memory
         * address) or not.
         */
        bool operator==(const Iterator &rhs) const { return (this->objPtr_ == rhs.objPtr_); }
        bool operator==(const ConstIterator &rhs) const { return (this->objPtr_ == rhs.objPtr_); }
        bool operator!=(const Iterator &rhs) const { return (this->objPtr_ != rhs.objPtr_); }
        bool operator!=(const ConstIterator &rhs) const { return (this->objPtr_ != rhs.objPtr_); }

    private:
        T** objPtr_;
        const Vector<T>* vectorPtr_;
	};

	Vector() noexcept(noexcept(Allocator())) = default;

    explicit Vector(const Allocator& allocator) : allocator_(allocator) {}

    Vector(SizeT count, const T& value, const Allocator& allocator = Allocator())
        : allocator_(allocator) {
        this->Reserve(count);
        for (SizeT i = 0; i < count; ++i) {
            this->PushBack(value);
        }
    }

    explicit Vector(SizeT count, const Allocator& allocator = Allocator())
        : allocator_(allocator) {
        this->Reserve(count);
        for (SizeT i = 0; i < count; ++i) {
            this->PushBack(T());
        }
    }

    template<class InputIterator>
    Vector(const InputIterator& first,
           const InputIterator& last,
           const Allocator& allocator = Allocator())
        : allocator_(allocator) {
        this->Reserve(last - first);
        for (auto element = first; element != last; ++element) {
            this->PushBack(*element);
        }
    }

	Vector(const Vector& obj) : capacity_(obj.size_),
                                size_(obj.size_),
                                beginIndex_(0) {
        target_ = pointerAllocator_.allocate(capacity_);
        for (SizeT i = 0; i < size_; ++i) {
            target_[i] = allocator_.allocate(1);
            ::new (target_[i]) T(*(obj.target_[i + obj.beginIndex_]));
        }
    }

    Vector(Vector&& obj) noexcept : capacity_(obj.capacity_),
                                    size_(obj.size_),
                                    beginIndex_(obj.beginIndex_),
                                    target_(obj.target_) {
        obj.target_ = nullptr;
        obj.capacity_ = 0;
        obj.size_ = 0;
        obj.beginIndex_ = 0;
    }

    Vector(std::initializer_list<T> init,
           const Allocator& allocator = Allocator())
        : allocator_(allocator) {
        this->Reserve(init.size());
        for (const auto& element : init) {
            this->PushBack(element);
        }
    }


	Vector& operator=(const Vector& obj) {
        if (&obj == this) return *this;
        this->clear();
        capacity_ = obj.size_;
        size_ = obj.size_;
        beginIndex_ = 0;
        target_ = pointerAllocator_.allocate(capacity_);
        for (SizeT i = 0; i < size_; ++i) {
            target_[i] = allocator_.allocate(1);
            ::new (target_[i]) T(*(obj.target_[i + obj.beginIndex_]));
        }
        return *this;
    }

    Vector& operator=(Vector&& obj) noexcept {
        this->clear();
        capacity_ = obj.capacity_;
        size_ = obj.size_;
        beginIndex_ = obj.beginIndex_;
        target_ = obj.beginIndex_;
        obj.target_ = nullptr;
        obj.capacity_ = 0;
        obj.size_ = 0;
        obj.beginIndex_ = 0;
    }

    ~Vector() { this->clear(); }

	/**
	 * Access specified element with bounds checking.  If pos is not in range
	 * [0, size), an index_out_of_bound will be thrown.
	 * @param index
	 * @return a reference to the element at the input index
	 */
	T& At(SizeT index) {
        if (index >= size_ || index < 0) throw IndexOutOfBound();
        return *target_[index + beginIndex_];
    }

    /**
	 * Access specified element with bounds checking.  If pos is not in range
	 * [0, size), an index_out_of_bound will be thrown.
	 * @param index
	 * @return a const reference to the element at the input index
	 */
	const T& At(SizeT index) const {
        if (index >= size_ || index < 0) throw IndexOutOfBound();
        return *target_[index + beginIndex_];
    }

	/**
	 * Access specified element with bounds checking.  If pos is not in range
	 * [0, size), an index_out_of_bound will be thrown.
	 * @param index
	 * @return a reference to the element at the input index
	 */
	T& operator[](SizeT index) {
        if (index >= size_ || index < 0) throw IndexOutOfBound();
        return *target_[index + beginIndex_];
    }

    /**
     * assign specified element with bounds checking.  If pos is not in range
     * [0, size) , a index_out_of_bound will be thrown.
     * @param index
     * @return a const reference to the element at the input index
     */
	const T& operator[](SizeT index) const {
        if (index >= size_ || index < 0) throw IndexOutOfBound();
        return *target_[index + beginIndex_];
    }

	/**
	 * Access the first element.  If the container is empty, a
	 * container_is_empty will be thrown.
	 * @return a constant reference to the first element
	 */
	const T& Front() const {
        if (Empty()) throw EmptyContainer();
        return *target_[beginIndex_];
    }

	/**
	 * Access the last element.  If the container is empty, a
	 * container_is_empty will be thrown.
	 * @return a const reference to the last element
	 */
	const T& Back() const {
        if (Empty()) throw EmptyContainer();
        return *target_[size_ + beginIndex_ - 1];
    }

    /**
	 * Access the first element.  If the container is empty, a
	 * container_is_empty will be thrown.
	 * @return a reference to the first element
	 */
    T& Front() {
        if (Empty()) throw EmptyContainer();
        return *target_[beginIndex_];
    }

    /**
     * Access the last element.  If the container is empty, a
     * container_is_empty will be thrown.
     * @return a reference to the last element
     */
    T& Back() {
        if (Empty()) throw EmptyContainer();
        return *target_[size_ + beginIndex_ - 1];
    }

    Iterator Begin() { return Iterator(target_ + beginIndex_, this); }
    Iterator begin() { return Begin(); }

    ConstIterator ConstBegin() const { return ConstIterator(target_ + beginIndex_, this); }

    Iterator End() { return Iterator(target_ + beginIndex_ + size_, this); }
    Iterator end() { return End(); }

	ConstIterator ConstEnd() const { return ConstIterator(target_ + beginIndex_ + size_, this); }

	/**
	 * Check whether the container is empty.
	 */
    [[nodiscard]] bool Empty() const { return size_ == 0; }

	/**
	 * Return the number of elements.
	 */
	[[nodiscard]] SizeT Size() const { return size_; }

    /**
     * Clear the whole vector class.
     */
	void Clear() {
        for (SizeT i = 0; i < size_; ++i) {
            target_[i + beginIndex_]->~T();
            allocator_.deallocate(target_[i + beginIndex_], 1);
            target_[i + beginIndex_] = nullptr;
        }
        pointerAllocator_.deallocate(target_, capacity_);
        target_ = nullptr;
        capacity_ = 0;
        size_ = 0;
        beginIndex_ = 0;
    }

	/**
	 * Insert value before position.
	 * @return an iterator pointing to the inserted value
	 */
	Iterator Insert(const Iterator& position, const T& value) {
        SizeT index = position - this->begin();
        return Insert(index, value);
    }

	/**
	 * Insert value at index.  If <code>index > size</code>, an
	 * index_out_of_bound will be thrown.  After this operation,
	 * <code>this->at(ind)</code> will be <code>value</code>.
	 * @return an iterator pointing to the inserted value
	 */
	Iterator Insert(SizeT index, const T& value) {
        if (index > size_) throw IndexOutOfBound();
        if (index == 0) {
            PushFront(value);
            return Begin();
        }

        if (NeedEnlarging_()) Enlarge_();
        for (SizeT i = size_; i > index; --i) {
            target_[i + beginIndex_] = target_[i + beginIndex_ - 1];
        }
        target_[index + beginIndex_] = allocator_.allocate(1);
        ::new (target_[index + beginIndex_]) T(value);
        ++size_;
        return Iterator(target_ + beginIndex_ + index, this);
    }

	/**
	 * Erase the element at pos.  If the iterator pos refers the last
	 * element, the <code>End()</code> iterator is returned.
	 * @return an iterator pointing to the following element
	 */
	Iterator Erase(const Iterator& position) {
        SizeT index = position - this->begin();
        return Erase(index);
    }

	/**
	 * Erase the element at index.  If <code>index >= size</code>, an
	 * index_out_of_bound will be thrown.
	 * @return an iterator pointing to the following element
	 */
	Iterator Erase(SizeT index) {
        if (index >= size_) throw IndexOutOfBound();
        --size_;
        if (index == 0) {
            target_[beginIndex_]->~T();
            allocator_.deallocate(target_[beginIndex_], 1);
            target_[beginIndex_] = nullptr;
            ++beginIndex_;
        } else {
            target_[beginIndex_ + index]->~T();
            allocator_.deallocate(target_[beginIndex_ + index], 1);
            for (SizeT i = index; i < size_; ++i) {
                target_[i + beginIndex_] = target_[i + beginIndex_ + 1];
            }
        }
        if (size_ == 0) beginIndex_ = 0;
        return Iterator(target_ + beginIndex_ + index, this);
    }

	/**
	 * Add an element to the end.
	 * @param value
     * @return a reference to the current class
	 */
    Vector& PushBack(const T& value) {
        if (NeedEnlarging_()) Enlarge_();
        target_[size_ + beginIndex_] = allocator_.allocate(1);
        ::new (target_[size_ + beginIndex_]) T(value);
        ++size_;
        return *this;
    }

    /**
	 * Add an element to the beginning.
	 * @param value
     * @return a reference to the current class
	 */
    Vector& PushFront(const T& value) {
        if (beginIndex_ > 0) {
            --beginIndex_;
            target_[beginIndex_] = allocator_.allocate(1);
            ::new (target_[beginIndex_]) T(value);
            ++size_;
        } else {
            if (NeedEnlarging_()) Enlarge_();
            for (SizeT i = size_; i > 0; --i) {
                target_[i] = target_[i - 1];
            }
            target_[0] = allocator_.allocate(1);
            ::new (target_[0]) T(value);
            ++size_;
        }
        return *this;
    }

	/**
	 * Remove the last element from the end.  If <code>size() == 0</code>, a
	 * container_is_empty will be thrown.
     * @return a reference to the current class
	 */
    Vector& PopBack() {
        if (size_ == 0) throw EmptyContainer();
        target_[size_ + beginIndex_ - 1]->~T();
        allocator_.deallocate(target_[size_ + beginIndex_ - 1], 1);
        --size_;
        if (size_ == 0) beginIndex_ = 0;
        return *this;
    }

    /**
     * Remove the first element from the beginning.  If
     * <code>size() == 0</code>, a container_is_empty will be thrown.
     * @return a reference to the current class
     */
    Vector& PopFront() {
        if (size_ == 0) throw EmptyContainer();
        --size_;
        target_[beginIndex_]->~T();
        allocator_.deallocate(target_[beginIndex_], 1);
        target_[beginIndex_] = nullptr;
        ++beginIndex_;
        if (size_ == 0) beginIndex_ = 0;
        return *this;
    }

    /**
     * Reserve enough space of newCapacity.
     * @param newCapacity
     * @return a reference to the current class
     */
    Vector& Reserve(SizeT newCapacity) {
        if (newCapacity <= capacity_ - beginIndex_) return *this;

        T** tmp = target_;
        target_ = pointerAllocator_.allocate(newCapacity);
        for (SizeT i = 0; i < size_; ++i) target_[i] = tmp[i + beginIndex_];
        for (SizeT i = size_; i < capacity_; ++i) target_[i] = nullptr;
        beginIndex_ = 0;
        pointerAllocator_.deallocate(tmp, capacity_);
        capacity_ = newCapacity;
        return *this;
    }

    /**
     * Swap two vector
     * @param other
     * @return a reference to the current class
     */
    Vector& Swap(Vector& other) {
        SizeT tmp;

        tmp = other.size_;
        other.size_ = this->size_;
        this->size_ = tmp;

        tmp = other.capacity_;
        other.capacity_ = this->capacity_;
        this->capacity_ = tmp;

        tmp = other.beginIndex_;
        other.beginIndex_ = this->beginIndex_;
        this->beginIndex_ = tmp;

        T** tmpTarget = other.target_;
        other.target_ = this->target_;
        this->target_ = tmpTarget;

        return *this;
    }

    /**
     * Reduce the capacity to its size.
     * @return a reference to the current class
     */
    Vector& ShrinkToFit() {
        if (size_ == capacity_) return *this;
        T** tmp = target_;
        target_ = pointerAllocator_.allocate(size_);
        for (SizeT i = 0; i < size_; ++i) {
            target_[i] = tmp[i + beginIndex_];
        }
        pointerAllocator_.deallocate(tmp, capacity_);
        capacity_ = size_;
        beginIndex_ = 0;
        return *this;
    }

    /**
     * Resize the container to contain count elements.  If the current size
     * is greater than count, the container is reduced to its first count
     * elements.  If the current size is less than count, additional
     * default-inserted elements are appended.
     * @param count
     * @return a reference to the current class
     */
    Vector& Resize(SizeT count) {
        if (size_ > count) {
            for (SizeT i = count; i < size_; ++i) {
                target_[i + beginIndex_]->~T();
                allocator_.deallocate(target_[i + beginIndex_], 1);
            }
        } else {
            Reserve(count);
            for (SizeT i = size_; i < count; ++i) {
                target_[i + beginIndex_] = allocator_.allocate(1);
                ::new (target_[i + beginIndex_]) T();
            }
        }
        size_ = count;
        return *this;
    }

    /**
     * Resize the container to contain count elements.  If the current size
     * is greater than count, the container is reduced to its first count
     * elements.  If the current size is less than count, additional
     * default-inserted elements are appended.
     * @param count
     * @return a reference to the current class
     */
    Vector& Resize(SizeT count, const T& value) {
        if (size_ > count) {
            for (SizeT i = count; i < size_; ++i) {
                target_[i + beginIndex_]->~T();
                allocator_.deallocate(target_[i + beginIndex_], 1);
            }
        } else {
            Reserve(count);
            for (SizeT i = size_; i < count; ++i) {
                target_[i + beginIndex_] = allocator_.allocate(1);
                ::new (target_[i + beginIndex_]) T(value);
            }
        }
        size_ = count;
        return *this;
    }

    /**
     * Get the capacity of the vector.  Please note that since its vector can
     * be popped from the front, so the maximum of the vector might be less
     * than the value.
     * @return the capacity of the vector
     */
    [[nodiscard]] SizeT Capacity() { return capacity_; }

    /**
     * Get a copy of the allocator.
     * @return a copy of the allocator
     */
    [[nodiscard]] AllocatorType GetAllocator() { return allocator_; }

    /**
     * Get the maximum size of the vector.
     * @return the maximum size of the vector
     */
    [[nodiscard]] long MaxSize() {
        return std::allocator_traits<Allocator>::max_size(allocator_);
    }

private:
    typename std::allocator_traits<Allocator>::template rebind_alloc<T*> pointerAllocator_;

    T**       target_     = nullptr;
    SizeT     size_       = 0;
    SizeT     capacity_   = 0;
    SizeT     beginIndex_ = 0;
    Allocator allocator_;

    /**
     * Enlarge the vector for the case that the vector is right at its biggest
     * capacity.
     */
    void Enlarge_() {
        if (capacity_ == 0) {
            Reserve(4);
        } else {
            Reserve(capacity_ * 2);
        }
    }

    /// Check whether a vector needs enlarging.
    bool NeedEnlarging_() { return (capacity_ == beginIndex_ + size_); }

};

/**
 * Swap the content of two vectors of one certain type.
 * @tparam T the type of value in vector
 * @param vector1
 * @param vector2
 */
template<class T>
void Swap(Vector<T>& vector1, Vector<T>& vector2) { vector1.Swap(vector2); }

} // namespace lau

#endif // LAU_CPP_LIB_LAU_VECTOR_H
