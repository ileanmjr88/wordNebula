#pragma once
#include <array>
#include <string_view>
#include <utility>

namespace wnebula {

struct ShortcutEntry {
    std::string_view key;
    std::string_view description;
};

constexpr std::array KEYBOARD_SHORTCUTS = {
    ShortcutEntry{"Ctrl+S", "Save"},
    ShortcutEntry{"Ctrl+O", "Open"},
    ShortcutEntry{"Ctrl+Q", "Quit"},
    ShortcutEntry{"F1", "Toggle Help"},
    ShortcutEntry{"Ctrl+←/→", "Jump Word"},
    ShortcutEntry{"Ctrl+↑/↓", "Jump Paragraph"},
    ShortcutEntry{"Home/End", "Start/End of Line"},
};

} // namespace wnebula
