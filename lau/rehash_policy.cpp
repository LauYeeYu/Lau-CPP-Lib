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
 * @file lau/rehash_policy.cpp
 *
 * This is a internal source file, including the implementation of the
 * <code>lau::RehashPolicy</code> class.
 */

#include "rehash_policy.h"

#include "exception.h"
#include "type_traits.h"


lau::RehashPolicy::RehashPolicy(SizeT minimumSize) {
    ReserveAtLeast(minimumSize);
}

[[nodiscard]] SizeT lau::RehashPolicy::NextSize() noexcept {
    if (index_ == maxIndex_) return bucketSize_[index_];
    ++index_;
    return bucketSize_[index_];
}

[[nodiscard]] SizeT lau::RehashPolicy::PreviousSize() noexcept {
    if (index_ == 0) {
        return bucketSize_[index_];
    }
    --index_;
    return bucketSize_[index_];
}

SizeT lau::RehashPolicy::ReserveAtLeast(SizeT size) {
    if (size < 0) {
        throw lau::InvalidArgument("Invalid Argument: the size is negative");
    }
    if (size == 0) {
        index_ = 0;
        return bucketSize_[index_];
    }
    SizeT left = 0, right = maxIndex_;
    while (left < right) {
        SizeT mid = (left + right) / 2;
        if (bucketSize_[mid] >= size) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    index_ = left;
    return bucketSize_[index_];
}

void lau::RehashPolicy::SetSize(SizeT size) {
    if (size < 0) {
        throw lau::InvalidArgument("Invalid Argument: the size is negative");
    }
    if (size == 0) {
        index_ = 0;
        return;
    }
    SizeT left = 0, right = maxIndex_;
    while (left < right) {
        SizeT mid = (left + right) / 2;
        if (bucketSize_[mid] >= size) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    if (bucketSize_[left] != size) {
        throw lau::InvalidArgument("Invalid Argument: argument not matching any of the bucket size");
    }
    index_ = left;
}

[[nodiscard]] SizeT lau::RehashPolicy::GetSize() const noexcept {
    return bucketSize_[index_];
}
