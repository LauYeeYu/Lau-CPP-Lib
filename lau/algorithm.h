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
 * @file lau/algorithm.h
 *
 * This is a external header file, including all the algorithms in Lau CPP Lib.
 */

#ifndef LAU_CPP_LIB_LAU_ALGORITHM_H
#define LAU_CPP_LIB_LAU_ALGORITHM_H

#include <cstring>
#include <memory>

#include "type_trait.h"

namespace lau {

/// The following are code about Knuth-Morris-Pratt algorithm.

/**
 * Calculate the partial match table for Knuth-Morris-Pratt algorithm.
 * @tparam T the type that need matching
 * @tparam Container the container type to contain the table
 * @param table the container to contain the table
 * @param target the target string
 * @param size size of the string
 * @return the container
 */
template<class T, class Container>
Container& PartialMatchTable(T target, SizeT size, Container& table) {
    table[0] = -1;
    SizeT clue = -1;
    SizeT position = 0;
    while (position < size - 1) {
        if (clue == -1 || target[position] == target[clue]) {
            ++position;
            ++clue;
            table[position] = clue;
        }
        else {
            clue = table[clue];
        }
    }
    return table;
}

/**
 * Find the index where a target string matches the source string with a
 * partial match table that have been calculated before (You may use
 * <code>lau::PartialMatchTable</code>).  If the target string doesn't match
 * the source string, the function will return -1.
 * <br>
 * This function uses Knuth-Morris-Pratt algorithm.
 * @tparam T The type of string (MUST have operator[])
 * @tparam Container The type of the partial match table
 * @param source
 * @param sourceLength
 * @param target
 * @param targetLength
 * @param table the partial table that have been calculated
 * @return the index of the place where the target string match the source
 * string; if the target string doesn't match, a -1 will be returned.
 */
template<class T, class Container>
[[nodiscard]] SizeT StringMatchingWithPartialTable(const T& source,
                                                   SizeT sourceLength,
                                                   const T& target,
                                                   SizeT targetLength,
                                                   const Container& table) {
    SizeT targetCursor = 0; // Target pointer
    SizeT sourceCursor = 0; // Source pointer
    while (sourceCursor < sourceLength) {
        if (targetCursor == -1 ||
            target[targetCursor] == source[sourceCursor]) {
            ++targetCursor;
            ++sourceCursor;
            if (targetCursor == targetLength) {
                return (sourceCursor - targetLength);
            }
        } else {
            targetCursor = table[targetCursor];
        }
    }
    return -1;
}

/**
 * Find the index where a target string matches the source string.  If the
 * target string doesn't match the source string, the function will return -1.
 * <br>
 * This function uses Knuth-Morris-Pratt algorithm.
 * @tparam T The type of string (MUST have operator[])
 * @tparam Allocator the allocator type to allocate the partial match table
 * @param source
 * @param sourceLength
 * @param target
 * @param targetLength
 * @param allocator
 * @return the index of the place where the target string match the source
 * string; if the target string doesn't match, a -1 will be returned.
 */
template<class T, class Allocator = std::allocator<SizeT>>
[[nodiscard]] SizeT StringMatching(const T& source, SizeT sourceLength,
                                   const T& target, SizeT targetLength,
                                   Allocator allocator = Allocator()) {
    if (targetLength == 0) return 0;
    SizeT* table = allocator.allocate(targetLength);
    PartialMatchTable(target, targetLength, table);
    SizeT value = StringMatchingWithPartialTable(source, sourceLength, target, targetLength, table);
    allocator.deallocate(table, targetLength);
    return value;
}

/**
 * Find the index where a target string matches the source string.  If the
 * target string doesn't match the source string, the function will return -1.
 * <br>
 * This function uses Knuth-Morris-Pratt algorithm.
 * @tparam Allocator the allocator type to allocate the partial match table
 * @param source
 * @param target
 * @param allocator
 * @return the index of the place where the target string match the source
 * string; if the target string doesn't match, a -1 will be returned.
 */
template<class Allocator = std::allocator<SizeT>>
[[nodiscard]] SizeT StringMatching(const char* source,
                                   const char* target,
                                   Allocator allocator = Allocator()) {
    SizeT sourceLength = strlen(source);
    SizeT targetLength = strlen(target);
    if (targetLength == 0) return 0;
    SizeT* table = allocator.allocate(targetLength);
    PartialMatchTable(target, targetLength, table);
    SizeT value = StringMatchingWithPartialTable(source, sourceLength, target, targetLength, table);
    allocator.deallocate(table, targetLength);
    return value;
}

/**
 * Find the index where a target string matches the source string.  If the
 * target string doesn't match the source string, the function will return -1.
 * <br>
 * This function uses Knuth-Morris-Pratt algorithm.
 * @tparam Allocator the allocator type to allocate the partial match table
 * @param source
 * @param target
 * @param allocator
 * @return the index of the place where the target string match the source
 * string; if the target string doesn't match, a -1 will be returned.
 */
template<class Allocator = std::allocator<SizeT>>
[[nodiscard]] SizeT StringMatching(const std::string& source,
                                   const std::string& target,
                                   Allocator allocator = Allocator()) {
    SizeT sourceLength = source.size();
    SizeT targetLength = target.size();
    if (targetLength == 0) return 0;
    SizeT* table = allocator.allocate(targetLength);
    PartialMatchTable(target, targetLength, table);
    SizeT value = StringMatchingWithPartialTable(source, sourceLength, target, targetLength, table);
    allocator.deallocate(table, targetLength);
    return value;
}

}

#endif //LAU_CPP_LIB_LAU_ALGORITHM_H
