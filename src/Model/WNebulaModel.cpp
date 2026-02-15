#include "WNebulaModel.hpp"
#include "Model/IBuffer.hpp"
#include "Model/TextBuffer.hpp"
#include <memory>
#include <string>
#include <utility>

namespace wnebula {

WNebulaModel::WNebulaModel() : buffer(std::make_shared<TextBuffer>()) {}

WNebulaModel::WNebulaModel(std::shared_ptr<IBuffer> buf) : buffer(std::move(buf)) {}

// Text Operations
void WNebulaModel::insertChar(char c) { buffer->insertChar(c); }
void WNebulaModel::insertText(const std::string &text) { buffer->insertText(text); }
void WNebulaModel::deleteChar() { buffer->deleteChar(); }
void WNebulaModel::deleteForward() { buffer->deleteForward(); }
void WNebulaModel::deleteText(int position, int length) { buffer->deleteText(position, length); }

// Text Access
std::string WNebulaModel::getText() const { return buffer->getText(); }
std::string WNebulaModel::getTextRange(int start, int length) const { return buffer->getTextRange(start, length); }
int WNebulaModel::getLength() const { return buffer->getLength(); }

// Cursor Management
int WNebulaModel::getCursorPosition() const { return buffer->getCursorPosition(); }
void WNebulaModel::setCursorPosition(int position) { buffer->setCursorPosition(position); }
void WNebulaModel::moveCursor(int offset) { buffer->moveCursor(offset); }

// Smart Navigation
int WNebulaModel::findNextWordBoundary(int fromPos) const { return buffer->findNextWordBoundary(fromPos); }
int WNebulaModel::findPrevWordBoundary(int fromPos) const { return buffer->findPrevWordBoundary(fromPos); }
int WNebulaModel::findNextParagraph(int fromPos) const { return buffer->findNextParagraph(fromPos); }
int WNebulaModel::findPrevParagraph(int fromPos) const { return buffer->findPrevParagraph(fromPos); }

// Statistics
int WNebulaModel::getWordCount() const { return buffer->getWordCount(); }
int WNebulaModel::getParagraphCount() const { return buffer->getParagraphCount(); }

} // namespace wnebula
