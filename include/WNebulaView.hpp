#ifndef WNEBULAVIEW_HPP
#define WNEBULAVIEW_HPP
#pragma once

#include <ncurses.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

namespace wnebula {
class WNebulaPresenter; // Forward declaration

class WNebulaView {
  public:
    WNebulaView(std::shared_ptr<WNebulaPresenter> presenter);
    ~WNebulaView();

    void render(const std::string &text, int cursorPosition);
    void processInput();

  private:
    std::weak_ptr<WNebulaPresenter> presenter;
};
} // namespace wnebula

#endif // WNEBULAVIEW_HPP