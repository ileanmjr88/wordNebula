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
    EXPECT_EQ(buffer->getLength(), 0);
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

TEST_F(TextBufferTest, InsertText) {
    buffer->insertText("Hello");
    EXPECT_EQ(buffer->getText(), "Hello");
    EXPECT_EQ(buffer->getCursorPosition(), 5);
    EXPECT_EQ(buffer->getLength(), 5);
}

TEST_F(TextBufferTest, InsertTextAtMiddle) {
    buffer->insertText("Hd");
    buffer->setCursorPosition(1);
    buffer->insertText("ello Worl");
    EXPECT_EQ(buffer->getText(), "Hello World");
}

TEST_F(TextBufferTest, InsertTextEmpty) {
    buffer->insertText("");
    EXPECT_EQ(buffer->getText(), "");
    EXPECT_EQ(buffer->getCursorPosition(), 0);
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

TEST_F(TextBufferTest, DeleteForward) {
    buffer->insertText("Hello");
    buffer->setCursorPosition(0);
    buffer->deleteForward();
    EXPECT_EQ(buffer->getText(), "ello");
    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

TEST_F(TextBufferTest, DeleteForwardAtEnd) {
    buffer->insertText("Hi");
    buffer->deleteForward(); // Cursor at end, nothing to delete
    EXPECT_EQ(buffer->getText(), "Hi");
}

TEST_F(TextBufferTest, DeleteForwardFromEmpty) {
    buffer->deleteForward();
    EXPECT_EQ(buffer->getText(), "");
    EXPECT_EQ(buffer->getLength(), 0);
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

TEST_F(TextBufferTest, DeleteTextAdjustsCursor) {
    buffer->insertText("Hello World");
    // Cursor is at 11 (end)
    buffer->deleteText(5, 6); // Delete " World"
    EXPECT_EQ(buffer->getText(), "Hello");
    EXPECT_EQ(buffer->getCursorPosition(), 5);
}

TEST_F(TextBufferTest, DeleteTextOutOfBounds) {
    buffer->insertText("Hi");
    buffer->deleteText(0, 10);          // Length exceeds buffer
    EXPECT_EQ(buffer->getText(), "Hi"); // No change
}

// ============================================================================
// Text Access Tests
// ============================================================================

TEST_F(TextBufferTest, GetLength) {
    EXPECT_EQ(buffer->getLength(), 0);
    buffer->insertText("Hello");
    EXPECT_EQ(buffer->getLength(), 5);
}

TEST_F(TextBufferTest, GetTextRange) {
    buffer->insertText("Hello World");
    EXPECT_EQ(buffer->getTextRange(0, 5), "Hello");
    EXPECT_EQ(buffer->getTextRange(6, 5), "World");
}

TEST_F(TextBufferTest, GetTextRangeOutOfBounds) {
    buffer->insertText("Hi");
    EXPECT_EQ(buffer->getTextRange(-1, 2), "");
    EXPECT_EQ(buffer->getTextRange(0, 10), "Hi"); // Clamps to available text
    EXPECT_EQ(buffer->getTextRange(5, 3), "");    // Start beyond buffer
}

TEST_F(TextBufferTest, GetTextRangeFullBuffer) {
    buffer->insertText("Hello");
    EXPECT_EQ(buffer->getTextRange(0, 5), "Hello");
}

// ============================================================================
// Cursor Management Tests
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

TEST_F(TextBufferTest, SetCursorPosition) {
    buffer->insertText("Hello");
    buffer->setCursorPosition(2);
    EXPECT_EQ(buffer->getCursorPosition(), 2);
}

TEST_F(TextBufferTest, SetCursorPositionClampedToEnd) {
    buffer->insertText("Hi");
    buffer->setCursorPosition(100);
    EXPECT_EQ(buffer->getCursorPosition(), 2);
}

TEST_F(TextBufferTest, SetCursorPositionClampedToStart) {
    buffer->insertText("Hi");
    buffer->setCursorPosition(-5);
    EXPECT_EQ(buffer->getCursorPosition(), 0);
}

// ============================================================================
// Smart Navigation Tests
// ============================================================================

TEST_F(TextBufferTest, FindNextWordBoundary) {
    buffer->insertText("Hello World");
    EXPECT_EQ(buffer->findNextWordBoundary(0), 6);
}

TEST_F(TextBufferTest, FindNextWordBoundaryAtEnd) {
    buffer->insertText("Hello");
    EXPECT_EQ(buffer->findNextWordBoundary(0), 5);
}

TEST_F(TextBufferTest, FindNextWordBoundaryMultipleSpaces) {
    buffer->insertText("Hello   World");
    EXPECT_EQ(buffer->findNextWordBoundary(0), 8);
}

TEST_F(TextBufferTest, FindPrevWordBoundary) {
    buffer->insertText("Hello World");
    EXPECT_EQ(buffer->findPrevWordBoundary(11), 6);
}

TEST_F(TextBufferTest, FindPrevWordBoundaryAtStart) {
    buffer->insertText("Hello");
    EXPECT_EQ(buffer->findPrevWordBoundary(3), 0);
}

TEST_F(TextBufferTest, FindNextParagraph) {
    buffer->insertText("Line1\nLine2");
    EXPECT_EQ(buffer->findNextParagraph(0), 6);
}

TEST_F(TextBufferTest, FindNextParagraphNoNewline) {
    buffer->insertText("Hello");
    EXPECT_EQ(buffer->findNextParagraph(0), 5);
}

TEST_F(TextBufferTest, FindPrevParagraph) {
    buffer->insertText("Line1\nLine2");
    EXPECT_EQ(buffer->findPrevParagraph(11), 6);
}

TEST_F(TextBufferTest, FindPrevParagraphAtStart) {
    buffer->insertText("Hello");
    EXPECT_EQ(buffer->findPrevParagraph(3), 0);
}

// ============================================================================
// Statistics Tests
// ============================================================================

TEST_F(TextBufferTest, WordCount) {
    buffer->insertText("Hello World");
    EXPECT_EQ(buffer->getWordCount(), 2);
}

TEST_F(TextBufferTest, WordCountEmpty) { EXPECT_EQ(buffer->getWordCount(), 0); }

TEST_F(TextBufferTest, WordCountMultipleSpaces) {
    buffer->insertText("  Hello   World  ");
    EXPECT_EQ(buffer->getWordCount(), 2);
}

TEST_F(TextBufferTest, ParagraphCount) {
    buffer->insertText("Line1\nLine2\nLine3");
    EXPECT_EQ(buffer->getParagraphCount(), 3);
}

TEST_F(TextBufferTest, ParagraphCountEmpty) { EXPECT_EQ(buffer->getParagraphCount(), 1); }

TEST_F(TextBufferTest, ParagraphCountSingleLine) {
    buffer->insertText("Hello");
    EXPECT_EQ(buffer->getParagraphCount(), 1);
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
    buffer->insertText("Hello World");
    EXPECT_EQ(buffer->getText(), "Hello World");

    // Delete "World"
    buffer->deleteText(6, 5);
    EXPECT_EQ(buffer->getText(), "Hello ");

    // Add "C++"
    buffer->insertText("C++");
    EXPECT_EQ(buffer->getText(), "Hello C++");
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
