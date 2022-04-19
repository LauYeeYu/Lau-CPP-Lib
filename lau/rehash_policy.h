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
 * @file lau/rehash_policy.h
 *
 * This is a external header file, including the rehash policy for hash
 * tables.
 */

#ifndef LAU_CPP_LIB_LAU_REHASH_POLICY_H
#define LAU_CPP_LIB_LAU_REHASH_POLICY_H

#include "type_traits.h"

namespace lau {

class RehashPolicy {
public:
    RehashPolicy() noexcept = default;
    explicit RehashPolicy(SizeT minimumSize);
    RehashPolicy(const RehashPolicy&) noexcept = default;

    RehashPolicy& operator=(const RehashPolicy&) noexcept = default;

    ~RehashPolicy() = default;

    /**
     * Move to the next bucket size.  Note that if the current size reaches
     * the maximum size, the size will be set to the maximum size.
     * @return the new bucket size
     */
    [[nodiscard]] SizeT NextSize() noexcept;

    /**
     * Move back to the previous bucket size.  Note that if the size reaches
     * the minimum size (namely, 0), the size will be set to the minimum size.
     * @return the new bucket size
     */
    [[nodiscard]] SizeT PreviousSize() noexcept;

    /**
     * Set the size to the minimum size that is larger than the given size.
     * If the size is negative, a <code>lau::InvalidArgument</code> will be
     * thrown.  Note that if the size is larger than the maximum size, the
     * size will be set to the maximum size.
     * @param size
     * @return the new bucket size
     */
    SizeT ReserveAtLeast(SizeT size);

    /**
     * Set the size to the input value.  If the size is negative, or the size
     * is not included in the table, a <code>lau::InvalidArgument</code> will
     * be thrown.  It is recommended to use this function only when you have
     * to fall back to the previous size.
     * @param size
     */
    void SetSize(SizeT size);

    /**
     * Get the current bucket size.
     * @return the current bucket size
     */
    [[nodiscard]] SizeT GetSize() const noexcept;

    [[nodiscard]] constexpr static SizeT MaxSize() noexcept {
        return bucketSize_[maxIndex_];
    }

private:
    constexpr static SizeT bucketSize_[32] = {0,
                                              127,
                                              257,
                                              521,
                                              1031,
                                              2053,
                                              4099,
                                              8209,
                                              16411,
                                              32771,
                                              65537,
                                              131101,
                                              262147,
                                              524309,
                                              1048583,
                                              2097169,
                                              4194319,
                                              8388617,
                                              19260817, // in memory of him
                                              33554467,
                                              67108879,
                                              134217757,
                                              268435459,
                                              536870923,
                                              1073741827,
                                              2147483659,
                                              4294967291,
                                              8589934583,
                                              17179869143,
                                              34359738337,
                                              68719476731,
                                              137438953481};
    constexpr static SizeT maxIndex_ = 31;

    SizeT index_ = 0;
};

}

#endif // LAU_CPP_LIB_LAU_REHASH_POLICY_H
