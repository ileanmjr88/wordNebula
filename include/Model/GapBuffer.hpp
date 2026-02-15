#pragma once

#include "Model/IBuffer.hpp"
#include <string>
#include <vector>

namespace wnebula {

/**
 * @brief Gap buffer implementation for efficient text editing
 *
 * A gap buffer is a dynamic array with a "gap" (unused space) that follows
 * the cursor position. This makes insertions and deletions at the cursor
 * position very fast (O(1) amortized), which is ideal for text editors.
 *
 * Buffer structure:
 *   [text before gap][___gap___][text after gap]
 *                    ^         ^
 *                    gapStart  gapEnd
 *
 * Example: "Hello World" with cursor at position 6 (after "Hello ")
 *   [H][e][l][l][o][ ][_][_][_][W][o][r][l][d]
 *                    ^         ^
 *                    gapStart=6 gapEnd=9
 *
 * When user types 'X':
 *   [H][e][l][l][o][ ][X][_][_][W][o][r][l][d]
 *                      ^       ^
 *                      gapStart=7 gapEnd=9
 *
 * Design decisions:
 * - Gap moves with cursor for O(1) insertions
 * - Gap expands when full (doubles in size)
 * - Stores plain text only (no formatting)
 * - Linear cursor position (not row/col)
 */
class GapBuffer : public IBuffer {
  public:
    /**
     * @brief Construct a new Gap Buffer
     * @param initialSize Initial buffer capacity (default: 256 chars)
     */
    explicit GapBuffer(size_t initialSize = 256);

    /**
     * @brief Destructor - clean up buffer
     */
    ~GapBuffer() override = default;

    // ========================================================================
    // TEXT OPERATIONS (IBuffer interface implementation)
    // ========================================================================

    void insertChar(char c) override;
    void insertText(const std::string &text) override;
    void deleteChar() override;
    void deleteForward() override;
    void deleteText(int position, int length) override;

    // ========================================================================
    // TEXT ACCESS (IBuffer interface implementation)
    // ========================================================================

    std::string getText() const override;
    std::string getTextRange(int start, int length) const override;
    int getLength() const override;

    // ========================================================================
    // CURSOR MANAGEMENT (IBuffer interface implementation)
    // ========================================================================

    int getCursorPosition() const override;
    void setCursorPosition(int position) override;
    void moveCursor(int offset) override;

    // ========================================================================
    // SMART NAVIGATION (IBuffer interface implementation)
    // ========================================================================

    int findNextWordBoundary(int fromPos) const override;
    int findPrevWordBoundary(int fromPos) const override;
    int findNextParagraph(int fromPos) const override;
    int findPrevParagraph(int fromPos) const override;

    // ========================================================================
    // STATISTICS (IBuffer interface implementation)
    // ========================================================================

    int getWordCount() const override;
    int getParagraphCount() const override;

  private:
    // ========================================================================
    // INTERNAL STATE
    // ========================================================================

    std::vector<char> buffer; ///< Underlying character storage
    size_t gapStart;          ///< Index of first gap position
    size_t gapEnd;            ///< Index after last gap position (gap size = gapEnd - gapStart)
    int cursor;               ///< Current cursor position (logical position in text, not buffer)

    // ========================================================================
    // INTERNAL HELPER METHODS
    // ========================================================================

    /**
     * @brief Move gap to cursor position
     *
     * Moves characters to position the gap at the cursor location.
     * After this operation, gapStart == cursor.
     *
     * Example: Moving gap from position 3 to position 6
     *   Before: [a][b][c][_][_][d][e][f]
     *           gapStart=3, gapEnd=5
     *   After:  [a][b][c][d][e][f][_][_]
     *           gapStart=6, gapEnd=8
     */
    void moveGapToCursor();

    /**
     * @brief Expand gap to accommodate at least minGapSize characters
     *
     * Doubles the buffer size or grows to fit minGapSize, whichever is larger.
     * @param minGapSize Minimum required gap size (default: 1)
     */
    void expandGap(size_t minGapSize = 1);

    /**
     * @brief Get current gap size
     * @return Number of unused positions in gap
     */
    size_t getGapSize() const;

    /**
     * @brief Get character at logical position (skipping gap)
     * @param logicalPos Position in the text (0 to length-1)
     * @return Character at that position
     *
     * Converts logical position to physical buffer position,
     * accounting for the gap.
     */
    char getCharAt(int logicalPos) const;

    /**
     * @brief Check if character is whitespace
     * @param c Character to check
     * @return true if whitespace (space, tab, newline, etc.)
     */
    static bool isWhitespace(char c);
};

} // namespace wnebula
