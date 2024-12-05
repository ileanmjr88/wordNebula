#ifndef GAPBUFFER_HPP
#define GAPBUFFER_HPP
#pragma once

#include <algorithm>
#include <spdlog/spdlog.h>
#include <vector>

#include "Model/IBuffer.hpp"

namespace wnebula {
class GapBuffer : public IBuffer {
  public:
    GapBuffer(size_t initialSize = 150);
    void insert(char c);
    void insertChar(char c);
    void deleteChar();
    void deleteText(int position, int length);
    void moveCursor(int offset);
    char getText(size_t index) const;
    int getCursorPosition() const;
    size_t size() const;

  private:
    std::vector<char> buffer;
    size_t gapStart;
    size_t gapEnd;

    size_t getGapSpace() const;
    void moveGapToCursor(size_t index);
    void resizeBuffer(size_t new_size);
};
} // namespace wnebula

#endif // GAPBUFFER_HPP