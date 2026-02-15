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
    virtual ~WNebulaView();

    WNebulaView(const WNebulaView &) = delete;
    WNebulaView &operator=(const WNebulaView &) = delete;
    WNebulaView(WNebulaView &&) = delete;
    WNebulaView &operator=(WNebulaView &&) = delete;

    virtual void render(const std::string &text, int cursorPosition);
    virtual void processInput();

  protected:
    // Protected constructor for testing - allows skipping ncurses init
    WNebulaView(const std::shared_ptr<WNebulaPresenter> &presenter, bool initNcurses);

  private:
    std::weak_ptr<WNebulaPresenter> presenter;
};
} // namespace wnebula
