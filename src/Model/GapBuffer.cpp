#include "Model/GapBuffer.hpp"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <spdlog/spdlog.h>
#include <string>
#include <utility>
#include <vector>

namespace wnebula {
// Default Constructor
GapBuffer::GapBuffer(size_t initialSize) : buffer(std::max(initialSize, static_cast<size_t>(1))), gapStart(0), gapEnd(std::max(initialSize, static_cast<size_t>(1))), cursor(0) {
    spdlog::info("GapBuffer created with initial size: {}", buffer.size());
}

// Text Operations
void GapBuffer::insertChar(char c) {
    moveGapToCursor();

    if (getGapSize() < 1) {
        expandGap();
    }

    buffer[gapStart] = c;
    gapStart++;
    cursor++;
}

void GapBuffer::insertText(const std::string &text) {
    moveGapToCursor();

    if (getGapSize() < text.size()) {
        expandGap(text.size());
    }

    // Insert each character in the text
    for (const char c : text) {
        buffer[gapStart] = c;
        gapStart++;
        cursor++;
    }
}

void GapBuffer::deleteChar() {
    moveGapToCursor();

    // Delete character from buffer
    if (gapStart > 0) {
        gapStart--;
        cursor--;
    }
}

void GapBuffer::deleteForward() {
    moveGapToCursor();

    if (gapEnd < buffer.size()) {
        gapEnd++;
    }
}

void GapBuffer::deleteText(int position, int length) {
    if (position < 0 || length < 0 || length > getLength() - position) {
        return;
    }
    const int savedCursor = cursor;
    setCursorPosition(position);
    moveGapToCursor();
    for (int i = 0; i < length && gapEnd < buffer.size(); i++) {
        gapEnd++;
    }
    if (savedCursor > position) {
        cursor = std::max(position, savedCursor - length);
    } else {
        cursor = savedCursor;
    }
}

// Text Access
std::string GapBuffer::getText() const {
    std::string result;
    result.reserve(buffer.size() - getGapSize());
    for (size_t i = 0; i < gapStart; i++) {
        result += buffer[i];
    }
    for (size_t i = gapEnd; i < buffer.size(); i++) {
        result += buffer[i];
    }
    return result;
}

std::string GapBuffer::getTextRange(int start, int length) const {
    if (start < 0 || length <= 0 || start >= getLength()) {
        return "";
    }
    const int clampedLength = std::min(length, getLength() - start);
    std::string result;
    result.reserve(static_cast<size_t>(clampedLength));
    for (int i = start; i < start + clampedLength; i++) {
        result += getCharAt(i);
    }
    return result;
}

int GapBuffer::getLength() const { return static_cast<int>(buffer.size() - getGapSize()); }

// Cursor Management
int GapBuffer::getCursorPosition() const { return cursor; }

void GapBuffer::setCursorPosition(int position) { cursor = std::clamp(position, 0, getLength()); }

// Smart Navigation
int GapBuffer::findNextWordBoundary(int fromPos) const {
    const int len = getLength();
    int pos = fromPos;
    while (pos < len && !isWhitespace(getCharAt(pos))) {
        pos++;
    }
    while (pos < len && isWhitespace(getCharAt(pos))) {
        pos++;
    }
    return pos;
}

int GapBuffer::findPrevWordBoundary(int fromPos) const {
    int pos = fromPos;
    while (pos > 0 && isWhitespace(getCharAt(pos - 1))) {
        pos--;
    }
    while (pos > 0 && !isWhitespace(getCharAt(pos - 1))) {
        pos--;
    }
    return pos;
}

int GapBuffer::findNextParagraph(int fromPos) const {
    const int len = getLength();
    int pos = fromPos;
    while (pos < len) {
        if (getCharAt(pos) == '\n') {
            return pos + 1;
        }
        pos++;
    }
    return len;
}

int GapBuffer::findPrevParagraph(int fromPos) const {
    int pos = fromPos;
    if (pos > 0) {
        pos--;
    }
    while (pos > 0) {
        if (getCharAt(pos - 1) == '\n') {
            return pos;
        }
        pos--;
    }
    return 0;
}

// Statistics
int GapBuffer::getWordCount() const {
    int count = 0;
    bool inWord = false;
    const int len = getLength();
    for (int i = 0; i < len; i++) {
        if (isWhitespace(getCharAt(i))) {
            inWord = false;
        } else if (!inWord) {
            inWord = true;
            count++;
        }
    }
    return count;
}

int GapBuffer::getParagraphCount() const {
    if (getLength() == 0) {
        return 1;
    }
    int count = 1;
    const int len = getLength();
    for (int i = 0; i < len; i++) {
        if (getCharAt(i) == '\n') {
            count++;
        }
    }
    return count;
}

// Private Helpers
char GapBuffer::getCharAt(int logicalPos) const {
    assert(logicalPos >= 0 && logicalPos < getLength() && "getCharAt: logicalPos out of bounds");
    const auto pos = static_cast<size_t>(logicalPos);
    if (pos < gapStart) {
        return buffer[pos];
    }
    return buffer[pos + getGapSize()];
}

bool GapBuffer::isWhitespace(char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; }

void GapBuffer::moveCursor(int offset) {
    cursor += offset;
    const int maxPos = static_cast<int>(buffer.size() - getGapSize());
    cursor = std::clamp(cursor, 0, maxPos);
}

// Internal Helper Methods
size_t GapBuffer::getGapSize() const {
    // Return space left in buffer
    return gapEnd - gapStart;
}

void GapBuffer::expandGap(size_t minGapSize) {
    const size_t contentSize = buffer.size() - getGapSize();
    const size_t newSize = std::max(buffer.size() * 2, contentSize + minGapSize);

    std::vector<char> newBuffer(newSize);
    // Copy the text before the gap
    std::copy(buffer.begin(), buffer.begin() + static_cast<std::ptrdiff_t>(gapStart), newBuffer.begin());

    // Copy the text after the gap
    const size_t newGapEnd = newSize - (buffer.size() - gapEnd);
    std::copy(buffer.begin() + static_cast<std::ptrdiff_t>(gapEnd), buffer.end(), newBuffer.begin() + static_cast<std::ptrdiff_t>(newGapEnd));

    // Update gap pointers
    gapEnd = newGapEnd;
    buffer = std::move(newBuffer);
}

void GapBuffer::moveGapToCursor() {
    const size_t offset = gapEnd - gapStart;
    const auto cursorPos = static_cast<size_t>(cursor);

    // Edge case: if the cursor is within the gap
    if (cursorPos == gapStart) {
        return;
    }

    // Edge case: if the buffer is empty
    if (buffer.empty()) {
        return;
    }

    // Identify the direction of the cursor
    if (cursorPos < gapStart) {
        // Move the gap to the left
        std::copy_backward(buffer.begin() + static_cast<std::ptrdiff_t>(cursorPos), buffer.begin() + static_cast<std::ptrdiff_t>(gapStart), buffer.begin() + static_cast<std::ptrdiff_t>(gapEnd));
        gapStart = cursorPos;
        gapEnd = cursorPos + offset;
    } else if (cursorPos > gapStart) {
        // Move the gap to the right
        const size_t charsToMove = cursorPos - gapStart;
        std::copy(buffer.begin() + static_cast<std::ptrdiff_t>(gapEnd), buffer.begin() + static_cast<std::ptrdiff_t>(gapEnd + charsToMove), buffer.begin() + static_cast<std::ptrdiff_t>(gapStart));
        gapStart = cursorPos;
        gapEnd = cursorPos + offset;
    }
}

} // namespace wnebula
