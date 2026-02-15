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
    [[nodiscard]] std::string getText() const override;
    [[nodiscard]] std::string getTextRange(int start, int length) const override;
    [[nodiscard]] int getLength() const override;

    // Cursor Management
    [[nodiscard]] int getCursorPosition() const override;
    void setCursorPosition(int position) override;
    void moveCursor(int offset) override;

    // Smart Navigation
    [[nodiscard]] int findNextWordBoundary(int fromPos) const override;
    [[nodiscard]] int findPrevWordBoundary(int fromPos) const override;
    [[nodiscard]] int findNextParagraph(int fromPos) const override;
    [[nodiscard]] int findPrevParagraph(int fromPos) const override;

    // Statistics
    [[nodiscard]] int getWordCount() const override;
    [[nodiscard]] int getParagraphCount() const override;

  private:
    std::string buffer;
    int currentCursor = 0;
};

} // namespace wnebula
