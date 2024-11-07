#ifndef TEXTBUFFER_HPP
#define TEXTBUFFER_HPP
#pragma once

#include <algorithm>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

#include "Model/IBuffer.hpp"

namespace wnebula {
class TextBuffer : public IBuffer {
  public:
    TextBuffer();
    ~TextBuffer();

    void insert(char c);
    void insertChar(char c);
    void deleteChar();
    void deleteText(int position, int length);
    void moveCursor(int offset);
    std::string getText() const;
    int getCursorPosition() const;

  private:
    std::string buffer;
    int currentCursor = 0;
};
} // namespace wnebula

#endif // TEXTBUFFER_HPP