

#include "TextBuffer.hpp"

namespace wnebula{

    TextBuffer::TextBuffer(){
        buffer = "";
        currentCursor = 0;
        spdlog::info("TextBuffer created");
    }

    TextBuffer::~TextBuffer(){
        spdlog::default_logger()->flush();
    }
    
    void TextBuffer::insertChar(char c){
        buffer.insert(currentCursor, 1, c);
        currentCursor++;
        spdlog::info("Inserted character '{}' at position {}. Buffer: {}", c, currentCursor - 1, buffer);
    }

    void TextBuffer::deleteChar(){
        if(currentCursor > 0){
            buffer.erase(currentCursor - 1, 1);
            currentCursor--;
            spdlog::info("Deleted character at position {}. Buffer: {}", currentCursor, buffer);
        } else {
            spdlog::warn("Attempted to delete character at position 0. Buffer: {}", buffer);
        }
    }

    void TextBuffer::moveCursor(int offset){
        int newCursor = std::clamp(currentCursor + offset, 0, static_cast<int>(buffer.size()));
        spdlog::info("Moved cursor from position {} to position {}", currentCursor, newCursor);
        currentCursor = newCursor;
    }

    std::string TextBuffer::getText() const{
        spdlog::info("Returning buffer: {}", buffer);
        return buffer;
    }

    int TextBuffer::getCursorPosition() const{
        spdlog::info("Returning cursor position: {}", currentCursor); 
        return currentCursor;
    }

} // namespace wnebula

