#include "WNebulaPresenter.hpp"
#include "WNebulaView.hpp"

namespace wnebula {

WNebulaPresenter::WNebulaPresenter() 
    : model(std::make_shared<TextBuffer>()) {}

void WNebulaPresenter::setView(std::shared_ptr<WNebulaView> view) {
    this->view = view;
}

void WNebulaPresenter::onInsert(char c) {
    model->insertChar(c);
    if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
        v->render(model->getText(), model->getCursorPosition());
    }
}

void WNebulaPresenter::onDelete() {
    model->deleteChar();
    if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
        v->render(model->getText(), model->getCursorPosition());
    }
}

void WNebulaPresenter::onMoveCursor(int delta) {
    model->moveCursor(delta);
    if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
        v->render(model->getText(), model->getCursorPosition());
    }
}

void WNebulaPresenter::onExit() {
    isRunning = false;
}

void WNebulaPresenter::run() {
    while (isRunning) {
        if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
            v->processInput();
        }
    }
}

} // namespace wnebula