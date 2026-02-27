# View Implementation Progress (FtxuiView)

**Created**: 2026-02-25 **Branch**: `phase1/view` **Status**: Implemented and
tested — merged to `develop` via `phase1/test`

______________________________________________________________________

## What Was Done

### Files Created/Modified

- **`src/View/FtxuiView.cpp`** - Full implementation of the FTXUI-based View
- **`include/View/FtxuiView.hpp`** - Added `renderStatusMessage()` declaration
  with Doxygen docs
- **`include/View/KeyboardShortcuts.hpp`** - Static shortcut definitions for
  help overlay
- **`CMakeLists.txt`** - Added `src/View/FtxuiView.cpp` to `WORDNEBULA_SOURCES`

### Implemented Methods

| Method                  | Status | Notes                                              |
| ----------------------- | ------ | -------------------------------------------------- |
| `FtxuiView()`           | Done   | Member initializer list for `screen`               |
| `~FtxuiView()`          | Done   | `= default`                                        |
| `run()`                 | Done   | Stores callback, creates component, calls `Loop`   |
| `render()`              | Done   | Saves state, posts Custom event to trigger redraw  |
| `exit()`                | Done   | Calls `screen.Exit()` directly                     |
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

## Exit Flow (Resolved)

`exit()` calls `screen.Exit()` directly. The double-press-to-quit logic lives
entirely in the Presenter (`WNebulaPresenter::onExit()`), keeping the View
passive as intended by MVP:

```cpp
// WNebulaPresenter::onExit() — Presenter decides when to exit
void WNebulaPresenter::onExit() {
    if (isDirty && !exitWarningShown) {
        exitWarningShown = true;
        if (auto v = view.lock()) {
            v->showMessage("Unsaved changes! Press Ctrl+Q again to quit.", true);
        }
        return;
    }
    isRunning = false;
    if (auto v = view.lock()) { v->exit(); }
}
```

`exitWarningShown` is reset by `handleInput()` on any non-exit event.

______________________________________________________________________

## Build Notes

### CMake Configure Command

Use the `devcontainer` preset (vcpkg toolchain is set automatically):

```bash
cmake --preset devcontainer
ninja -C build
```

For coverage (GCC required — Clang's profiling runtime not installed):

```bash
cmake --preset coverage
ninja -C build-coverage coverage
```

______________________________________________________________________

## What's Left (Deferred to Later Phases)

### Known Limitations

- [ ] Word wrapping in `renderEditor()` (currently single-line hbox)
- [ ] Newline handling in `renderEditor()` (need to split on `\n` and use vbox)
- [ ] Viewport scrolling for long documents
- [ ] Line-aware Up/Down arrow navigation (currently delegates to paragraph
  jump)
- [ ] Auto-dismiss temporary messages after timeout
- [ ] File I/O in Presenter (`saveFile()` / `loadFile()` are stubs)

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
