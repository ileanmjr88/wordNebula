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

TEST_F(GapBufferTest, ConstructorInitializesEmpty) {
    EXPECT_EQ(buffer->getLength(), 0);
    EXPECT_EQ(buffer->getCursorPosition(), 0);
    EXPECT_EQ(buffer->getText(), "");
}

TEST_F(GapBufferTest, ConstructorWithCustomSize) {
    auto customBuffer = std::make_unique<GapBuffer>(100);
    EXPECT_EQ(customBuffer->getLength(), 0);
    EXPECT_EQ(customBuffer->getText(), "");
}

// ============================================================================
// Insert Tests
// ============================================================================

TEST_F(GapBufferTest, InsertSingleCharacter) {
    buffer->insertChar('H');
    EXPECT_EQ(buffer->getCursorPosition(), 1);
    EXPECT_EQ(buffer->getLength(), 1);
    EXPECT_EQ(buffer->getText(), "H");
}

TEST_F(GapBufferTest, InsertMultipleCharacters) {
    buffer->insertChar('H');
    buffer->insertChar('e');
    buffer->insertChar('l');
    buffer->insertChar('l');
    buffer->insertChar('o');
    EXPECT_EQ(buffer->getCursorPosition(), 5);
    EXPECT_EQ(buffer->getLength(), 5);
    EXPECT_EQ(buffer->getText(), "Hello");
}

TEST_F(GapBufferTest, InsertBeyondInitialCapacity) {
    // Insert more characters than initial size to trigger expandGap
    for (int i = 0; i < 20; ++i) {
        buffer->insertChar('a');
    }
    EXPECT_EQ(buffer->getCursorPosition(), 20);
    EXPECT_EQ(buffer->getLength(), 20);
}

TEST_F(GapBufferTest, InsertText) {
    buffer->insertText("Hello");
    EXPECT_EQ(buffer->getCursorPosition(), 5);
    EXPECT_EQ(buffer->getLength(), 5);
    EXPECT_EQ(buffer->getText(), "Hello");
}

// ============================================================================
// Delete Tests
// ============================================================================

TEST_F(GapBufferTest, DeleteSingleCharacter) {
    buffer->insertChar('H');
    buffer->insertChar('i');
    buffer->deleteChar();
    EXPECT_EQ(buffer->getCursorPosition(), 1);
    EXPECT_EQ(buffer->getText(), "H");
}

TEST_F(GapBufferTest, DeleteFromEmptyBuffer) {
    buffer->deleteChar();
    EXPECT_EQ(buffer->getCursorPosition(), 0);
    EXPECT_EQ(buffer->getLength(), 0);
}

TEST_F(GapBufferTest, DeleteMultipleCharacters) {
    buffer->insertChar('H');
    buffer->insertChar('e');
    buffer->insertChar('l');
    buffer->deleteChar();
    buffer->deleteChar();
    EXPECT_EQ(buffer->getCursorPosition(), 1);
    EXPECT_EQ(buffer->getText(), "H");
}

TEST_F(GapBufferTest, DeleteForward) {
    buffer->insertText("Hello");
    buffer->setCursorPosition(0);
    buffer->deleteForward();
    EXPECT_EQ(buffer->getText(), "ello");
    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

TEST_F(GapBufferTest, DeleteText) {
    buffer->insertText("Hello World");
    buffer->deleteText(5, 6);
    EXPECT_EQ(buffer->getText(), "Hello");
}

// ============================================================================
// Cursor Movement Tests
// ============================================================================

TEST_F(GapBufferTest, SetCursorPosition) {
    buffer->insertChar('H');
    buffer->insertChar('e');
    buffer->insertChar('l');
    buffer->setCursorPosition(0);
    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

TEST_F(GapBufferTest, MoveCursorByOffset) {
    buffer->insertChar('H');
    buffer->insertChar('e');
    buffer->moveCursor(-1);
    EXPECT_EQ(buffer->getCursorPosition(), 1);
}

TEST_F(GapBufferTest, MoveCursorClampedToStart) {
    buffer->insertChar('H');
    buffer->moveCursor(-10);
    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

TEST_F(GapBufferTest, MoveCursorClampedToEnd) {
    buffer->insertChar('H');
    buffer->insertChar('e');
    buffer->moveCursor(10);
    EXPECT_EQ(buffer->getCursorPosition(), 2);
}

// ============================================================================
// Complex Operations
// ============================================================================

TEST_F(GapBufferTest, InsertDeleteInsertSequence) {
    buffer->insertChar('H');
    buffer->insertChar('e');
    buffer->deleteChar();
    buffer->insertChar('i');
    EXPECT_EQ(buffer->getCursorPosition(), 2);
    EXPECT_EQ(buffer->getText(), "Hi");
}

TEST_F(GapBufferTest, InsertAtDifferentPositions) {
    buffer->insertText("Hello");

    // Move cursor to position 2 and insert
    buffer->setCursorPosition(2);
    buffer->insertChar('X');

    // Cursor should now be at position 3
    EXPECT_EQ(buffer->getCursorPosition(), 3);
    EXPECT_EQ(buffer->getText(), "HeXllo");
}

TEST_F(GapBufferTest, StressTestLargeInserts) {
    // Insert 1000 characters
    for (int i = 0; i < 1000; ++i) {
        buffer->insertChar(static_cast<char>('a' + (i % 26)));
    }
    EXPECT_EQ(buffer->getCursorPosition(), 1000);
    EXPECT_EQ(buffer->getLength(), 1000);
}

TEST_F(GapBufferTest, BufferGrowth) {
    // Fill beyond initial capacity (10 chars)
    for (int i = 0; i < 20; ++i) {
        buffer->insertChar('x');
    }

    // All characters should be present
    EXPECT_EQ(buffer->getLength(), 20);
    EXPECT_EQ(buffer->getText(), std::string(20, 'x'));
}

// ============================================================================
// Text Access Tests
// ============================================================================

TEST_F(GapBufferTest, GetTextRange) {
    buffer->insertText("Hello World");
    EXPECT_EQ(buffer->getTextRange(0, 5), "Hello");
    EXPECT_EQ(buffer->getTextRange(6, 5), "World");
}

TEST_F(GapBufferTest, GetTextAfterCursorMove) {
    buffer->insertText("Hello");
    buffer->setCursorPosition(2);
    // getText should still return full text regardless of cursor position
    EXPECT_EQ(buffer->getText(), "Hello");
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(GapBufferTest, MultipleDeletes) {
    buffer->insertChar('A');
    buffer->insertChar('B');
    buffer->insertChar('C');

    buffer->deleteChar();
    buffer->deleteChar();
    buffer->deleteChar();
    buffer->deleteChar(); // Extra delete on empty buffer

    EXPECT_EQ(buffer->getCursorPosition(), 0);
    EXPECT_EQ(buffer->getLength(), 0);
}

TEST_F(GapBufferTest, DeleteTextAdjustsCursor) {
    buffer->insertText("Hello World");
    // Cursor is at 11 (end)
    buffer->deleteText(5, 6); // Delete " World"
    EXPECT_EQ(buffer->getText(), "Hello");
    EXPECT_EQ(buffer->getCursorPosition(), 5);
}

TEST_F(GapBufferTest, DeleteTextCursorBeforeDeletion) {
    buffer->insertText("Hello World");
    buffer->setCursorPosition(3); // Cursor at 'l' in "Hello"
    buffer->deleteText(6, 5);     // Delete "World"
    EXPECT_EQ(buffer->getText(), "Hello ");
    EXPECT_EQ(buffer->getCursorPosition(), 3); // Cursor unchanged
}

TEST_F(GapBufferTest, DeleteTextCursorWithinDeletion) {
    buffer->insertText("Hello World");
    buffer->setCursorPosition(8); // Cursor within "World"
    buffer->deleteText(5, 6);     // Delete " World"
    EXPECT_EQ(buffer->getText(), "Hello");
    EXPECT_EQ(buffer->getCursorPosition(), 5); // Clamped to deletion point
}

TEST_F(GapBufferTest, DeleteTextOutOfBounds) {
    buffer->insertText("Hi");
    buffer->deleteText(0, 10);          // Length exceeds buffer
    EXPECT_EQ(buffer->getText(), "Hi"); // No change beyond buffer size
}

TEST_F(GapBufferTest, GetTextRangeOutOfBounds) {
    buffer->insertText("Hi");
    EXPECT_EQ(buffer->getTextRange(-1, 2), "");
    EXPECT_EQ(buffer->getTextRange(0, 10), "Hi"); // Clamps to available text
    EXPECT_EQ(buffer->getTextRange(5, 3), "");    // Start beyond buffer
}

TEST_F(GapBufferTest, DeleteForwardAtEnd) {
    buffer->insertText("Hi");
    buffer->deleteForward(); // Cursor at end, nothing to delete
    EXPECT_EQ(buffer->getText(), "Hi");
    EXPECT_EQ(buffer->getLength(), 2);
}

TEST_F(GapBufferTest, DeleteForwardFromEmpty) {
    buffer->deleteForward();
    EXPECT_EQ(buffer->getText(), "");
    EXPECT_EQ(buffer->getLength(), 0);
}

TEST_F(GapBufferTest, SetCursorPositionClampedToEnd) {
    buffer->insertText("Hi");
    buffer->setCursorPosition(100);
    EXPECT_EQ(buffer->getCursorPosition(), 2);
}

TEST_F(GapBufferTest, SetCursorPositionClampedToStart) {
    buffer->insertText("Hi");
    buffer->setCursorPosition(-5);
    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

TEST_F(GapBufferTest, AlternatingInsertDelete) {
    for (int i = 0; i < 10; ++i) {
        buffer->insertChar('X');
        if (i % 2 == 0) {
            buffer->deleteChar();
        }
    }
    EXPECT_EQ(buffer->getCursorPosition(), 5);
    EXPECT_EQ(buffer->getLength(), 5);
}

// ============================================================================
// Smart Navigation Tests
// ============================================================================

TEST_F(GapBufferTest, FindNextWordBoundary) {
    buffer->insertText("Hello World");
    EXPECT_EQ(buffer->findNextWordBoundary(0), 6);
}

TEST_F(GapBufferTest, FindPrevWordBoundary) {
    buffer->insertText("Hello World");
    EXPECT_EQ(buffer->findPrevWordBoundary(11), 6);
}

TEST_F(GapBufferTest, FindNextParagraph) {
    buffer->insertText("Line1\nLine2");
    EXPECT_EQ(buffer->findNextParagraph(0), 6);
}

TEST_F(GapBufferTest, FindPrevParagraph) {
    buffer->insertText("Line1\nLine2");
    EXPECT_EQ(buffer->findPrevParagraph(11), 6);
}

// ============================================================================
// Statistics Tests
// ============================================================================

TEST_F(GapBufferTest, WordCount) {
    buffer->insertText("Hello World");
    EXPECT_EQ(buffer->getWordCount(), 2);
}

TEST_F(GapBufferTest, WordCountEmpty) { EXPECT_EQ(buffer->getWordCount(), 0); }

TEST_F(GapBufferTest, ParagraphCount) {
    buffer->insertText("Line1\nLine2\nLine3");
    EXPECT_EQ(buffer->getParagraphCount(), 3);
}

TEST_F(GapBufferTest, ParagraphCountEmpty) { EXPECT_EQ(buffer->getParagraphCount(), 1); }

// ============================================================================
// Main
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
