#include "Presenter/WNebulaPresenter.hpp"
#include "Model/WNebulaModel.hpp"
#include "View/WNebulaView.hpp"
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

// Private Helper
void WNebulaPresenter::updateView() {
    if (auto v = view.lock()) {
        v->render(model->getText(), model->getCursorPosition());
    }
}

// Text Operations
void WNebulaPresenter::onInsert(char c) {
    model->insertChar(c);
    isDirty = true;
    updateView();
}

void WNebulaPresenter::onDelete() {
    model->deleteChar();
    isDirty = true;
    updateView();
}

void WNebulaPresenter::onDeleteForward() {
    model->deleteForward();
    isDirty = true;
    updateView();
}

// Single Character Navigation
void WNebulaPresenter::onMoveCursorLeft() {
    model->moveCursor(-1);
    updateView();
}

void WNebulaPresenter::onMoveCursorRight() {
    model->moveCursor(1);
    updateView();
}

void WNebulaPresenter::onMoveCursor(int offset) {
    model->moveCursor(offset);
    updateView();
}

// Smart Navigation
void WNebulaPresenter::onCtrlLeft() {
    const int pos = model->findPrevWordBoundary(model->getCursorPosition());
    model->setCursorPosition(pos);
    updateView();
}

void WNebulaPresenter::onCtrlRight() {
    const int pos = model->findNextWordBoundary(model->getCursorPosition());
    model->setCursorPosition(pos);
    updateView();
}

void WNebulaPresenter::onCtrlUp() {
    const int pos = model->findPrevParagraph(model->getCursorPosition());
    model->setCursorPosition(pos);
    updateView();
}

void WNebulaPresenter::onCtrlDown() {
    const int pos = model->findNextParagraph(model->getCursorPosition());
    model->setCursorPosition(pos);
    updateView();
}

void WNebulaPresenter::onHome() {
    model->setCursorPosition(0);
    updateView();
}

void WNebulaPresenter::onEnd() {
    model->setCursorPosition(model->getLength());
    updateView();
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
