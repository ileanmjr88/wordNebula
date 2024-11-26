#ifndef GAPBUFFER_HPP
#define GAPBUFFER_HPP

#include <vector>

class GapBuffer {
public:
    GapBuffer(size_t initial_size = 10);
    void insert(char c);
    void remove();
    char get(size_t index) const;
    size_t size() const;

private:
    std::vector<char> buffer;
    size_t gap_start;
    size_t gap_end;

    void move_gap_to(size_t index);
    void resize_buffer(size_t new_size);
};

#endif // GAPBUFFER_HPP