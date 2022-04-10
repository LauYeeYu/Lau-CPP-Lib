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
 * @file lau/utility.h
 *
 * This is a external header file, including all the tools that can boost
 * efficiency.
 */

#ifndef LAU_CPP_LIB_LAU_UTILITY_H
#define LAU_CPP_LIB_LAU_UTILITY_H

#include "token_scanner.h"

namespace lau {

template<class T1, class T2>
class Pair {
public:
    Pair() : first(), second() {}
    Pair(const Pair& other) = default;
    Pair(Pair&& other) = default;
    Pair(const T1& x, const T2& y) : first(x), second(y) {}

    template<class U1, class U2>
    Pair(U1&& x, U2&& y) : first(x), second(y) {}

    template<class U1, class U2>
    Pair(const Pair<U1, U2>& other) : first(other.first), second(other.second) {}

    template<class U1, class U2>
    Pair(Pair<U1, U2>&& other) : first(other.first), second(other.second) {}

    T1 first;
    T2 second;
};

template<class Key, class Value>
struct KeyValuePair {
    KeyValuePair() : key(), value() {}
    KeyValuePair(const KeyValuePair& other) = default;
    KeyValuePair(KeyValuePair&& other) = default;
    KeyValuePair(const Key& key, const Value& value) : key(key), value(value) {}

    template<class KeyIn, class ValueIn>
    KeyValuePair(KeyIn&& key, ValueIn&& value) : key(key), value(value) {}

    template<class KeyIn, class ValueIn>
    KeyValuePair(const Pair<KeyIn, ValueIn>& other) : key(other.first), value(other.second) {}

    template<class KeyIn, class ValueIn>
    KeyValuePair(Pair<KeyIn, ValueIn>&& other) : key(other.first), value(other.second) {}

    const Key key;
    Value value;
};

}

#endif // LAU_CPP_LIB_LAU_UTILITY_H
