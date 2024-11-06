#ifndef TEXTBUFFER_HPP
#define TEXTBUFFER_HPP
#pragma once
#include <string>
#include <vector>
#include <algorithm> // Ensure this is included for std::clamp
#include <spdlog/spdlog.h>


namespace wnebula{
    class TextBuffer{
    public:
        TextBuffer();
        ~TextBuffer();
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