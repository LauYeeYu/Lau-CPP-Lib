/**
 * @file lau/token_scanner.cpp
 *
 * This is a internal source file, including the implementation of the
 * <code>lau::TokenScanner</code> class.
 */

#include "token_scanner.h"

#include <iostream>
#include <string>

std::string lau::TokenScanner::NextToken() noexcept {
    if (mode_ == multiple) {
        // Skip delimiter
        while (current_ < buffer_.size() && buffer_[current_] == delimiter_) ++current_;
        int start = current_;

        // Find another delimiter
        while (current_ < buffer_.size() && buffer_[current_] != delimiter_) ++current_;
        return buffer_.substr(start, current_ - start);
    } else { // mode_ == single
        if (current_ >= buffer_.size()) return std::string();

        int start = current_;

        // Find another delimiter
        while (current_ < buffer_.size() && buffer_[current_] != delimiter_) ++current_;
        ++current_;
        return buffer_.substr(start, current_ - start - 1);
    }
}

std::string lau::TokenScanner::PeekNextToken() noexcept {
    if (mode_ == multiple) {
        // Skip delimiter
        while (current_ < buffer_.size() && buffer_[current_] == delimiter_) ++current_;
        int end = current_;

        // Find another delimiter
        while (end < buffer_.size() && buffer_[end] != delimiter_) ++end;
        return buffer_.substr(current_, end - current_);
    } else { // mode_ == single
        if (current_ >= buffer_.size()) return std::string();

        int end = current_;

        // Find another delimiter
        while (end < buffer_.size() && buffer_[end] != delimiter_) ++end;
        return buffer_.substr(current_, end - current_);
    }
}

bool lau::TokenScanner::HasMoreToken() noexcept {
    if (mode_ == multiple) {
        while (current_ < buffer_.size() && buffer_[current_] == delimiter_) ++current_;
    }
    return current_ < buffer_.size();
}

lau::TokenScanner& lau::TokenScanner::NewLine() noexcept {
    current_ = 0;
    std::getline(std::cin, buffer_);
    return *this;
}

long lau::TokenScanner::TotalLength() noexcept {
    return buffer_.length();
}

lau::TokenScanner& lau::TokenScanner::ChangeMode(TokenScannerMode mode) noexcept {
    mode_ = mode;
    return *this;
}

lau::TokenScanner& lau::TokenScanner::ResetState() noexcept {
    current_ = 0;
    return *this;
}

lau::TokenScanner& lau::TokenScanner::Read(std::string newInput) noexcept {
    buffer_ = std::move(newInput);
    current_ = 0;
    return *this;
}

lau::TokenScanner& lau::TokenScanner::SkipDelimiter() noexcept {
    while (current_ < buffer_.size() && buffer_[current_] == delimiter_) ++current_;
    return *this;
}

lau::TokenScanner& lau::TokenScanner::SetDelimiter(char delimiter) noexcept {
    delimiter_ = delimiter;
    return *this;
}

const std::string& lau::TokenScanner::GetInputString() const noexcept {
    return buffer_;
}

char lau::TokenScanner::GetDelimiter() const noexcept {
    return delimiter_;
}

lau::TokenScanner::TokenScannerMode lau::TokenScanner::GetMode() const noexcept {
    return mode_;
}

lau::TokenScanner& lau::TokenScanner::operator=(const lau::TokenScanner& obj) = default;

lau::TokenScanner& lau::TokenScanner::operator=(lau::TokenScanner&& obj) noexcept = default;
