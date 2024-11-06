#include "WNebulaView.hpp"
#include "WNebulaPresenter.hpp"

namespace wnebula {

WNebulaView::WNebulaView(std::shared_ptr<WNebulaPresenter> presenter) : presenter(presenter) {
    this->presenter = std::weak_ptr<WNebulaPresenter>(presenter);
    auto ptr = initscr();
    if (ptr == nullptr) {
        spdlog::error("Failed to initialize ncurses");  
        throw std::runtime_error("Failed to initialize ncurses");
    }
    echo();
    keypad(stdscr, TRUE);
    spdlog::info("WNebulaView initialized");
}

WNebulaView::~WNebulaView() {
    endwin();
    spdlog::default_logger()->flush();  
}

void WNebulaView::render(const std::string &text, int cursorPosition) {
    clear();
    mvprintw(0,0, "%s",text.c_str());
    move(0, cursorPosition);
    refresh();
    spdlog::info("Rendered text: {}", text);
}

void WNebulaView::processInput() {
    int ch = getch();
    auto presenterPtr = presenter.lock();
    switch (ch) {
        case KEY_BACKSPACE:
            presenterPtr->onDelete();
            break;
        case KEY_LEFT:
            presenterPtr->onMoveCursor(-1);
            break;
        case KEY_RIGHT:
            presenterPtr->onMoveCursor(1);
            break;
        case KEY_UP:
            presenterPtr->onMoveCursor(-10);
            break;
        case KEY_DOWN:
            presenterPtr->onMoveCursor(10);
            break;
        case 27: // ESC key
            presenterPtr->onExit();
            break;
        default:
            presenterPtr->onInsert(static_cast<char>(ch));
            break;
    }
    spdlog::info("Processed input: {}", ch);

}
}// namespace wnebula