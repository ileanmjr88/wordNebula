#include "View/FtxuiView.hpp"
#include "View/KeyboardShorcuts.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/terminal.hpp>
#include <memory>
#include <string>

namespace wnebula {

FtxuiView::FtxuiView() : screen(ftxui::ScreenInteractive::Fullscreen()) {}

FtxuiView::~FtxuiView() = default;

void FtxuiView::run(std::function<void(const InputEvent &)> onInput) {
    inputCallback = onInput;
    mainComponent = createMainComponent();
    screen.Loop(mainComponent);
}

void FtxuiView::render(const ViewState &state) {
    currentState = state;
    temporaryMessage.clear();
    temporaryMessageIsError = false;
    screen.PostEvent(ftxui::Event::Custom);
}

void FtxuiView::exit() { screen.Exit(); }

std::pair<int, int> FtxuiView::getTerminalSize() const {
    auto size = ftxui::Terminal::Size();
    return {size.dimx, size.dimy};
}

void FtxuiView::showMessage(const std::string &message, bool isError) {
    temporaryMessage = message;
    temporaryMessageIsError = isError;
    screen.PostEvent(ftxui::Event::Custom);
}

// Private functions

ftxui::Component FtxuiView::createMainComponent() {
    auto renderer = ftxui::Renderer([this] {
        return ftxui::vbox({
            renderStatusMessage(), // temporary messages like saving or warnings
            ftxui::dbox({
                renderEditor() | ftxui::flex, // text area (back layer)
                renderHelpOverlay(),          // help overlay (front layer)
            }) | ftxui::flex,
            renderStatusBar(), // filename, word count at bottom
        });
    });

    return ftxui::CatchEvent(renderer, [this](ftxui::Event event) {
        if (event == ftxui::Event::Custom)
            return false; // let redraw events pass through
        InputEvent ie = translateEvent(event);
        if (ie.type != InputEvent::Type::UNKNOWN && inputCallback) {
            inputCallback(ie);
            return true; // we handled it
        }
        return false; // let FTXUI handle it
    });
}

ftxui::Element FtxuiView::renderEditor() {
    const auto &text = currentState.visibleText;
    const size_t pos = static_cast<size_t>(currentState.cursorPosition);

    auto before = ftxui::text(text.substr(0, pos));

    // Character at cursor with cyan background
    std::string cursorChar = pos < text.size() ? std::string(1, text[pos]) : " ";
    auto cursor = ftxui::text(cursorChar) | ftxui::bgcolor(ftxui::Color::Cyan) | ftxui::color(ftxui::Color::Black);

    auto after = pos + 1 < text.size() ? ftxui::text(text.substr(pos + 1)) : ftxui::text("");

    return ftxui::hbox({before, cursor, after});
}

ftxui::Element FtxuiView::renderStatusBar() {
    std::string name = currentState.filename.empty() ? "Untitled" : currentState.filename;
    if (currentState.isDirty) {
        name = "~ " + name;
    }

    std::string words = "Words: " + std::to_string(currentState.wordCount);

    return ftxui::hbox({
               ftxui::text(name),
               ftxui::filler(),
               ftxui::text(words),
           }) |
           ftxui::bgcolor(ftxui::Color::GrayDark) | ftxui::color(ftxui::Color::White);
}

ftxui::Element FtxuiView::renderStatusMessage() {
    std::string message = temporaryMessage.empty() ? currentState.statusMessage : temporaryMessage;

    if (message.empty()) {
        return ftxui::emptyElement();
    }

    auto style = temporaryMessageIsError ? ftxui::bgcolor(ftxui::Color::Red) | ftxui::color(ftxui::Color::White)
                                         : ftxui::bgcolor(ftxui::Color::Blue) | ftxui::color(ftxui::Color::White);

    return ftxui::hbox({
               ftxui::text(message),
               ftxui::filler(),
           }) |
           style;
}

ftxui::Element FtxuiView::renderHelpOverlay() {
    if (!currentState.showHelp) {
        return ftxui::emptyElement();
    }

    ftxui::Elements lines;
    lines.push_back(ftxui::text("Keyboard Shortcuts") | ftxui::bold);
    lines.push_back(ftxui::separator());
    for (const auto &shortcut : KEYBOARD_SHORTCUTS) {
        lines.push_back(ftxui::hbox({
            ftxui::text(std::string(shortcut.key)),
            ftxui::filler(),
            ftxui::text(std::string(shortcut.description)),
        }));
    }

    return ftxui::vbox(lines) | ftxui::border | ftxui::bgcolor(ftxui::Color::Black) | ftxui::center;
}

InputEvent FtxuiView::translateEvent(const ftxui::Event &event) {
    // Navigation
    if (event == ftxui::Event::ArrowLeft)
        return {InputEvent::Type::ARROW_LEFT};
    if (event == ftxui::Event::ArrowRight)
        return {InputEvent::Type::ARROW_RIGHT};
    if (event == ftxui::Event::ArrowUp)
        return {InputEvent::Type::ARROW_UP};
    if (event == ftxui::Event::ArrowDown)
        return {InputEvent::Type::ARROW_DOWN};

    // Ctrl+Arrow
    if (event == ftxui::Event::ArrowLeftCtrl)
        return {InputEvent::Type::CTRL_LEFT};
    if (event == ftxui::Event::ArrowRightCtrl)
        return {InputEvent::Type::CTRL_RIGHT};
    if (event == ftxui::Event::ArrowUpCtrl)
        return {InputEvent::Type::CTRL_UP};
    if (event == ftxui::Event::ArrowDownCtrl)
        return {InputEvent::Type::CTRL_DOWN};

    // Editing
    if (event == ftxui::Event::Backspace)
        return {InputEvent::Type::BACKSPACE};
    if (event == ftxui::Event::Delete)
        return {InputEvent::Type::DELETE};
    if (event == ftxui::Event::Return)
        return {InputEvent::Type::ENTER};

    // Navigation keys
    if (event == ftxui::Event::Home)
        return {InputEvent::Type::HOME};
    if (event == ftxui::Event::End)
        return {InputEvent::Type::END};
    if (event == ftxui::Event::PageUp)
        return {InputEvent::Type::PAGE_UP};
    if (event == ftxui::Event::PageDown)
        return {InputEvent::Type::PAGE_DOWN};

    // Ctrl+Key commands
    if (event == ftxui::Event::CtrlS)
        return {InputEvent::Type::CTRL_S};
    if (event == ftxui::Event::CtrlO)
        return {InputEvent::Type::CTRL_O};
    if (event == ftxui::Event::CtrlQ)
        return {InputEvent::Type::CTRL_Q};
    if (event == ftxui::Event::CtrlW)
        return {InputEvent::Type::CTRL_W};
    if (event == ftxui::Event::F1)
        return {InputEvent::Type::F1};
    if (event == ftxui::Event::Escape)
        return {InputEvent::Type::ESCAPE};

    // Printable character
    if (event.is_character()) {
        InputEvent ie{InputEvent::Type::CHARACTER};
        ie.character = event.character()[0];
        return ie;
    }

    return {InputEvent::Type::UNKNOWN};
}

} // namespace wnebula
