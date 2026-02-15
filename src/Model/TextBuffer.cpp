#include "Model/TextBuffer.hpp"
#include <algorithm>
#include <cctype>
#include <spdlog/spdlog.h>

namespace wnebula {

TextBuffer::TextBuffer() { spdlog::info("TextBuffer created"); }

// Text Operations
void TextBuffer::insertChar(char c) {
    buffer.insert(static_cast<size_t>(currentCursor), 1, c);
    currentCursor++;
}

void TextBuffer::insertText(const std::string &text) {
    buffer.insert(static_cast<size_t>(currentCursor), text);
    currentCursor += static_cast<int>(text.size());
}

void TextBuffer::deleteChar() {
    if (currentCursor > 0) {
        buffer.erase(static_cast<size_t>(currentCursor - 1), 1);
        currentCursor--;
    }
}

void TextBuffer::deleteForward() {
    if (currentCursor < static_cast<int>(buffer.size())) {
        buffer.erase(static_cast<size_t>(currentCursor), 1);
    }
}

void TextBuffer::deleteText(int position, int length) {
    if (position >= 0 && static_cast<size_t>(position + length) <= buffer.size()) {
        buffer.erase(static_cast<size_t>(position), static_cast<size_t>(length));
        if (currentCursor > position) {
            currentCursor = std::max(position, currentCursor - length);
        }
    }
}

// Text Access
std::string TextBuffer::getText() const { return buffer; }

std::string TextBuffer::getTextRange(int start, int length) const {
    if (start < 0 || static_cast<size_t>(start + length) > buffer.size()) {
        return "";
    }
    return buffer.substr(static_cast<size_t>(start), static_cast<size_t>(length));
}

int TextBuffer::getLength() const { return static_cast<int>(buffer.size()); }

// Cursor Management
int TextBuffer::getCursorPosition() const { return currentCursor; }

void TextBuffer::setCursorPosition(int position) { currentCursor = std::clamp(position, 0, static_cast<int>(buffer.size())); }

void TextBuffer::moveCursor(int offset) { setCursorPosition(currentCursor + offset); }

// Smart Navigation
int TextBuffer::findNextWordBoundary(int fromPos) const {
    const int len = static_cast<int>(buffer.size());
    int pos = fromPos;
    // Skip current word (non-whitespace)
    while (pos < len && !std::isspace(static_cast<unsigned char>(buffer[static_cast<size_t>(pos)]))) {
        pos++;
    }
    // Skip whitespace
    while (pos < len && std::isspace(static_cast<unsigned char>(buffer[static_cast<size_t>(pos)]))) {
        pos++;
    }
    return pos;
}

int TextBuffer::findPrevWordBoundary(int fromPos) const {
    int pos = fromPos;
    // Skip whitespace backwards
    while (pos > 0 && std::isspace(static_cast<unsigned char>(buffer[static_cast<size_t>(pos - 1)]))) {
        pos--;
    }
    // Skip word backwards
    while (pos > 0 && !std::isspace(static_cast<unsigned char>(buffer[static_cast<size_t>(pos - 1)]))) {
        pos--;
    }
    return pos;
}

int TextBuffer::findNextParagraph(int fromPos) const {
    const int len = static_cast<int>(buffer.size());
    int pos = fromPos;
    while (pos < len) {
        if (buffer[static_cast<size_t>(pos)] == '\n') {
            return pos + 1;
        }
        pos++;
    }
    return len;
}

int TextBuffer::findPrevParagraph(int fromPos) const {
    int pos = fromPos;
    if (pos > 0) {
        pos--; // Move back from current position
    }
    while (pos > 0) {
        if (buffer[static_cast<size_t>(pos - 1)] == '\n') {
            return pos;
        }
        pos--;
    }
    return 0;
}

// Statistics
int TextBuffer::getWordCount() const {
    int count = 0;
    bool inWord = false;
    for (const char c : buffer) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            inWord = false;
        } else if (!inWord) {
            inWord = true;
            count++;
        }
    }
    return count;
}

int TextBuffer::getParagraphCount() const {
    if (buffer.empty()) {
        return 1;
    }
    return 1 + static_cast<int>(std::count(buffer.begin(), buffer.end(), '\n'));
}

} // namespace wnebula
