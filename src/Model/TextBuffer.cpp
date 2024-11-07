
/**
 * @file TextBuffer.cpp
 * @brief Implementation of the TextBuffer class for managing a text buffer with cursor operations.
 * @author Ilean Monterrubio Jr
 */

#include "Model/TextBuffer.hpp"

namespace wnebula {

TextBuffer::TextBuffer() {
    buffer = "";
    currentCursor = 0;
    spdlog::info("TextBuffer created");
}

TextBuffer::~TextBuffer() { spdlog::default_logger()->flush(); }

void TextBuffer::insertChar(char c) {
    buffer.insert(currentCursor, 1, c);
    currentCursor++;
    spdlog::info("Inserted character '{}' at position {}. Buffer: {}", c, currentCursor - 1, buffer);
}

void TextBuffer::deleteChar() {
    if (currentCursor > 0) {
        buffer.erase(currentCursor - 1, 1);
        currentCursor--;
        spdlog::info("Deleted character at position {}. Buffer: {}", currentCursor, buffer);
    } else {
        spdlog::warn("Attempted to delete character at position 0. Buffer: {}", buffer);
    }
}

void TextBuffer::insert(char c) { insertChar(c); }

void TextBuffer::deleteText(int position, int length) {
    if (position >= 0 && position + length <= buffer.size()) {
        buffer.erase(position, length);
        spdlog::info("Deleted text from position {} with length {}. Buffer: {}", position, length, buffer);
    } else {
        spdlog::warn("Attempted to delete text out of bounds. Buffer: {}", buffer);
    }
}

std::string TextBuffer::getText() const { return buffer; }

int TextBuffer::getCursorPosition() const { return currentCursor; }

void TextBuffer::moveCursor(int offset) {
    int newCursor = currentCursor + offset;
    if (newCursor >= 0 && newCursor <= buffer.size()) {
        currentCursor = newCursor;
        spdlog::info("Moved cursor to position {}. Buffer: {}", currentCursor, buffer);
    } else {
        spdlog::warn("Attempted to move cursor out of bounds. Buffer: {}", buffer);
    }
}
} // namespace wnebula
