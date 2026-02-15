#include "WNebulaPresenter.hpp"
#include "WNebulaModel.hpp"
#include "WNebulaView.hpp"
#include <memory>
#include <spdlog/spdlog.h>
#include <utility>

namespace wnebula {

WNebulaPresenter::WNebulaPresenter() { spdlog::info("WNebulaPresenter created"); }

WNebulaPresenter::~WNebulaPresenter() { spdlog::default_logger()->flush(); }

void WNebulaPresenter::setup(const std::shared_ptr<WNebulaView> &newView, std::shared_ptr<WNebulaModel> newModel) {
    view = newView;
    model = std::move(newModel);
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
