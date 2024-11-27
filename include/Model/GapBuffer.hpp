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
    GapBuffer(size_t initial_size = 100);
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
    size_t gap_start;
    size_t gap_end;

    void move_gap_to(size_t index);
    void resize_buffer(size_t new_size);
};
} // namespace wnebula

#endif // GAPBUFFER_HPP