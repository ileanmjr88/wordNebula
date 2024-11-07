#include "WNebulaPresenter.hpp"

#include "WNebulaView.hpp"

namespace wnebula {

WNebulaPresenter::WNebulaPresenter() { spdlog::info("WNebulaPresenter created"); }

WNebulaPresenter::~WNebulaPresenter() { spdlog::default_logger()->flush(); }

void WNebulaPresenter::setup(std::shared_ptr<WNebulaView> view, std::shared_ptr<WNebulaModel> model) {
    this->view = view;
    this->model = model;
    spdlog::info("View set for presenter");
}

void WNebulaPresenter::onInsert(char c) {
    model->insertChar(c);
    if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
        v->render(model->getText(), model->getCursorPosition());
        spdlog::info("Rendered text");
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

void WNebulaPresenter::onExit() { isRunning = false; }

void WNebulaPresenter::run() {
    while (isRunning) {
        if (auto v = view.lock()) { // Convert weak_ptr to shared_ptr
            v->processInput();
        }
    }
}

} // namespace wnebula