// Lau CPP Library - to strengthen the ability of C++ STL
// Copyright (C) 2022 Lau Yee-Yu
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

/**
 * @file lau/sparse_table.h
 *
 * This is a external header file, including the code of sparse table in
 * Lau CPP Lib.
 */

#ifndef LAU_CPP_LIB_LAU_SPARSE_TABLE_H
#define LAU_CPP_LIB_LAU_SPARSE_TABLE_H

#include <cmath>
#include <functional>
#include <type_traits>

#include "exception.h"
#include "type_trait.h"

namespace lau {

/**
 * @class SparseTable
 *
 * This class creates a sparse table and calculate each value, making query
 * for multiple times more efficient.
 * <br>
 * Please note that the function used in this procedure must have the following
 * properties: (The Function below is just a mathematical motion. The
 * calculation only call the function with two input.)
 * <br>
 * 1. Function(a) = a;
 * <br>
 * 2. Function(a, b) = Function(b, a);
 * <br>
 * 3. Function(a, b, ...) = Function(Function(a, b), ...).
 * @tparam T the type of the input type and the return type
 */
template<class T>
class SparseTable {
public:
    SparseTable() noexcept = default;

    /**
     * Create and calculate the sparse table in [begin, end).  If the begin
     * is greater than end, then a <code>lau::InvalidArgument</code> will be
     * thrown.
     * <br><br>
     * Please note that <code>end - begin</code> must be available.
     * @tparam Iterator
     * @param begin
     * @param end
     * @param Function T(T, T)
     */
    template<class Iterator>
    SparseTable(Iterator begin, Iterator end, std::function<T(T, T)> Function)
        : size_(end - begin),
          totalLayer_(static_cast<SizeT>(log(size_) / log(2)) + 1),
          Function_(std::move(Function)) {
        if (size_ < 0) {
            throw InvalidArgument("Invalid Argument: The begin iterator is greater than the end iterator");
        }
        data_ = new T[size_ * totalLayer_];
        for (SizeT i = 0; i < size_; ++i) {
            data_[i] = *begin;
            ++begin;
        }
        for (int i = 1; i < totalLayer_; ++i) {
            for (int j = 0; j + (1 << i) <= size_; ++j) {
                data_[i * size_ + j] = Function_(data_[(i - 1) * size_ + j],
                                                 data_[(i - 1) * size_ + j + (1 << (i - 1))]);
            }
        }
    }

    /**
     * Calculate the sparse table in [begin, begin + size).
     * @tparam Iterator
     * @param begin
     * @param size
     * @param Function T(T, T)
     */
    template<class Iterator>
    SparseTable(Iterator begin, SizeT size, std::function<T(T, T)> Function)
        : size_(size),
          totalLayer_(static_cast<SizeT>(log(size_) / log(2)) + 1),
          Function_(std::move(Function)) {
        if (size_ < 0) throw InvalidArgument("Invalid Argument: The size is less than 0");
        data_ = new T[size_ * totalLayer_];
        for (SizeT i = 0; i < size_; ++i) {
            data_[i] = *begin;
            ++begin;
        }
        for (int i = 1; i < totalLayer_; ++i) {
            for (int j = 0; j + (1 << i) <= size_; ++j) {
                data_[i * size_ + j] = Function_(data_[(i - 1) * size_ + j],
                                                 data_[(i - 1) * size_ + j + (1 << (i - 1))]);
            }
        }
    }

    SparseTable(const SparseTable& obj) {
        if (&obj == this) return;
        this->Clear();
        this->size_ = obj.size_;
        this->Function_ = obj.Function_;
        this->totalLayer_ = obj.totalLayer_;
        data_ = new T[size_ * totalLayer_];
        for (int i = 0; i < totalLayer_; ++i) {
            for (int j = 0; j + (1 << i) <= size_; ++j) {
                data_[i * size_ + j] = obj.data_[i * size_ + j];
            }
        }
    }

    SparseTable(SparseTable&& obj) noexcept : size_(obj.size_),
                                              Function_(std::move(obj.Function_)),
                                              totalLayer_(obj.totalLayer_) {
        delete[] data_;
        data_ = obj.data_;
        obj.data_ = nullptr;
    }

    SparseTable& operator=(const SparseTable& obj) {
        if (&obj == this) return *this;
        this->Clear();
        this->size_ = obj.size_;
        this->Function_ = obj.Function_;
        this->totalLayer_ = obj.totalLayer_;
        data_ = new T[size_ * totalLayer_];
        for (int i = 0; i < totalLayer_; ++i) {
            for (int j = 0; j + (1 << i) <= size_; ++j) {
                data_[i * size_ + j] = obj.data_[i * size_ + j];
            }
        }
        return *this;
    }

    SparseTable& operator=(SparseTable&& obj) noexcept {
        delete[] data_;
        this->size_ = obj.size_;
        this->Function_ = std::move(obj.Function_);
        this->totalLayer_ = obj.totalLayer_;
        this->data_ = obj.data_;
        obj.data_ = nullptr;
        return *this;
    }

    ~SparseTable() {
        delete[] data_;
    }

    /**
     * Clear all the result that has been generated before. Please note that
     * this operation cannot clear the function data due to lack of such
     * interface.
     * @return a reference to the current class
     */
    SparseTable& Clear() {
        delete[] data_;
        data_ = nullptr;
        size_ = 0;
        totalLayer_ = 0;
        return *this;
    }

    [[nodiscard]] bool Empty() const noexcept { return (data_ == nullptr); }

    /**
     * Get the size of the origin data.
     */
    [[nodiscard]] SizeT Size() const noexcept { return size_; }

    /**
     * Query the sparse table.  If the index is logically incorrect, a
     * <code>lau::InvalidArgument</code> will be thrown.  If the table is still
     * empty, a <code>lau::EmptyContainer</code> will be thrown.
     * @param beginIndex
     * @param endIndex
     * @return
     */
    T Query(SizeT beginIndex, SizeT endIndex) {
        if (beginIndex >= endIndex) {
            throw InvalidArgument("Invalid Argument: The begin index is no less than end index");
        }
        if (beginIndex < 0 || beginIndex >= size_ || endIndex > size_) throw InvalidArgument();
        if (data_ == nullptr) throw EmptyContainer();
        int layer = static_cast<int>(log(endIndex - beginIndex) / log(2));
        return Function_(data_[layer * size_ + beginIndex],
                         data_[layer * size_ + endIndex - (1 << layer)]);
    }

    /**
     * Get a copy of the Function.  Please note that even if the class is
     * cleared, the function will not be cleared.
     * @return the value of the embedded function
     */
    [[nodiscard]] std::function<T(T, T)> GetFunction() const {
        return Function_;
    }

    /**
     * Create and calculate the sparse table.  If the argument is logically
     * incorrect, a <code>lau::InvalidArgument</code> will be thrown.  In this
     * operation, the table is clear anyway (including the case of throwing
     * an exception).
     * <br><br>
     * Please note that <code>end - begin</code> must be available.
     * @tparam Iterator
     * @param begin
     * @param end
     * @param Function
     * @return the value of the embedded function
     */
    template<class Iterator>
    SparseTable& Calculate(Iterator begin, Iterator end, std::function<T(T, T)> Function) {
        delete[] data_;
        data_ = nullptr;
        Function_ = Function;
        size_ = end - begin;
        totalLayer_ = static_cast<SizeT>(log(size_) / log(2)) + 1;
        if (size_ < 0) {
            throw InvalidArgument("Invalid Argument: The begin iterator is greater than the end iterator");
        }
        data_ = new T[size_ * totalLayer_];
        for (SizeT i = 0; i < size_; ++i) {
            data_[i] = *begin;
            ++begin;
        }
        for (int i = 1; i < totalLayer_; ++i) {
            for (int j = 0; j + (1 << i) <= size_; ++j) {
                data_[i * size_ + j] = Function_(data_[(i - 1) * size_ + j],
                                                 data_[(i - 1) * size_ + j + (1 << (i - 1))]);
            }
        }
    }

    /**
     * Create and calculate the sparse table.  If the argument is logically
     * incorrect, a <code>lau::InvalidArgument</code> will be thrown.  In this
     * operation, the table is clear anyway (including the case of throwing
     * an exception).
     * @tparam Iterator
     * @param begin
     * @param size
     * @param Function
     * @return the value of the embedded function
     */
    template<class Iterator>
    SparseTable& Calculate(Iterator begin, SizeT size, std::function<T(T, T)> Function) {
        delete[] data_;
        Function_ = Function;
        size_ = size;
        totalLayer_ = static_cast<SizeT>(log(size_) / log(2)) + 1;
        if (size_ < 0) throw InvalidArgument("Invalid Argument: The size is less than 0");
        data_ = new T[size_ * totalLayer_];
        for (SizeT i = 0; i < size_; ++i) {
            data_[i] = *begin;
            ++begin;
        }
        for (int i = 1; i < totalLayer_; ++i) {
            for (int j = 0; j + (1 << i) <= size_; ++j) {
                data_[i * size_ + j] = Function_(data_[(i - 1) * size_ + j],
                                                 data_[(i - 1) * size_ + j + (1 << (i - 1))]);
            }
        }
    }

private:
    std::function<T(T, T)> Function_;
    T* data_ = nullptr;
    SizeT size_ = 0;
    SizeT totalLayer_ = 0;
};

} // namespace lau

#endif //LAU_CPP_LIB_LAU_SPARSE_TABLE_H
