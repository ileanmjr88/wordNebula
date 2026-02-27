# View Implementation Design

**Version**: 1.0 **Last Updated**: 2026-02-16 **Status**: Implemented –
Historical design snapshot; implementation complete in `src/View/FtxuiView.cpp`

______________________________________________________________________

## Overview

This document captures the design decisions and implementation strategy for
wordNebula's View layer, specifically the transition from ncurses to FTXUI and
the implementation of the `FtxuiView` class.

______________________________________________________________________

## Library Selection: FTXUI

### Decision

**Chosen**: FTXUI (v6.1.9) over ncurses, cpp-terminal, and GGUI

### Rationale

**Why FTXUI over ncurses:**

- Modern C++17 (matches codebase standards)
- Component-based architecture (fits MVP pattern)
- Cross-platform (Linux, macOS, Windows) - aligns with platform support goals
- Active maintenance and good documentation
- Handles layout, events, and modern UI automatically

**What ncurses would require that FTXUI provides:**

- ✅ Custom layout system → FTXUI has automatic layout
- ✅ Component composition → FTXUI has Component system
- ✅ Event loop management → FTXUI handles this
- ✅ Modern UI styling → FTXUI provides rich styling API
- ✅ Window management → FTXUI abstracts this

**Cursor rendering complexity:**

- Exists with ANY terminal UI library (not unique to FTXUI)
- FTXUI provides the tools (styling, colors, text composition)
- Implementation effort is comparable across all libraries

**GPU rendering note:**

- Terminal UIs are inherently CPU-based
- GPU acceleration happens at the terminal emulator level (iTerm2, GNOME
  Terminal, etc.)
- FTXUI/ncurses send character sequences - no GPU API access at TUI library
  level
- This limitation applies to all terminal UI approaches

### Integration

Added to `vcpkg.json`:

```json
{
  "dependencies": [
    "ncurses",
    "ftxui",
    "spdlog",
    "gtest"
  ],
  "builtin-baseline": "23dc124705fcac41cf35c33dd9541f5094a9c19f"
}
```

Added to `CMakeLists.txt`:

```cmake
find_package(ftxui CONFIG REQUIRED)
target_link_libraries(wordNebula
    PRIVATE
    ${CURSES_LIBRARIES}
    ftxui::screen
    ftxui::dom
    ftxui::component
    spdlog::spdlog
)
```

______________________________________________________________________

## IView Interface Design

### Philosophy

**State-based rendering** with **event-driven input**:

- Presenter tells View WHAT to display (via `ViewState`)
- View decides HOW to display it (layout, colors, styling)
- User input flows from View → Presenter via callbacks
- Clean separation of concerns

### Interface Contract

**File**: [include/View/IView.hpp](../../include/View/IView.hpp)

**Key Components:**

1. **ViewState struct** - Complete application state for rendering:

   ```cpp
   struct ViewState {
       std::string visibleText;    // Text in viewport
       int cursorPosition;         // Linear cursor position
       int wordCount;              // Primary writing metric (MVP)
       std::string filename;       // Current file
       bool isDirty;               // Unsaved changes
       std::string statusMessage;  // Temporary messages
       bool showHelp;              // Help overlay toggle
   };
   ```

1. **InputEvent struct** - User input abstraction:

   ```cpp
   struct InputEvent {
       enum class Type {
           CHARACTER,               // Printable character
           ARROW_LEFT, ARROW_RIGHT, ARROW_UP, ARROW_DOWN,
           CTRL_LEFT, CTRL_RIGHT, CTRL_UP, CTRL_DOWN,
           HOME, END, PAGE_UP, PAGE_DOWN,
           BACKSPACE, DELETE, ENTER,
           CTRL_S, CTRL_O, CTRL_Q, CTRL_W, CTRL_H,
           ESCAPE, RESIZE, UNKNOWN
       };
       Type type;
       char character = '\0';      // Valid only for CHARACTER
   };
   ```

1. **IView interface** - Abstract view contract:

   ```cpp
   class IView {
       virtual void run(std::function<void(const InputEvent&)> onInput) = 0;
       virtual void render(const ViewState& state) = 0;
       virtual void exit() = 0;
       virtual std::pair<int, int> getTerminalSize() const = 0;
       virtual void showMessage(const std::string& message, bool isError) = 0;
   };
   ```

**Design Benefits:**

- Presenter is library-agnostic (can swap FTXUI for another library)
- Thin interface (only essential operations)
- State-based (View doesn't track incremental changes)
- Testable (can mock IView for Presenter tests)

______________________________________________________________________

## MVP Statistics Simplification

### Decision: Word Count Only

**Rationale:**

- For prose/novel writing, **word count is THE metric** that matters
- Publishers use word count for submission requirements
- Writing goals are set in words (e.g., "write 2000 words today")
- NaNoWriMo and similar challenges are word-count based

**Why NOT line count:**

- Line count is a formatting artifact (depends on terminal width, wrapping,
  font)
- Only meaningful for poetry, scripts, or code (not prose)
- Changes constantly as user resizes terminal
- Not a semantic document property

**Why NOT paragraph count (for MVP):**

- Nice to have, but not essential for MVP
- Can be added later if writers find it useful
- Keeping MVP minimal and focused

**MVP Status Bar:**

```
mynovel.txt * │ Words: 1,234
```

**Future enhancements:**

- Paragraph count (optional)
- Character count (useful for Twitter threads, abstracts)
- Reading time estimate (based on average reading speed)
- Custom goals/targets ("500 words remaining")

______________________________________________________________________

## Cursor Rendering Strategy

### Decision Process

**Options Considered:**

1. **Vertical bar cursor** - Insert `|` character at cursor position
1. **Colored character** - Apply styling to character at cursor
1. **Inverted character** - Flip colors of character at cursor
1. **Custom FTXUI component** - Complex, custom rendering

**User Preferences (from discussion):**

- Visual: "Lets keep it simple vertical bar for now"
- Blinking: "We can choose the simplest choice" → Non-blinking
- Wrapping: "beginning of next" line when wrapping
- Unicode: "UTF8 is okay in MVP" (full Unicode later)
- Viewport: "like vim for now" → Fixed viewport, text scrolls

### Final Decision: Cyan Background Cursor

**Implementation:**

```cpp
// Split text around cursor position
string before = text.substr(0, cursorPos);
string atCursor = text.substr(cursorPos, 1);  // Character at cursor
string after = text.substr(cursorPos + 1);

// Render with cyan background
return hbox({
    text(before),
    text(atCursor) | bgcolor(Color::Cyan) | color(Color::Black),
    text(after)
});
```

**Why cyan background:**

- High contrast on VS Code's dark terminal (primary development environment)
- Cyan pops nicely against black/dark gray backgrounds
- Black text on cyan background = very readable
- Simple to implement with FTXUI's styling API

**Future Enhancement:**

- User preferences/theming system (config file)
- Customizable cursor color, style, blinking
- Multiple cursor styles (block, underline, bar)
- Theme support (dark mode, light mode, custom themes)

______________________________________________________________________

## FtxuiView Implementation

### Status: Header Complete ✅

**File**: [include/View/FtxuiView.hpp](../../include/View/FtxuiView.hpp)

**Class Overview:**

```cpp
class FtxuiView : public IView {
  public:
    FtxuiView();
    ~FtxuiView() override;

    // IView interface implementation
    void run(std::function<void(const InputEvent&)> onInput) override;
    void render(const ViewState& state) override;
    void exit() override;
    std::pair<int, int> getTerminalSize() const override;
    void showMessage(const std::string& message, bool isError) override;

  private:
    // Rendering methods
    ftxui::Component createMainComponent();
    ftxui::Element renderEditor();
    ftxui::Element renderStatusBar();
    ftxui::Element renderHelpOverlay();

    // Event handling
    InputEvent translateEvent(const ftxui::Event& event);

    // State
    ftxui::ScreenInteractive screen_;
    ftxui::Component mainComponent_;
    ViewState currentState_;
    std::function<void(const InputEvent&)> inputCallback_;
    bool shouldExit_ = false;
    std::string temporaryMessage_;
    bool temporaryMessageIsError_ = false;
};
```

### Implementation Strategy

**UI Component Tree:**

```
┌─────────────────────────────────────┐
│  Text Editor Area                   │
│  - Word-wrapped text                │
│  - Cursor (cyan background)         │
│  - Fixed viewport (vim-style)       │
├─────────────────────────────────────┤
│  Status Bar                         │
│  filename.txt * │ Words: 42 Lines: 5│
├─────────────────────────────────────┤
│  Status Message Bar (if any)        │
│  "Saved successfully" / "Error: ..." │
└─────────────────────────────────────┘

[Help Overlay - appears over everything when showHelp=true]
```

**Key Methods:**

1. **`run()`** - Event loop

   - Initialize FTXUI screen
   - Create main component
   - Start blocking event loop
   - Translate FTXUI events to InputEvent
   - Call onInput callback

1. **`render()`** - State update

   - Update `currentState_` with new state
   - Trigger FTXUI re-render
   - FTXUI calls rendering methods to rebuild UI

1. **`renderEditor()`** - Text with cursor

   - Split text at cursor position
   - Apply cyan background to character at cursor
   - Handle word wrapping to terminal width
   - Return FTXUI Element

1. **`renderStatusBar()`** - File info

   - Filename (or "Untitled")
   - Dirty indicator (`*`)
   - Word count and line count
   - Styled with FTXUI

1. **`translateEvent()`** - Input mapping

   - Convert FTXUI Event to InputEvent enum
   - Map keyboard shortcuts (Ctrl+S, Ctrl+Q, etc.)
   - Handle special keys (arrows, Page Up/Down, etc.)

### Status: Implemented ✅

All steps below were completed in `src/View/FtxuiView.cpp`:

1. ~~Create `src/View/FtxuiView.cpp`~~
1. ~~Implement constructor (initialize screen)~~
1. ~~Implement `getTerminalSize()` (simple)~~
1. ~~Implement event loop (`run()`)~~
1. ~~Implement rendering methods~~
1. ~~Implement event translation~~
1. ~~Test with minimal Presenter integration~~

**User will code this with guidance/hints** (learning experience)

______________________________________________________________________

## Future Enhancements

### Section/Chapter Navigation

**User mentioned:** "section/chapter/act navigation so they dont have to scroll
too much"

**Design considerations:**

- Command system (`:section next`, `:chapter 3`, `:act 1`)
- Document structure metadata (sections, chapters, acts)
- Quick jump to section (table of contents overlay)
- Breadcrumb navigation in status bar
- Keyboard shortcuts (Ctrl+PgUp/PgDn for section navigation)

**Not in MVP** - Fixed viewport with vim-style scrolling is sufficient for now

### User Preferences/Theming

**Configuration file** (`~/.config/wordnebula/config.json`):

```json
{
  "theme": {
    "cursor_style": "block",           // "block", "underline", "bar"
    "cursor_color": "#00FFFF",         // Hex color
    "cursor_blink": false,
    "background": "#1E1E1E",
    "foreground": "#D4D4D4",
    "status_bar_bg": "#007ACC",
    "syntax_highlight": false          // Future: Markdown syntax
  },
  "editor": {
    "word_wrap": true,
    "wrap_column": 80,                 // Or 0 for terminal width
    "show_line_numbers": false,        // Future enhancement
    "font_size": "medium"              // Terminal emulator dependent
  }
}
```

**Implementation needs:**

- JSON parsing library (nlohmann/json via vcpkg)
- Settings class to manage preferences
- Default theme + custom theme loading
- Theme validation and error handling

______________________________________________________________________

## Testing Strategy

### Unit Tests (Google Test)

**File**: `tests/test_FtxuiView.cpp`

**Test cases:**

1. Constructor/destructor don't crash
1. `getTerminalSize()` returns valid dimensions
1. `render()` updates internal state correctly
1. `translateEvent()` maps FTXUI events correctly
1. `showMessage()` updates temporary message state

**Mocking challenges:**

- FTXUI's ScreenInteractive is not easily mockable
- Consider integration tests instead of pure unit tests
- Test individual rendering methods (renderEditor, renderStatusBar)

### Integration Tests

**Manual testing:**

1. Launch wordNebula with FtxuiView
1. Type characters, verify cursor moves
1. Use arrow keys, verify navigation
1. Ctrl+S, Ctrl+Q, verify commands work
1. Resize terminal, verify layout adapts
1. Type long text, verify word wrapping

**Automated integration tests** (future):

- Capture FTXUI screen output
- Compare against expected output
- Requires test harness for terminal UIs

______________________________________________________________________

## Open Questions

1. **Word wrapping algorithm**: How to handle UTF-8 character width
   calculations?

   - ASCII is 1 character = 1 column
   - UTF-8 can be multi-byte, variable width on screen
   - Consider using FTXUI's built-in text wrapping?

1. **Viewport scrolling**: Fixed viewport size or dynamic based on terminal?

   - Start with full terminal height - status bar height
   - Future: User-configurable viewport height

1. **Help overlay**: Modal dialog or side panel?

   - Modal overlay (like vim's `:help`) for MVP
   - Can add side panel later

1. **Error messages**: How long to display temporary messages?

   - Option A: Auto-dismiss after timeout (3 seconds?)
   - Option B: Dismiss on next input event
   - Recommend: Dismiss on next input (simpler)

______________________________________________________________________

## References

- [FTXUI Documentation](https://arthursonzogni.github.io/FTXUI/)
- [FTXUI Examples](https://github.com/ArthurSonzogni/FTXUI/tree/main/examples)
- [IView Interface](../../include/View/IView.hpp)
- [FtxuiView Header](../../include/View/FtxuiView.hpp)
- [Platform Support Documentation](../../CLAUDE.md#platform-support)

______________________________________________________________________

## Timeline

- **2026-02-16**: Library evaluation, FTXUI selected
- **2026-02-16**: IView interface designed and implemented
- **2026-02-16**: FtxuiView header completed
- **Next**: User will implement FtxuiView.cpp with guidance

______________________________________________________________________

**Note**: This is a living document. Update as implementation progresses and new
decisions are made.
