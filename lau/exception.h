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
 * @file lau/exception.h
 *
 * This is a external header file, including all exceptions in Lau CPP Lib.
 */

#ifndef LAU_CPP_LIB_LAU_EXCEPTION_H
#define LAU_CPP_LIB_LAU_EXCEPTION_H

#include <exception>

namespace lau {

class Exception : std::exception {
public:
    explicit Exception(const char* message) : message_(message) {}

    ~Exception() override = default;

    [[nodiscard]] const char* what() const noexcept override { return message_; }

protected:
    const char* message_;
};

class OutOfRange : public Exception {
public:
    explicit OutOfRange(const char* message = "Out of Range") : Exception(message) {}

    ~OutOfRange() override = default;

    [[nodiscard]] const char* what() const noexcept override { return message_; }
};

class RuntimeError : public Exception {
public:
    explicit RuntimeError(const char* message = "Runtime Error") : Exception(message) {}

    ~RuntimeError() override = default;

    [[nodiscard]] const char* what() const noexcept override { return message_; }
};

class InvalidIterator : public Exception {
public:
    explicit InvalidIterator(const char* message = "Invalid Iterator") : Exception(message) {}

    ~InvalidIterator() override = default;

    [[nodiscard]] const char* what() const noexcept override { return message_; }
};

class EmptyContainer : public Exception {
public:
    explicit EmptyContainer(const char* message = "Empty Container") : Exception(message) {}

    ~EmptyContainer() override = default;

    [[nodiscard]] const char* what() const noexcept override { return message_; }
};

}

#endif // LAU_CPP_LIB_LAU_EXCEPTION_H
