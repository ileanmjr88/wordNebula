#pragma once

#include <ncurses.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

namespace wnebula {
class WNebulaPresenter; // Forward declaration

class WNebulaView {
  public:
    explicit WNebulaView(const std::shared_ptr<WNebulaPresenter> &presenter);
    ~WNebulaView();

    WNebulaView(const WNebulaView &) = delete;
    WNebulaView &operator=(const WNebulaView &) = delete;
    WNebulaView(WNebulaView &&) = delete;
    WNebulaView &operator=(WNebulaView &&) = delete;

    void render(const std::string &text, int cursorPosition);
    void processInput();

  private:
    std::weak_ptr<WNebulaPresenter> presenter;
};
} // namespace wnebula
