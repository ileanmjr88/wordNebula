#ifndef TEXTBUFFER_HPP
#define TEXTBUFFER_HPP
#pragma once
#include <string>
#include <vector>
#include <algorithm> // Ensure this is included for std::clamp


namespace wnebula{
    class TextBuffer{
    public:
        TextBuffer();
        void insertChar(char c);
        void deleteChar();
        void moveCursor(int offset);

        std::string getText() const;
        int getCursorPosition() const;

    private:
        std::string buffer;
        int currentCursor = 0;
    };
}

#endif // TEXTBUFFER_HPP