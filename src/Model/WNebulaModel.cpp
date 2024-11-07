#include "WNebulaModel.hpp"

namespace wnebula {

WNebulaModel::WNebulaModel() : buffer(std::make_shared<TextBuffer>()) {}

WNebulaModel::WNebulaModel(std::shared_ptr<TextBuffer> textBuffer) : buffer(textBuffer) {}

void WNebulaModel::insert(char c) {
    buffer->insertChar(c);
}

void WNebulaModel::insertChar(char c) {
    buffer->insertChar(c);
}

void WNebulaModel::deleteChar() {
    buffer->deleteChar();
}

void WNebulaModel::deleteText(int position, int length) {
    buffer->deleteText(position, length);
}

void WNebulaModel::moveCursor(int offset) {
    buffer->moveCursor(offset);
}

std::string WNebulaModel::getText() const {
    return buffer->getText();
}

int WNebulaModel::getCursorPosition() const {
    return buffer->getCursorPosition();
}

} // namespace wnebula