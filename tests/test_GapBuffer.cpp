/**
 * @file test_GapBuffer.cpp
 * @brief Unit tests for GapBuffer class
 */

#include "Model/GapBuffer.hpp"
#include <gtest/gtest.h>

using namespace wnebula;

class GapBufferTest : public ::testing::Test {
  protected:
    void SetUp() override { buffer = std::make_unique<GapBuffer>(10); }

    void TearDown() override { buffer.reset(); }

    std::unique_ptr<GapBuffer> buffer;
};

// ============================================================================
// Constructor Tests
// ============================================================================

TEST_F(GapBufferTest, ConstructorInitializesBuffer) {
    EXPECT_EQ(buffer->size(), 10);
    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

TEST_F(GapBufferTest, ConstructorWithCustomSize) {
    auto customBuffer = std::make_unique<GapBuffer>(100);
    EXPECT_EQ(customBuffer->size(), 100);
}

// ============================================================================
// Insert Tests
// ============================================================================

TEST_F(GapBufferTest, InsertSingleCharacter) {
    buffer->insert('H');
    EXPECT_EQ(buffer->getCursorPosition(), 1);
}

TEST_F(GapBufferTest, InsertMultipleCharacters) {
    buffer->insert('H');
    buffer->insert('e');
    buffer->insert('l');
    buffer->insert('l');
    buffer->insert('o');
    EXPECT_EQ(buffer->getCursorPosition(), 5);
}

TEST_F(GapBufferTest, InsertBeyondInitialCapacity) {
    // Insert more characters than initial size
    for (int i = 0; i < 20; ++i) {
        buffer->insert('a');
    }
    EXPECT_EQ(buffer->getCursorPosition(), 20);
    // Buffer should have grown
    EXPECT_GT(buffer->size(), 10);
}

// ============================================================================
// Delete Tests
// ============================================================================

TEST_F(GapBufferTest, DeleteSingleCharacter) {
    buffer->insert('H');
    buffer->insert('i');
    buffer->deleteChar();
    EXPECT_EQ(buffer->getCursorPosition(), 1);
}

TEST_F(GapBufferTest, DeleteFromEmptyBuffer) {
    buffer->deleteChar();
    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

TEST_F(GapBufferTest, DeleteMultipleCharacters) {
    buffer->insert('H');
    buffer->insert('e');
    buffer->insert('l');
    buffer->deleteChar();
    buffer->deleteChar();
    EXPECT_EQ(buffer->getCursorPosition(), 1);
}

// ============================================================================
// Cursor Movement Tests
// ============================================================================

TEST_F(GapBufferTest, MoveCursorForward) {
    buffer->insert('H');
    buffer->insert('e');
    buffer->insert('l');
    buffer->moveCursor(0); // Move gap to position 0
    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

TEST_F(GapBufferTest, MoveCursorBackward) {
    buffer->insert('H');
    buffer->insert('e');
    buffer->moveCursor(1); // Move gap to position 1
    EXPECT_EQ(buffer->getCursorPosition(), 1);
}

// ============================================================================
// Complex Operations
// ============================================================================

TEST_F(GapBufferTest, InsertDeleteInsertSequence) {
    buffer->insert('H');
    buffer->insert('e');
    buffer->deleteChar();
    buffer->insert('i');
    EXPECT_EQ(buffer->getCursorPosition(), 2);
}

TEST_F(GapBufferTest, InsertAtDifferentPositions) {
    buffer->insert('H');
    buffer->insert('e');
    buffer->insert('l');
    buffer->insert('l');
    buffer->insert('o');

    // Move cursor to middle and insert
    buffer->moveCursor(2);
    buffer->insert('X');

    // Cursor should now be at position 3
    EXPECT_EQ(buffer->getCursorPosition(), 3);
}

TEST_F(GapBufferTest, StressTestLargeInserts) {
    // Insert 1000 characters
    for (int i = 0; i < 1000; ++i) {
        buffer->insert(static_cast<char>('a' + (i % 26)));
    }
    EXPECT_EQ(buffer->getCursorPosition(), 1000);
}

TEST_F(GapBufferTest, BufferGrowth) {
    size_t initialSize = buffer->size();

    // Fill beyond initial capacity
    for (size_t i = 0; i < initialSize + 10; ++i) {
        buffer->insert('x');
    }

    // Buffer should have grown
    EXPECT_GT(buffer->size(), initialSize);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(GapBufferTest, MultipleDeletes) {
    buffer->insert('A');
    buffer->insert('B');
    buffer->insert('C');

    buffer->deleteChar();
    buffer->deleteChar();
    buffer->deleteChar();
    buffer->deleteChar(); // Extra delete

    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

TEST_F(GapBufferTest, AlternatingInsertDelete) {
    for (int i = 0; i < 10; ++i) {
        buffer->insert('X');
        if (i % 2 == 0) {
            buffer->deleteChar();
        }
    }
    EXPECT_EQ(buffer->getCursorPosition(), 5);
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
