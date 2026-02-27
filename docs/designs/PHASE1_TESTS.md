# Phase 1 Unit Test Plan

**Created**: 2026-02-26 **Branch**: `phase1/tests` **Status**: Planning

______________________________________________________________________

## Current Test Coverage

| File                        | Tests | What It Covers                                 |
| --------------------------- | ----- | ---------------------------------------------- |
| `test_GapBuffer.cpp`        | 40    | All IBuffer operations, edge cases, navigation |
| `test_TextBuffer.cpp`       | 44    | Alternate buffer implementation                |
| `test_WNebulaPresenter.cpp` | 25    | Presenter methods called **directly** via API  |
| **Total**                   | 109   |                                                |

______________________________________________________________________

## Gaps — What Needs Tests

### Priority 1: `handleInput()` Dispatch (add to `test_WNebulaPresenter.cpp`)

The existing presenter tests call `onInsert()`, `onDelete()`, etc. **directly**.
The `handleInput()` routing switch is never exercised — a typo in the switch
would silently break user input with no test failure.

**Approach**: Call `presenter->handleInput(InputEvent{...})` and verify the same
outcome as calling the method directly.

| Test Name                             | Input Event                      | What to Verify                          |
| ------------------------------------- | -------------------------------- | --------------------------------------- |
| `HandleInput_Character_InsertsChar`   | `Type::CHARACTER, character='A'` | `model->getText() == "A"`               |
| `HandleInput_Backspace_DeletesChar`   | `Type::BACKSPACE`                | text shrinks, `isDirty == true`         |
| `HandleInput_Delete_DeletesForward`   | `Type::DELETE`                   | forward char removed                    |
| `HandleInput_Enter_InsertsNewline`    | `Type::ENTER`                    | `model->getText()` contains `'\n'`      |
| `HandleInput_ArrowLeft_MovesCursor`   | `Type::ARROW_LEFT`               | `model->getCursorPosition()` decrements |
| `HandleInput_ArrowRight_MovesCursor`  | `Type::ARROW_RIGHT`              | cursor increments                       |
| `HandleInput_CtrlLeft_JumpsWord`      | `Type::CTRL_LEFT`                | cursor at prev word boundary            |
| `HandleInput_CtrlRight_JumpsWord`     | `Type::CTRL_RIGHT`               | cursor at next word boundary            |
| `HandleInput_CtrlUp_JumpsParagraph`   | `Type::CTRL_UP`                  | cursor at prev paragraph                |
| `HandleInput_CtrlDown_JumpsParagraph` | `Type::CTRL_DOWN`                | cursor at next paragraph                |
| `HandleInput_Home_GoesToStart`        | `Type::HOME`                     | `getCursorPosition() == 0`              |
| `HandleInput_End_GoesToEnd`           | `Type::END`                      | cursor at text length                   |
| `HandleInput_CtrlS_ClearsIsDirty`     | `Type::CTRL_S`                   | `isDirty == false`                      |
| `HandleInput_F1_TogglesHelp`          | `Type::F1`                       | `lastState.showHelp` flips              |
| `HandleInput_CtrlQ_TriggersExit`      | `Type::CTRL_Q`                   | exit flow begins                        |
| `HandleInput_Escape_TriggersExit`     | `Type::ESCAPE`                   | exit flow begins                        |
| `HandleInput_Unknown_DoesNothing`     | `Type::UNKNOWN`                  | no state change                         |

______________________________________________________________________

### Priority 2: Exit Flow Behavior (add to `test_WNebulaPresenter.cpp`)

The double-press-to-quit pattern and unsaved changes warning are key UX
behaviors that currently have minimal coverage (`OnExitDoesNotCrash` only).

| Test Name                                  | Setup                                        | What to Verify                                                  |
| ------------------------------------------ | -------------------------------------------- | --------------------------------------------------------------- |
| `OnExit_CleanState_ExitsImmediately`       | No inserts                                   | `view->exitCalled == true`                                      |
| `OnExit_DirtyFirstPress_ShowsWarning`      | Insert char, call `onExit()`                 | `view->lastMessageIsError == true`, `view->exitCalled == false` |
| `OnExit_DirtySecondPress_Exits`            | Insert char, `onExit()` twice                | `view->exitCalled == true` after second call                    |
| `OnExit_DirtyThenOtherInput_ResetsWarning` | Insert, `onExit()`, insert again, `onExit()` | warning shown again (not exit) — `exitWarningShown` reset       |
| `OnExit_EscapeAlias_SameBehavior`          | Use `ESCAPE` instead of `CTRL_Q`             | same exit flow                                                  |

______________________________________________________________________

### Priority 3: ViewState Field Propagation (add to `test_WNebulaPresenter.cpp`)

Verifies that `updateView()` correctly populates every field of `ViewState`.
These catch regressions if new state is added but not wired up.

| Test Name                              | Setup                             | What to Verify                      |
| -------------------------------------- | --------------------------------- | ----------------------------------- |
| `UpdateView_PropagatesIsDirty`         | Insert char                       | `lastState.isDirty == true`         |
| `UpdateView_PropagatesFilename`        | `loadFile("novel.txt")`           | `lastState.filename == "novel.txt"` |
| `UpdateView_DefaultFilenameIsUntitled` | No `loadFile()` call              | `lastState.filename == "Untitled"`  |
| `UpdateView_PropagatesWordCount`       | Insert "Hello World"              | `lastState.wordCount == 2`          |
| `UpdateView_PropagatesShowHelp`        | `onToggleHelp()`                  | `lastState.showHelp == true`        |
| `UpdateView_ShowHelpTogglesOff`        | `onToggleHelp()` twice            | `lastState.showHelp == false`       |
| `UpdateView_PropagatesCursorPosition`  | Insert "Hi", `onMoveCursorLeft()` | `lastState.cursorPosition == 1`     |

______________________________________________________________________

### Priority 4: `FtxuiView::translateEvent()` (new file: `test_FtxuiView.cpp`)

`translateEvent()` is pure mapping logic — no terminal or screen needed.
`ftxui::Event` has static members (`Event::ArrowLeft`, `Event::CtrlS`, etc.)
that can be constructed directly in tests.

**Challenge**: `translateEvent()` is currently `private`. Options:

- Make it `public` (simplest, acceptable since it's deterministic logic)
- Add `friend class FtxuiViewTest` to header
- Extract into a free function in an anonymous namespace and test via the public
  `run()` + callback path (indirect, more complex)

**Recommendation**: Make `translateEvent()` `public` — it has no side effects
and is clearly testable in isolation.

| Test Name                      | Input (`ftxui::Event`)  | Expected `InputEvent::Type`      |
| ------------------------------ | ----------------------- | -------------------------------- |
| `TranslateEvent_ArrowLeft`     | `Event::ArrowLeft`      | `ARROW_LEFT`                     |
| `TranslateEvent_ArrowRight`    | `Event::ArrowRight`     | `ARROW_RIGHT`                    |
| `TranslateEvent_ArrowUp`       | `Event::ArrowUp`        | `ARROW_UP`                       |
| `TranslateEvent_ArrowDown`     | `Event::ArrowDown`      | `ARROW_DOWN`                     |
| `TranslateEvent_CtrlLeft`      | `Event::ArrowLeftCtrl`  | `CTRL_LEFT`                      |
| `TranslateEvent_CtrlRight`     | `Event::ArrowRightCtrl` | `CTRL_RIGHT`                     |
| `TranslateEvent_CtrlUp`        | `Event::ArrowUpCtrl`    | `CTRL_UP`                        |
| `TranslateEvent_CtrlDown`      | `Event::ArrowDownCtrl`  | `CTRL_DOWN`                      |
| `TranslateEvent_Backspace`     | `Event::Backspace`      | `BACKSPACE`                      |
| `TranslateEvent_Delete`        | `Event::Delete`         | `DELETE`                         |
| `TranslateEvent_Return`        | `Event::Return`         | `ENTER`                          |
| `TranslateEvent_Home`          | `Event::Home`           | `HOME`                           |
| `TranslateEvent_End`           | `Event::End`            | `END`                            |
| `TranslateEvent_PageUp`        | `Event::PageUp`         | `PAGE_UP`                        |
| `TranslateEvent_PageDown`      | `Event::PageDown`       | `PAGE_DOWN`                      |
| `TranslateEvent_CtrlS`         | `Event::CtrlS`          | `CTRL_S`                         |
| `TranslateEvent_CtrlO`         | `Event::CtrlO`          | `CTRL_O`                         |
| `TranslateEvent_CtrlQ`         | `Event::CtrlQ`          | `CTRL_Q`                         |
| `TranslateEvent_CtrlW`         | `Event::CtrlW`          | `CTRL_W`                         |
| `TranslateEvent_F1`            | `Event::F1`             | `F1`                             |
| `TranslateEvent_Escape`        | `Event::Escape`         | `ESCAPE`                         |
| `TranslateEvent_PrintableChar` | `Event::Character("A")` | `CHARACTER`, `.character == 'A'` |
| `TranslateEvent_Unknown`       | `Event::Custom`         | `UNKNOWN`                        |

______________________________________________________________________

## Files to Create/Modify

| File                              | Action     | Notes                                      |
| --------------------------------- | ---------- | ------------------------------------------ |
| `tests/test_WNebulaPresenter.cpp` | **Modify** | Add Priority 1, 2, 3 test groups           |
| `tests/test_FtxuiView.cpp`        | **Create** | Priority 4 — `translateEvent()` tests      |
| `include/View/FtxuiView.hpp`      | **Modify** | Make `translateEvent()` public             |
| `CMakeLists.txt`                  | **Modify** | Register `test_FtxuiView` as a test target |

______________________________________________________________________

## What Is NOT Worth Testing

| Item                                        | Reason                                                                               |
| ------------------------------------------- | ------------------------------------------------------------------------------------ |
| `renderEditor()`, `renderStatusBar()`, etc. | Build FTXUI Element trees — no text output to assert against without a real terminal |
| `run()`                                     | Starts blocking FTXUI event loop — requires terminal, can't unit test                |
| `showMessage()` / `render()`                | Visual side effects only; tested indirectly via Presenter tests                      |
| `WNebulaModel` directly                     | Thin wrapper over GapBuffer; already covered by Presenter tests                      |

______________________________________________________________________

## Test Implementation Notes

### `test_WNebulaPresenter.cpp` — no new infrastructure needed

The existing `MockView` and `WNebulaPresenterTest` fixture cover everything. New
tests just need `handleInput()` to be public (it already is).

```cpp
// Example: handleInput dispatch test
TEST_F(WNebulaPresenterTest, HandleInput_Character_InsertsChar) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    ie.character = 'A';
    presenter->handleInput(ie);
    EXPECT_EQ(model->getText(), "A");
}

// Example: exit flow test
TEST_F(WNebulaPresenterTest, OnExit_DirtyFirstPress_ShowsWarning) {
    presenter->onInsert('X');          // make dirty
    presenter->onExit();               // first press
    EXPECT_FALSE(view->exitCalled);
    EXPECT_TRUE(view->lastMessageIsError);
}
```

### `test_FtxuiView.cpp` — requires `translateEvent()` to be public

```cpp
#include "View/FtxuiView.hpp"
#include <ftxui/component/event.hpp>
#include <gtest/gtest.h>

using namespace wnebula;

TEST(FtxuiViewTranslateEvent, ArrowLeft) {
    FtxuiView view;
    auto ie = view.translateEvent(ftxui::Event::ArrowLeft);
    EXPECT_EQ(ie.type, InputEvent::Type::ARROW_LEFT);
}
```

> **Note**: Constructing `FtxuiView` calls
> `ftxui::ScreenInteractive::Fullscreen()` in the constructor. This may require
> checking if that is safe in a non-terminal test environment (CI). If it causes
> issues, extract `translateEvent()` as a free function or static method to
> decouple it from the screen.
