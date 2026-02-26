#include "Presenter/WNebulaPresenter.hpp"
#include "Model/WNebulaModel.hpp"
#include "View/IView.hpp"
#include <memory>
#include <spdlog/spdlog.h>
#include <utility>

namespace wnebula {

WNebulaPresenter::WNebulaPresenter() { spdlog::info("WNebulaPresenter created"); }

void WNebulaPresenter::setup(const std::shared_ptr<IView> &newView, std::shared_ptr<WNebulaModel> newModel) {
    view = newView;
    model = std::move(newModel);
    spdlog::info("View set for presenter");
}

void WNebulaPresenter::run() {
    while (isRunning) {
        if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
            v->run([this](const InputEvent &event) { handleInput(event); });
        } else {
            spdlog::error("WNebulaPresenter::run: View is no longer available, stopping run loop");
            break;
        }
    }
}

// Private Helper
void WNebulaPresenter::updateView() {
    if (auto v = view.lock()) {
        ViewState state{};
        state.visibleText = model->getText();
        state.cursorPosition = model->getCursorPosition();
        state.wordCount = model->getWordCount();
        state.filename = currentFilePath.empty() ? "Untitled" : currentFilePath;
        state.isDirty = isDirty;
        state.showHelp = showHelp;
        v->render(state);
    }
}

void WNebulaPresenter::handleInput(const InputEvent &event) {
    if (event.type != InputEvent::Type::CTRL_Q && event.type != InputEvent::Type::ESCAPE) {
        exitWarningShown = false;
    }

    switch (event.type) {
    case InputEvent::Type::CHARACTER:
        onInsert(event.character);
        break;
    case InputEvent::Type::BACKSPACE:
        onDelete();
        break;
    case InputEvent::Type::DELETE:
        onDeleteForward();
        break;
    case InputEvent::Type::ENTER:
        onInsert('\n');
        break;
    case InputEvent::Type::ARROW_LEFT:
        onMoveCursorLeft();
        break;
    case InputEvent::Type::ARROW_RIGHT:
        onMoveCursorRight();
        break;
    case InputEvent::Type::CTRL_LEFT:
        onCtrlLeft();
        break;
    case InputEvent::Type::CTRL_RIGHT:
        onCtrlRight();
        break;
    case InputEvent::Type::CTRL_UP:
        onCtrlUp();
        break;
    case InputEvent::Type::CTRL_DOWN:
        onCtrlDown();
        break;
    case InputEvent::Type::HOME:
        onHome();
        break;
    case InputEvent::Type::END:
        onEnd();
        break;
    case InputEvent::Type::CTRL_S:
        saveFile(currentFilePath);
        break;
    case InputEvent::Type::F1:
        onToggleHelp();
        break;
    case InputEvent::Type::CTRL_Q: // fall through
    case InputEvent::Type::ESCAPE:
        onExit();
        break;
    default:
        break;
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
    if (isDirty && !exitWarningShown) {
        exitWarningShown = true;
        if (auto v = view.lock()) {
            v->showMessage("Unsaved changes! Press Ctrl+Q again to quit.", true);
        }
        return;
    }
    isRunning = false;
    if (auto v = view.lock()) {
        v->exit();
    }
}

void WNebulaPresenter::onToggleHelp() {
    showHelp = !showHelp;
    updateView();
}

// File I/O
void WNebulaPresenter::saveFile([[maybe_unused]] const std::string &path) { isDirty = false; }

void WNebulaPresenter::loadFile(const std::string &path) {
    currentFilePath = path;
    isDirty = false;
}

[[nodiscard]] bool WNebulaPresenter::getIsDirty() const { return isDirty; }

} // namespace wnebula
