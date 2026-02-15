#pragma once

#include <memory>
#include <spdlog/spdlog.h>

#include "WNebulaModel.hpp"
#include "WNebulaView.hpp"

namespace wnebula {

class WNebulaPresenter {
  public:
    WNebulaPresenter();
    ~WNebulaPresenter();

    WNebulaPresenter(const WNebulaPresenter &) = delete;
    WNebulaPresenter &operator=(const WNebulaPresenter &) = delete;
    WNebulaPresenter(WNebulaPresenter &&) = delete;
    WNebulaPresenter &operator=(WNebulaPresenter &&) = delete;

    void onInsert(char c);
    void onDelete();
    void onMoveCursor(int delta);
    void onExit();
    void setup(const std::shared_ptr<WNebulaView> &newView, std::shared_ptr<WNebulaModel> newModel);

    void run();

  private:
    std::weak_ptr<WNebulaView> view; // Use weak_ptr to avoid circular dependency
    std::shared_ptr<WNebulaModel> model;
    bool isRunning = true;
};
} // namespace wnebula
