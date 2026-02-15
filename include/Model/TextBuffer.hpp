#pragma once

#include "Model/IBuffer.hpp"
#include <string>

namespace wnebula {

class TextBuffer : public IBuffer {
  public:
    TextBuffer();
    ~TextBuffer() override = default;

    // Text Operations
    void insertChar(char c) override;
    void insertText(const std::string &text) override;
    void deleteChar() override;
    void deleteForward() override;
    void deleteText(int position, int length) override;

    // Text Access
    std::string getText() const override;
    std::string getTextRange(int start, int length) const override;
    int getLength() const override;

    // Cursor Management
    int getCursorPosition() const override;
    void setCursorPosition(int position) override;
    void moveCursor(int offset) override;

    // Smart Navigation
    int findNextWordBoundary(int fromPos) const override;
    int findPrevWordBoundary(int fromPos) const override;
    int findNextParagraph(int fromPos) const override;
    int findPrevParagraph(int fromPos) const override;

    // Statistics
    int getWordCount() const override;
    int getParagraphCount() const override;

  private:
    std::string buffer;
    int currentCursor = 0;
};

} // namespace wnebula
