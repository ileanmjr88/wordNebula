#ifndef IBUFFER_HPP
#define IBUFFER_HPP
#pragma once
#include <string>

namespace wnebula {

class IBuffer {
  public:
    virtual ~IBuffer() = default;

    virtual void insert(char c) = 0;
    virtual void insertChar(char c) = 0;
    virtual void deleteChar() = 0;
    virtual void deleteText(int position, int length) = 0;
    virtual void moveCursor(int offset) = 0;
    virtual std::string getText() const = 0;
    virtual int getCursorPosition() const = 0;
};

} // namespace wnebula

#endif // IBUFFER_HPP