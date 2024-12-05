#include "Model/GapBuffer.hpp"

namespace wnebula {
// Default Constructor
GapBuffer::GapBuffer(size_t initialSize = 150){
    spdlog::info("GapBuffer created");
    buffer.resize(initialSize);
    gapStart = 0;
    gapEnd = initialSize;
}

GapBuffer::~GapBuffer(){
    spdlog::default_logger()->flush();
}

// Public Functions
void GapBuffer::insert(char c){
    // Get current gap space left in buffer
    if(getGapSpace() < 1){
        resizeBuffer(buffer.size() * 2);
    }

    //Insert character into buffer
    buffer[gapStart] = c;
    gapStart++;
}

void GapBuffer::insertChar(char c){
    // Insert character into buffer
    insert(c);
}

void GapBuffer::moveCursor(int offset){
    // Move the cursor
    moveGapToCursor(offset);
}

void GapBuffer::deleteChar(){
    // Delete character from buffer
    if(gapStart > 0){
        gapStart--;
    } else if (gapEnd < buffer.size()) {
        gapEnd++;
    }
}


size_t GapBuffer::size() const {
    // Return the size of the buffer
    return buffer.size();
}



// Private Functions
size_t GapBuffer::getGapSpace() const {
    // Return space left in buffer
    return gapEnd - gapStart;
}

void GapBuffer::resizeBuffer(size_t newSize){
    // Resize the buffer
    std::vector<char> newBuffer(newSize);
    // Copy the text before the gap
    std::copy(buffer.begin(), buffer.begin() + gapStart, newBuffer.begin());
    // Get gap size
    size_t gapSize = gapEnd - gapStart;
    // Copy the text after the gap
    std::copy(buffer.begin() + gapEnd, buffer.end(), newBuffer.begin() + newSize - (buffer.size() - gapEnd));

    // Update gap pointers
    gapEnd = newSize - (buffer.size() - gapEnd);
    buffer = std::move(newBuffer);
}

void GapBuffer::moveGapToCursor(size_t index){
    size_t offset = gapEnd - gapStart;

    // Edge case: if the cursor is within the gap
    if( index >= gapStart && index <= gapEnd) return;
    // Edge case: if the buffer is empty
    if( buffer.empty() ) return;

    // Identify the direction of the cursor
    if ( index < gapStart){
        // Move the gap to the left
        std::copy_backward(buffer.begin() + index, buffer.begin() + gapStart, buffer.begin() + gapEnd);
        gapStart = index;
        gapEnd = index + offset;
    }else if (index > gapStart){
        // Move the gap to the right
        std::copy(buffer.begin() + gapEnd, buffer.begin() + index, buffer.begin() + gapStart);
    }
}

} // namespace wnebula