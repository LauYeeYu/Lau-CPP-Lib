#ifndef LAU_CPP_LIB_LAU_TOKEN_SCANNER_H
#define LAU_CPP_LIB_LAU_TOKEN_SCANNER_H

#include <string>

namespace lau {

class TokenScanner {
public:
    enum TokenScannerMode {multiple, single};

    /**
     * The default constructor
     */
    TokenScanner() = default;

    ~TokenScanner() = default;

    /**
     * Construct a TokenScanner class with an input string, a customized
     * delimiter, and scanning mode.
     * @param input input string
     * @param delimiter customized delimiter, space by default
     * @param mode the scanning mode, <code>TokenScanner::multiple</code> by default
     */
    explicit TokenScanner(std::string input, char delimiter = ' ', TokenScannerMode mode = multiple)
        : buffer_(std::move(input)), delimiter_(delimiter), mode_(mode) {}

    /**
     * Read a new line from std::cin.
     */
    TokenScanner& NewLine() noexcept;

    /**
     * Get the new token, if <code>lau::TokenScanner::NextToken</code>
     * or <code>lau::TokenScanner::PeekNextToken</code> is called then, neither will
     * return this token again.
     * @return the next token
     */
    std::string NextToken() noexcept;

    /**
     * Get the new token, without moving the scanning pointer.  Different from
     * <code>lau::TokenScanner::NextToken</code>, it won't cause any difference
     * whether this function is called or not.  Namely, it just peeks the next
     * token without causing any major differences.
     * @return the next token
     */
    std::string PeekNextToken() noexcept;

    /**
     * Tell the length of the input string.
     * @return the total length of the input string
     */
    long TotalLength() noexcept;

    /**
     * Tell whether there remains token(s) or not.
     * @return the boolean of whether there remains token(s) or not
     */
    bool HasMoreToken() noexcept;

    /**
     * Change the scanning mode of the token scanner.
     * <br>
     * Since the two scanning modes may use different implementations,
     * changing the mode halfway in the scanning process may cause
     * UNDEFINED BEHAVIOUR!  So it is highly recommended to call the
     * <code>lau::TokenScanner::SkipDelimiter()</code> or
     * <code>lau::TokenScanner::ResetState()</code> to avoid
     * unexpected result.
     * @param mode either <code>lau::TokenScanner::single</code> or <code>lau::TokenScanner::multiple</code>
     * @return reference of the current TokenScanner class
     */
    TokenScanner& ChangeMode(TokenScannerMode mode) noexcept;

    /**
     * Reset the scanning state, making the scanning pointer back to the front
     * of the string.
     * @return reference of the current TokenScanner class
     */
    TokenScanner& ResetState() noexcept;

    /**
     * Read a new input string, and set the scanning pointer to the front of
     * the new input string.
     * @param newInput the new input string
     * @return reference of the current TokenScanner class
     */
    TokenScanner& Read(std::string newInput) noexcept;

    /**
     * Skip the delimiter(s) before the next token.  If this function meets
     * the end of the input string, it will stop at the end of the input
     * string.
     * @return reference of the current TokenScanner class
     */
    TokenScanner& SkipDelimiter() noexcept;

    /**
     * Set the new delimiter, the previous scanned result will be preserved.
     * <br>
     * Since the two scanning modes may use different implementations,
     * setting a new delimiter halfway in the scanning process may cause
     * UNDEFINED BEHAVIOUR!  So it is highly recommended to call the
     * <code>lau::TokenScanner::SkipDelimiter()</code> or
     * <code>TokenScanner::ResetState()</code> to avoid
     * unexpected result.
     * @param delimiter
     * @return reference of the current TokenScanner class
     */
    TokenScanner& SetDelimiter(char delimiter) noexcept;

    /**
     * Get the input string.
     * @return const reference to the input string
     */
    [[nodiscard]] const std::string& GetInputString() const noexcept;

    /**
     * Get the delimiter.
     * @return the current delimiter
     */
    [[nodiscard]] char GetDelimiter() const noexcept;

    /**
     * Get the scanning mode.
     * @return token scanner mode
     */
    [[nodiscard]] TokenScannerMode GetMode() const noexcept;

    TokenScanner& operator=(const TokenScanner& obj) noexcept;

    TokenScanner& operator=(TokenScanner&& obj) noexcept;

private:
    std::string buffer_;

    char delimiter_ = ' ';

    int current_ = 0;

    TokenScannerMode mode_ = multiple;
};

} // namespace lau

#endif //LAU_CPP_LIB_LAU_TOKEN_SCANNER_H
