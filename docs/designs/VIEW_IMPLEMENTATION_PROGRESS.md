# View Implementation Progress (FtxuiView)

**Created**: 2026-02-25 **Branch**: `phase1/view` **Status**: First compile
successful, needs testing and refinements

______________________________________________________________________

## What Was Done

### Files Created/Modified

- **`src/View/FtxuiView.cpp`** - Full implementation of the FTXUI-based View
- **`include/View/FtxuiView.hpp`** - Added `renderStatusMessage()` declaration
  with Doxygen docs
- **`include/View/KeyboardShorcuts.hpp`** - Static shortcut definitions for help
  overlay
- **`CMakeLists.txt`** - Added `src/View/FtxuiView.cpp` to `WORDNEBULA_SOURCES`

### Implemented Methods

| Method                  | Status | Notes                                              |
| ----------------------- | ------ | -------------------------------------------------- |
| `FtxuiView()`           | Done   | Member initializer list for `screen`               |
| `~FtxuiView()`          | Done   | `= default`                                        |
| `run()`                 | Done   | Stores callback, creates component, calls `Loop`   |
| `render()`              | Done   | Saves state, posts Custom event to trigger redraw  |
| `exit()`                | WIP    | Currently sets `shouldExit = true` - see below     |
| `getTerminalSize()`     | Done   | Uses `ftxui::Terminal::Size()`                     |
| `showMessage()`         | Done   | Sets temporary message, triggers redraw            |
| `createMainComponent()` | Done   | Renderer + CatchEvent with dbox for help overlay   |
| `renderEditor()`        | Done   | hbox with before/cursor/after text segments        |
| `renderStatusBar()`     | Done   | Filename, dirty indicator (~), word count          |
| `renderStatusMessage()` | Done   | Conditional bar at top (red for errors, blue info) |
| `renderHelpOverlay()`   | Done   | Centered bordered box, reads KEYBOARD_SHORTCUTS    |
| `translateEvent()`      | Done   | Maps all FTXUI events to InputEvent enum           |

### UI Layout

```
┌──────────────────────────────────────────┐
│  status message (if any)                 │  ← renderStatusMessage()
├──────────────────────────────────────────┤
│                                          │
│  dbox {                                  │
│    renderEditor()    ← back layer        │
│    renderHelpOverlay() ← front layer     │
│  }                                       │
│                                          │
├──────────────────────────────────────────┤
│  ~ Untitled              Words: 0        │  ← renderStatusBar()
└──────────────────────────────────────────┘
```

______________________________________________________________________

## Open Issue: Exit Flow

### Current State

`exit()` sets `shouldExit = true` but does NOT call `screen.Exit()`. This means
**the app won't actually quit** because FTXUI's `Loop()` doesn't check our flag.

### The Problem

`screen.Exit()` terminates the FTXUI loop and restores the terminal immediately.
But we need to show warnings (like "unsaved changes") BEFORE exiting.

### Proposed Solution: Double-Press-to-Quit Pattern

The fix belongs in the **Presenter**, not the View. The View's `exit()` should
simply call `screen.Exit()`. The Presenter controls the decision:

```cpp
// In WNebulaPresenter:
void WNebulaPresenter::onExit() {
    if (isDirty && !exitWarningShown) {
        // First Ctrl+Q with unsaved changes: show warning, don't exit
        exitWarningShown = true;
        if (auto v = view.lock()) {
            v->showMessage("Unsaved changes! Press Ctrl+Q again to quit.", true);
        }
        return;  // don't exit yet
    }
    // Second Ctrl+Q (or no unsaved changes): actually exit
    isRunning = false;
    if (auto v = view.lock()) { v->exit(); }
}
```

This requires:

1. Adding `bool exitWarningShown = false;` to `WNebulaPresenter`
1. Resetting `exitWarningShown = false` on any other input (so warning
   dismisses)
1. Changing `FtxuiView::exit()` to call `screen.Exit()` instead of
   `shouldExit = true`
1. Removing `shouldExit` from `FtxuiView.hpp` (unused)

### Key Insight

The View is "passive" in MVP - it doesn't make decisions about when to exit. It
just provides the mechanism (`screen.Exit()`). The Presenter decides when.

______________________________________________________________________

## Build Notes

### CMake Configure Command

```bash
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake
```

**Important**: Must pass the vcpkg toolchain file or FTXUI won't be found.
Running `cmake ..` from inside `build/` without the toolchain file will fail.

### Compiler Warnings to Fix

1. **Sign conversion** in `renderEditor()` line 74: `cursorPosition` is `int`,
   assigned to `size_t`. Fix with `static_cast<size_t>()`.

1. **Unused private field** `shouldExit` in header - remove after implementing
   the exit flow fix above.

______________________________________________________________________

## What's Left Before Phase 1 MVP Is Complete

### Must Do

- [ ] Fix `exit()` to call `screen.Exit()` and implement double-press-to-quit in
  Presenter
- [ ] Fix compiler warnings (sign conversion, unused field)
- [ ] Test the app end-to-end (type text, navigate, quit)
- [ ] Implement file I/O in Presenter (`saveFile()` / `loadFile()` are stubs)

### Nice to Have (Can Defer)

- [ ] Word wrapping in `renderEditor()` (currently single-line hbox)
- [ ] Newline handling in `renderEditor()` (need to split on `\n` and use vbox)
- [ ] Viewport scrolling for long documents
- [ ] Auto-dismiss temporary messages after timeout

______________________________________________________________________

## FTXUI Quick Reference

For anyone picking this up who isn't familiar with FTXUI:

- **Element** = visual (text, box, layout) - like HTML elements
- **Component** = interactive (handles events) - like React components
- **`Renderer(lambda)`** = creates Component from Element-returning lambda
- **`CatchEvent(component, handler)`** = intercepts events before reaching
  component
- **`screen.Loop(component)`** = blocking event loop, runs until `Exit()`
- **`screen.PostEvent(Event::Custom)`** = trigger a redraw from outside the loop
- **`vbox/hbox`** = vertical/horizontal layout (CSS flexbox)
- **`dbox`** = layered stacking (CSS z-index / position: absolute)
- **`flex`** = fill remaining space (CSS flex: 1)
- **`filler()`** = empty spacer that expands (CSS flex-grow on empty div)
