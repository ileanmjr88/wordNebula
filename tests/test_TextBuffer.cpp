/**
 * @file test_TextBuffer.cpp
 * @brief Unit tests for TextBuffer class
 */

#include "Model/TextBuffer.hpp"
#include <gtest/gtest.h>

using namespace wnebula;

class TextBufferTest : public ::testing::Test {
  protected:
    void SetUp() override { buffer = std::make_unique<TextBuffer>(); }

    void TearDown() override { buffer.reset(); }

    std::unique_ptr<TextBuffer> buffer;
};

// ============================================================================
// Constructor Tests
// ============================================================================

TEST_F(TextBufferTest, ConstructorInitializesEmptyBuffer) {
    EXPECT_EQ(buffer->getText(), "");
    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

// ============================================================================
// Insert Tests
// ============================================================================

TEST_F(TextBufferTest, InsertSingleCharacter) {
    buffer->insertChar('H');
    EXPECT_EQ(buffer->getText(), "H");
    EXPECT_EQ(buffer->getCursorPosition(), 1);
}

TEST_F(TextBufferTest, InsertMultipleCharacters) {
    buffer->insertChar('H');
    buffer->insertChar('e');
    buffer->insertChar('l');
    buffer->insertChar('l');
    buffer->insertChar('o');
    EXPECT_EQ(buffer->getText(), "Hello");
    EXPECT_EQ(buffer->getCursorPosition(), 5);
}

TEST_F(TextBufferTest, InsertAtMiddle) {
    buffer->insertChar('H');
    buffer->insertChar('e');
    buffer->insertChar('l');
    buffer->insertChar('o');
    buffer->moveCursor(-2); // Move to position 2
    buffer->insertChar('l');
    EXPECT_EQ(buffer->getText(), "Hello");
}

// ============================================================================
// Delete Tests
// ============================================================================

TEST_F(TextBufferTest, DeleteSingleCharacter) {
    buffer->insertChar('H');
    buffer->insertChar('i');
    buffer->deleteChar();
    EXPECT_EQ(buffer->getText(), "H");
    EXPECT_EQ(buffer->getCursorPosition(), 1);
}

TEST_F(TextBufferTest, DeleteFromEmptyBuffer) {
    buffer->deleteChar();
    EXPECT_EQ(buffer->getText(), "");
    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

TEST_F(TextBufferTest, DeleteMultipleCharacters) {
    buffer->insertChar('H');
    buffer->insertChar('e');
    buffer->insertChar('l');
    buffer->deleteChar();
    buffer->deleteChar();
    EXPECT_EQ(buffer->getText(), "H");
    EXPECT_EQ(buffer->getCursorPosition(), 1);
}

TEST_F(TextBufferTest, DeleteText) {
    buffer->insertChar('H');
    buffer->insertChar('e');
    buffer->insertChar('l');
    buffer->insertChar('l');
    buffer->insertChar('o');
    buffer->deleteText(1, 3); // Delete "ell"
    EXPECT_EQ(buffer->getText(), "Ho");
}

// ============================================================================
// Cursor Movement Tests
// ============================================================================

TEST_F(TextBufferTest, MoveCursorForward) {
    buffer->insertChar('H');
    buffer->insertChar('e');
    buffer->insertChar('l');
    buffer->moveCursor(-2); // Move to position 1
    EXPECT_EQ(buffer->getCursorPosition(), 1);
}

TEST_F(TextBufferTest, MoveCursorBackward) {
    buffer->insertChar('H');
    buffer->moveCursor(-1);
    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

TEST_F(TextBufferTest, MoveCursorBeyondBounds) {
    buffer->insertChar('H');
    buffer->moveCursor(10);                    // Try to move beyond buffer
    EXPECT_EQ(buffer->getCursorPosition(), 1); // Should stay at end
}

TEST_F(TextBufferTest, MoveCursorBeforeBounds) {
    buffer->insertChar('H');
    buffer->moveCursor(-10);                   // Try to move before start
    EXPECT_EQ(buffer->getCursorPosition(), 0); // Should clamp to start
}

// ============================================================================
// Complex Operations
// ============================================================================

TEST_F(TextBufferTest, InsertDeleteInsertSequence) {
    buffer->insertChar('H');
    buffer->insertChar('e');
    buffer->deleteChar();
    buffer->insertChar('i');
    EXPECT_EQ(buffer->getText(), "Hi");
}

TEST_F(TextBufferTest, ComplexTextEditing) {
    // Type "Hello World"
    const char *text = "Hello World";
    for (const char *p = text; *p; ++p) {
        buffer->insertChar(*p);
    }
    EXPECT_EQ(buffer->getText(), "Hello World");

    // Delete "World"
    buffer->deleteText(6, 5);
    EXPECT_EQ(buffer->getText(), "Hello ");

    // Add "C++"
    buffer->insertChar('C');
    buffer->insertChar('+');
    buffer->insertChar('+');
    EXPECT_EQ(buffer->getText(), "Hello C++");
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
