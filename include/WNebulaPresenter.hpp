#ifndef WNEBULAPRESENTER_HPP
#define WNEBULAPRESENTER_HPP
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
    void onInsert(char c);
    void onDelete();
    void onMoveCursor(int delta);
    void onExit();
    void setup(std::shared_ptr<WNebulaView> view, std::shared_ptr<WNebulaModel> model);

    void run();

  private:
    std::weak_ptr<WNebulaView> view; // Use weak_ptr to avoid circular dependency
    std::shared_ptr<WNebulaModel> model;
    bool isRunning = true;
};
} // namespace wnebula

#endif // WNEBULAPRESENTER_HPP