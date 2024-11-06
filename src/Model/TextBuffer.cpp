

#include "TextBuffer.hpp"

namespace wnebula{

    TextBuffer::TextBuffer(){
        buffer = "";
        currentCursor = 0;
    }
    
    void TextBuffer::insertChar(char c){
        buffer.insert(currentCursor, 1, c);
        currentCursor++;
    }

    void TextBuffer::deleteChar(){
        if(currentCursor > 0){
            buffer.erase(currentCursor - 1, 1);
            currentCursor--;
        }
    }

    void TextBuffer::moveCursor(int offset){
        std::clamp(currentCursor + offset, 0, static_cast<int>(buffer.size()));
    }

    std::string TextBuffer::getText() const{
        return buffer;
    }

    int TextBuffer::getCursorPosition() const{
        return currentCursor;
    }

} // namespace wnebula

