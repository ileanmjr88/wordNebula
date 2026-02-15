#pragma once
#include <string>

namespace wnebula {

/**
 * @brief Interface for all buffer implementations
 *
 * Stores PLAIN TEXT ONLY (no formatting - styling handled separately).
 * Cursor position is LINEAR (0 to length), not (row, col).
 *
 * Design rationale:
 * - Plain text buffer keeps Model simple and focused
 * - Styling/formatting applied as separate layer (future Phase 2)
 * - Linear cursor position simplifies viewport calculations
 * - Word/paragraph navigation methods support smart cursor movement
 *
 * Implementations:
 *   - TextBuffer: Simple std::string based implementation
 *   - GapBuffer: Efficient implementation for typing (Phase 1 focus)
 *   - Future: PieceTable (undo/redo), RopeBuffer (large files)
 */
class IBuffer {
  public:
    virtual ~IBuffer() = default;

    // ========================================================================
    // TEXT OPERATIONS
    // ========================================================================

    /**
     * @brief Insert character at current cursor position
     * @param c Character to insert
     *
     * Inserts character at cursor and advances cursor by 1.
     * Example: cursor=5, insertChar('X') → cursor=6
     */
    virtual void insertChar(char c) = 0;

    /**
     * @brief Insert string at current cursor position
     * @param text String to insert
     *
     * Inserts entire string at cursor and advances cursor by text.length().
     * Example: cursor=5, insertText("Hello") → cursor=10
     */
    virtual void insertText(const std::string &text) = 0;

    /**
     * @brief Delete character BEFORE cursor (backspace behavior)
     *
     * Deletes character at cursor-1 and moves cursor back by 1.
     * If cursor=0, does nothing.
     * Example: cursor=5, deleteChar() → cursor=4
     */
    virtual void deleteChar() = 0;

    /**
     * @brief Delete character AT cursor (delete key behavior)
     *
     * Deletes character at cursor position, cursor stays at same position.
     * If cursor=length, does nothing.
     * Example: cursor=5, buffer="Hello World", deleteForward() → "HelloWorld", cursor=5
     */
    virtual void deleteForward() = 0;

    /**
     * @brief Delete range of characters
     * @param position Starting position (0-indexed)
     * @param length Number of characters to delete
     *
     * Deletes substring [position, position+length).
     * Cursor position updated if affected by deletion.
     */
    virtual void deleteText(int position, int length) = 0;

    // ========================================================================
    // TEXT ACCESS (for Presenter/View)
    // ========================================================================

    /**
     * @brief Get entire buffer content as string
     * @return Complete buffer text
     *
     * Used for: save to file, word count, search
     * Performance: O(n) - may need to reconstruct string from internal representation
     */
    [[nodiscard]] virtual std::string getText() const = 0;

    /**
     * @brief Get substring of buffer
     * @param start Starting position (0-indexed)
     * @param length Number of characters to get
     * @return Substring [start, start+length)
     *
     * Used for: viewport rendering (only get visible portion)
     * Example: buffer="Hello World", getTextRange(0, 5) → "Hello"
     */
    [[nodiscard]] virtual std::string getTextRange(int start, int length) const = 0;

    /**
     * @brief Get total character count
     * @return Number of characters in buffer
     */
    [[nodiscard]] virtual int getLength() const = 0;

    // ========================================================================
    // CURSOR MANAGEMENT
    // ========================================================================

    /**
     * @brief Get current cursor position
     * @return Linear cursor position (0 to length)
     *
     * Position 0 = before first character
     * Position length = after last character
     */
    [[nodiscard]] virtual int getCursorPosition() const = 0;

    /**
     * @brief Set cursor to absolute position
     * @param position New cursor position (0 to length)
     *
     * Clamps to valid range [0, length].
     * Used for: jumping to specific position, search results
     */
    virtual void setCursorPosition(int position) = 0;

    /**
     * @brief Move cursor by relative offset
     * @param offset Number of positions to move (positive=right, negative=left)
     *
     * Clamps result to valid range [0, length].
     * Used for: arrow keys, page up/down
     * Example: cursor=5, moveCursor(3) → cursor=8
     */
    virtual void moveCursor(int offset) = 0;

    // ========================================================================
    // SMART NAVIGATION (Word/Paragraph Boundaries)
    // ========================================================================

    /**
     * @brief Find position of next word boundary
     * @param fromPos Starting position to search from
     * @return Position of start of next word, or length if no next word
     *
     * Word boundary = transition from non-space to space, or space to non-space.
     * Used for: Ctrl+Right navigation
     *
     * Example: "Hello World"
     *           ^     ^     ^
     *           0     6    11
     * findNextWordBoundary(0) → 6
     * findNextWordBoundary(6) → 11
     */
    [[nodiscard]] virtual int findNextWordBoundary(int fromPos) const = 0;

    /**
     * @brief Find position of previous word boundary
     * @param fromPos Starting position to search from
     * @return Position of start of previous word, or 0 if no previous word
     *
     * Used for: Ctrl+Left navigation
     */
    [[nodiscard]] virtual int findPrevWordBoundary(int fromPos) const = 0;

    /**
     * @brief Find position of next paragraph (next \n or end of buffer)
     * @param fromPos Starting position to search from
     * @return Position after next \n, or length if no next paragraph
     *
     * Used for: Ctrl+Down navigation
     *
     * Example: "Line 1\nLine 2\nLine 3"
     *           ^      ^      ^
     *           0      7     14
     * findNextParagraph(0) → 7
     */
    [[nodiscard]] virtual int findNextParagraph(int fromPos) const = 0;

    /**
     * @brief Find position of previous paragraph (previous \n or start)
     * @param fromPos Starting position to search from
     * @return Position of start of previous paragraph, or 0
     *
     * Used for: Ctrl+Up navigation
     */
    [[nodiscard]] virtual int findPrevParagraph(int fromPos) const = 0;

    // ========================================================================
    // STATISTICS (for status bar display)
    // ========================================================================

    /**
     * @brief Count words in buffer
     * @return Number of words
     *
     * Word = sequence of non-whitespace characters.
     * Example: "Hello World\n" → 2 words
     * Used for: status bar display
     */
    [[nodiscard]] virtual int getWordCount() const = 0;

    /**
     * @brief Count paragraphs in buffer
     * @return Number of paragraphs
     *
     * Paragraph count = number of \n + 1
     * Example: "Line 1\nLine 2" → 2 paragraphs
     * Empty buffer → 1 paragraph
     */
    [[nodiscard]] virtual int getParagraphCount() const = 0;
};

} // namespace wnebula
