#include "Model/GapBuffer.hpp"

namespace wnebula {

GapBuffer::GapBuffer(size_t initial_size = 100){
    buffer.resize(initial_size);
    gap_start = 0;
    gap_end = initial_size;
}

void GapBuffer::insert(char c){
    
}

} // namespace wnebula