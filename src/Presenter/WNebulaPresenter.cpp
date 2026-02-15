#include "WNebulaPresenter.hpp"
#include "WNebulaModel.hpp"
#include "WNebulaView.hpp"
#include <memory>
#include <spdlog/spdlog.h>
#include <utility>

namespace wnebula {

WNebulaPresenter::WNebulaPresenter() { spdlog::info("WNebulaPresenter created"); }

void WNebulaPresenter::setup(const std::shared_ptr<WNebulaView> &newView, std::shared_ptr<WNebulaModel> newModel) {
    view = newView;
    model = std::move(newModel);
    spdlog::info("View set for presenter");
}

void WNebulaPresenter::run() {
    while (isRunning) {
        if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
            v->processInput();
        } else {
            spdlog::error("WNebulaPresenter::run: View is no longer available, stopping run loop");
            break;
        }
    }
}

// Text Operations
void WNebulaPresenter::onInsert(char c) {
    model->insertChar(c);
    isDirty = true;
    if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
        v->render(model->getText(), model->getCursorPosition());
        spdlog::info("Rendered text");
    }
}

void WNebulaPresenter::onDelete() {
    model->deleteChar();
    isDirty = true;
    if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
        v->render(model->getText(), model->getCursorPosition());
    }
}

void WNebulaPresenter::onDeleteForward() {
    model->deleteForward();
    isDirty = true;
    if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
        v->render(model->getText(), model->getCursorPosition());
    }
}

// Single Character Navigation
void WNebulaPresenter::onMoveCursorLeft() {
    model->moveCursor(-1);
    if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
        v->render(model->getText(), model->getCursorPosition());
    }
}

void WNebulaPresenter::onMoveCursorRight() {
    model->moveCursor(1);
    if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
        v->render(model->getText(), model->getCursorPosition());
    }
}

void WNebulaPresenter::onMoveCursor(int offset) {
    model->moveCursor(offset);
    if (auto v = view.lock()) {
        v->render(model->getText(), model->getCursorPosition());
    }
}

// Smart Navigation
void WNebulaPresenter::onCtrlLeft() {
    const int pos = model->findPrevWordBoundary(model->getCursorPosition());
    model->setCursorPosition(pos);
    if (auto v = view.lock()) {
        v->render(model->getText(), model->getCursorPosition());
    }
}

void WNebulaPresenter::onCtrlRight() {
    const int pos = model->findNextWordBoundary(model->getCursorPosition());
    model->setCursorPosition(pos);
    if (auto v = view.lock()) {
        v->render(model->getText(), model->getCursorPosition());
    }
}

void WNebulaPresenter::onCtrlUp() {
    const int pos = model->findPrevParagraph(model->getCursorPosition());
    model->setCursorPosition(pos);
    if (auto v = view.lock()) {
        v->render(model->getText(), model->getCursorPosition());
    }
}

void WNebulaPresenter::onCtrlDown() {
    const int pos = model->findNextParagraph(model->getCursorPosition());
    model->setCursorPosition(pos);
    if (auto v = view.lock()) {
        v->render(model->getText(), model->getCursorPosition());
    }
}

void WNebulaPresenter::onHome() {
    model->setCursorPosition(0);
    if (auto v = view.lock()) {
        v->render(model->getText(), model->getCursorPosition());
    }
}

void WNebulaPresenter::onEnd() {
    model->setCursorPosition(model->getLength());
    if (auto v = view.lock()) {
        v->render(model->getText(), model->getCursorPosition());
    }
}

// Application Control
void WNebulaPresenter::onExit() {
    if (isDirty) {
        spdlog::warn("File not saved!");
    }

    // TODO: Warn the user.

    isRunning = false;
}

// File I/O
void WNebulaPresenter::saveFile(const std::string &path) { isDirty = false; }

void WNebulaPresenter::loadFile(const std::string &path) {
    currentFilePath = path;
    isDirty = false;
}

[[nodiscard]] bool WNebulaPresenter::getIsDirty() const { return isDirty; }

} // namespace wnebula
