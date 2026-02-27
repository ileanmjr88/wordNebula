/**
 * @file test_WNebulaPresenter.cpp
 * @brief Unit tests for WNebulaPresenter class
 *
 * Tests the Presenter layer by calling Presenter methods (not Model directly)
 * and verifying proper delegation to Model and View.
 */

#include "Model/WNebulaModel.hpp"
#include "Presenter/WNebulaPresenter.hpp"
#include "View/IView.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>

using namespace wnebula;

// ============================================================================
// MockView - Test double implementing IView directly (no ncurses)
// ============================================================================

class MockView : public IView {
  public:
    // run() saves the callback but returns immediately (non-blocking for tests)
    void run(std::function<void(const InputEvent &)> onInput) override { inputCallback = std::move(onInput); }

    void render(const ViewState &state) override {
        lastState = state;
        renderCallCount++;
    }

    void exit() override { exitCalled = true; }

    [[nodiscard]] std::pair<int, int> getTerminalSize() const override { return {80, 24}; }

    void showMessage(const std::string &message, bool isError = false) override {
        lastMessage = message;
        lastMessageIsError = isError;
    }

    // Test inspection fields
    ViewState lastState;
    int renderCallCount = 0;
    bool exitCalled = false;
    std::string lastMessage;
    bool lastMessageIsError = false;
    std::function<void(const InputEvent &)> inputCallback;
};

// ============================================================================
// Test Fixture
// ============================================================================

class WNebulaPresenterTest : public ::testing::Test {
  protected:
    void SetUp() override {
        presenter = std::make_shared<WNebulaPresenter>();
        model = std::make_shared<WNebulaModel>();
        view = std::make_shared<MockView>();

        // Wire up the components (THIS IS CRITICAL!)
        presenter->setup(view, model);
    }

    void TearDown() override {
        view.reset();
        model.reset();
        presenter.reset();
    }

    std::shared_ptr<WNebulaPresenter> presenter;
    std::shared_ptr<WNebulaModel> model;
    std::shared_ptr<MockView> view;
};

// ============================================================================
// isDirty Flag Tests
// ============================================================================

TEST_F(WNebulaPresenterTest, IsDirtyInitiallyFalse) { EXPECT_FALSE(presenter->getIsDirty()); }

TEST_F(WNebulaPresenterTest, OnInsertSetsIsDirty) {
    EXPECT_FALSE(presenter->getIsDirty());

    presenter->onInsert('H');

    EXPECT_TRUE(presenter->getIsDirty());
}

TEST_F(WNebulaPresenterTest, OnDeleteSetsIsDirty) {
    presenter->onInsert('H');
    view->renderCallCount = 0; // Reset counter

    presenter->onDelete();

    EXPECT_TRUE(presenter->getIsDirty());
}

TEST_F(WNebulaPresenterTest, OnDeleteForwardSetsIsDirty) {
    presenter->onInsert('H');
    presenter->onMoveCursorLeft();
    view->renderCallCount = 0;

    presenter->onDeleteForward();

    EXPECT_TRUE(presenter->getIsDirty());
}

TEST_F(WNebulaPresenterTest, NavigationDoesNotSetIsDirty) {
    presenter->onInsert('H');
    presenter->saveFile("test.txt"); // Clear isDirty
    EXPECT_FALSE(presenter->getIsDirty());

    presenter->onMoveCursorLeft();
    presenter->onMoveCursorRight();
    presenter->onHome();
    presenter->onEnd();

    EXPECT_FALSE(presenter->getIsDirty());
}

TEST_F(WNebulaPresenterTest, SaveFileClearsIsDirty) {
    presenter->onInsert('H');
    EXPECT_TRUE(presenter->getIsDirty());

    presenter->saveFile("test.txt");

    EXPECT_FALSE(presenter->getIsDirty());
}

TEST_F(WNebulaPresenterTest, LoadFileClearsIsDirty) {
    presenter->onInsert('H');
    EXPECT_TRUE(presenter->getIsDirty());

    presenter->loadFile("test.txt");

    EXPECT_FALSE(presenter->getIsDirty());
}

// ============================================================================
// Text Operation Tests (Through Presenter API)
// ============================================================================

TEST_F(WNebulaPresenterTest, OnInsertDelegatesToModel) {
    presenter->onInsert('H');
    presenter->onInsert('i');

    EXPECT_EQ(model->getText(), "Hi");
    EXPECT_EQ(model->getCursorPosition(), 2);
}

TEST_F(WNebulaPresenterTest, OnInsertTriggersRender) {
    view->renderCallCount = 0;

    presenter->onInsert('X');

    EXPECT_EQ(view->renderCallCount, 1);
    EXPECT_EQ(view->lastState.visibleText, "X");
    EXPECT_EQ(view->lastState.cursorPosition, 1);
}

TEST_F(WNebulaPresenterTest, OnDeleteDelegatesToModel) {
    presenter->onInsert('H');
    presenter->onInsert('i');

    presenter->onDelete(); // Delete 'i'

    EXPECT_EQ(model->getText(), "H");
    EXPECT_EQ(model->getCursorPosition(), 1);
}

TEST_F(WNebulaPresenterTest, OnDeleteTriggersRender) {
    presenter->onInsert('H');
    view->renderCallCount = 0;

    presenter->onDelete();

    EXPECT_EQ(view->renderCallCount, 1);
    EXPECT_EQ(view->lastState.visibleText, "");
}

TEST_F(WNebulaPresenterTest, OnDeleteForwardDelegatesToModel) {
    presenter->onInsert('H');
    presenter->onInsert('i');
    presenter->onMoveCursorLeft(); // Cursor at position 1 (between H and i)

    presenter->onDeleteForward(); // Delete 'i'

    EXPECT_EQ(model->getText(), "H");
    EXPECT_EQ(model->getCursorPosition(), 1);
}

// ============================================================================
// Navigation Tests (Through Presenter API)
// ============================================================================

TEST_F(WNebulaPresenterTest, OnMoveCursorLeftDelegatesToModel) {
    presenter->onInsert('H');
    presenter->onInsert('i');

    presenter->onMoveCursorLeft();

    EXPECT_EQ(model->getCursorPosition(), 1);
}

TEST_F(WNebulaPresenterTest, OnMoveCursorRightDelegatesToModel) {
    presenter->onInsert('H');
    presenter->onInsert('i');
    presenter->onMoveCursorLeft();

    presenter->onMoveCursorRight();

    EXPECT_EQ(model->getCursorPosition(), 2);
}

TEST_F(WNebulaPresenterTest, OnMoveCursorGenericDelegatesToModel) {
    presenter->onInsert('H');
    presenter->onInsert('i');

    presenter->onMoveCursor(-1); // Move left
    EXPECT_EQ(model->getCursorPosition(), 1);

    presenter->onMoveCursor(1); // Move right
    EXPECT_EQ(model->getCursorPosition(), 2);
}

TEST_F(WNebulaPresenterTest, OnHomeDelegatesToModel) {
    presenter->onInsert('H');
    presenter->onInsert('e');
    presenter->onInsert('l');
    presenter->onInsert('l');
    presenter->onInsert('o');

    presenter->onHome();

    EXPECT_EQ(model->getCursorPosition(), 0);
}

TEST_F(WNebulaPresenterTest, OnEndDelegatesToModel) {
    presenter->onInsert('H');
    presenter->onInsert('i');
    presenter->onHome();

    presenter->onEnd();

    EXPECT_EQ(model->getCursorPosition(), 2);
}

TEST_F(WNebulaPresenterTest, OnCtrlLeftDelegatesToModel) {
    presenter->onInsert('H');
    presenter->onInsert('e');
    presenter->onInsert('l');
    presenter->onInsert('l');
    presenter->onInsert('o');
    presenter->onInsert(' ');
    presenter->onInsert('W');
    presenter->onInsert('o');
    presenter->onInsert('r');
    presenter->onInsert('l');
    presenter->onInsert('d');

    presenter->onCtrlLeft(); // Jump to start of "World"

    EXPECT_EQ(model->getCursorPosition(), 6);
}

TEST_F(WNebulaPresenterTest, OnCtrlRightDelegatesToModel) {
    presenter->onInsert('H');
    presenter->onInsert('e');
    presenter->onInsert('l');
    presenter->onInsert('l');
    presenter->onInsert('o');
    presenter->onInsert(' ');
    presenter->onInsert('W');
    presenter->onHome();

    presenter->onCtrlRight(); // Jump to end of "Hello"

    EXPECT_EQ(model->getCursorPosition(), 6);
}

TEST_F(WNebulaPresenterTest, OnCtrlUpDelegatesToModel) {
    presenter->onInsert('L');
    presenter->onInsert('i');
    presenter->onInsert('n');
    presenter->onInsert('e');
    presenter->onInsert(' ');
    presenter->onInsert('1');
    presenter->onInsert('\n');
    presenter->onInsert('L');
    presenter->onInsert('i');
    presenter->onInsert('n');
    presenter->onInsert('e');
    presenter->onInsert(' ');
    presenter->onInsert('2');

    presenter->onCtrlUp(); // Jump to previous paragraph

    EXPECT_EQ(model->getCursorPosition(), 7);
}

TEST_F(WNebulaPresenterTest, OnCtrlDownDelegatesToModel) {
    presenter->onInsert('L');
    presenter->onInsert('i');
    presenter->onInsert('n');
    presenter->onInsert('e');
    presenter->onInsert(' ');
    presenter->onInsert('1');
    presenter->onInsert('\n');
    presenter->onInsert('L');
    presenter->onHome();

    presenter->onCtrlDown(); // Jump to next paragraph

    EXPECT_EQ(model->getCursorPosition(), 7);
}

TEST_F(WNebulaPresenterTest, NavigationTriggersRender) {
    presenter->onInsert('H');
    presenter->onInsert('i');
    view->renderCallCount = 0;

    presenter->onMoveCursorLeft();

    EXPECT_EQ(view->renderCallCount, 1);
    EXPECT_EQ(view->lastState.cursorPosition, 1);
}

// ============================================================================
// Application Control Tests
// ============================================================================

TEST_F(WNebulaPresenterTest, OnExitDoesNotCrash) { EXPECT_NO_THROW(presenter->onExit()); }

TEST_F(WNebulaPresenterTest, MultipleExitCallsDoNotCrash) {
    EXPECT_NO_THROW({
        presenter->onExit();
        presenter->onExit();
    });
}

// ============================================================================
// File I/O State Tests
// ============================================================================

TEST_F(WNebulaPresenterTest, SaveLoadCycleWorksCorrectly) {
    presenter->onInsert('H');
    EXPECT_TRUE(presenter->getIsDirty());

    presenter->saveFile("test1.txt");
    EXPECT_FALSE(presenter->getIsDirty());

    presenter->onInsert('i');
    EXPECT_TRUE(presenter->getIsDirty());

    presenter->loadFile("test2.txt");
    EXPECT_FALSE(presenter->getIsDirty());
}

// ============================================================================
// HandleInput Tests
// ============================================================================

TEST_F(WNebulaPresenterTest, HandleInputCharacterInsertChar) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    ie.character = 'A';
    presenter->handleInput(ie);

    EXPECT_EQ(model->getText(), "A");
}

TEST_F(WNebulaPresenterTest, HandleInputBackspaceDeleteChar) {
    InputEvent ieBackspace{InputEvent::Type::BACKSPACE};
    InputEvent ieCharacter{InputEvent::Type::CHARACTER};

    ieCharacter.character = 'X';
    presenter->handleInput(ieCharacter);
    presenter->handleInput(ieBackspace);

    EXPECT_EQ(model->getText(), "");
    EXPECT_TRUE(presenter->getIsDirty());
}

TEST_F(WNebulaPresenterTest, HandleInputDeleteDeletesForward) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    for (char c : std::string{"WordNebula"}) {
        ie.character = c;
        presenter->handleInput(ie);
    }
    // cursor at end (pos 12), move left one position
    presenter->handleInput(InputEvent{InputEvent::Type::ARROW_LEFT});

    // delete the character at cursor (forward delete removes 'o')
    presenter->handleInput(InputEvent{InputEvent::Type::DELETE});

    EXPECT_EQ(model->getText(), "WordNebul");
}

TEST_F(WNebulaPresenterTest, HandleInputEnterInsertsNewline) {
    presenter->handleInput(InputEvent{InputEvent::Type::ENTER});
    EXPECT_NE(model->getText().find('\n'), std::string::npos);
}

TEST_F(WNebulaPresenterTest, HandleInputArrowLeftMovesCursor) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    for (char c : std::string{"AB"}) {
        ie.character = c;
        presenter->handleInput(ie);
    }
    // cursor at 2
    presenter->handleInput(InputEvent{InputEvent::Type::ARROW_LEFT});
    EXPECT_EQ(model->getCursorPosition(), 1);
}

TEST_F(WNebulaPresenterTest, HandleInputArrowRightMovesCursor) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    for (char c : std::string{"AB"}) {
        ie.character = c;
        presenter->handleInput(ie);
    }
    presenter->onHome(); // cursor at 0
    presenter->handleInput(InputEvent{InputEvent::Type::ARROW_RIGHT});
    EXPECT_EQ(model->getCursorPosition(), 1);
}

TEST_F(WNebulaPresenterTest, HandleInputCtrlLeftJumpsWord) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    for (char c : std::string{"Hello World"}) {
        ie.character = c;
        presenter->handleInput(ie);
    }
    // cursor at 11 (end)
    presenter->handleInput(InputEvent{InputEvent::Type::CTRL_LEFT});
    EXPECT_EQ(model->getCursorPosition(), 6); // start of "World"
}

TEST_F(WNebulaPresenterTest, HandleInputCtrlRightJumpsWord) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    for (char c : std::string{"Hello W"}) {
        ie.character = c;
        presenter->handleInput(ie);
    }
    presenter->onHome(); // cursor at 0
    presenter->handleInput(InputEvent{InputEvent::Type::CTRL_RIGHT});
    EXPECT_EQ(model->getCursorPosition(), 6); // past "Hello "
}

TEST_F(WNebulaPresenterTest, HandleInputCtrlUpJumpsParagraph) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    for (char c : std::string{"Line 1\nLine 2"}) {
        ie.character = c;
        presenter->handleInput(ie);
    }
    // cursor at 13 (end)
    presenter->handleInput(InputEvent{InputEvent::Type::CTRL_UP});
    EXPECT_EQ(model->getCursorPosition(), 7); // start of "Line 2"
}

TEST_F(WNebulaPresenterTest, HandleInputCtrlDownJumpsParagraph) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    for (char c : std::string{"Line 1\nL"}) {
        ie.character = c;
        presenter->handleInput(ie);
    }
    presenter->onHome(); // cursor at 0
    presenter->handleInput(InputEvent{InputEvent::Type::CTRL_DOWN});
    EXPECT_EQ(model->getCursorPosition(), 7); // past "Line 1\n"
}

TEST_F(WNebulaPresenterTest, HandleInputHomeGoesToStart) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    for (char c : std::string{"Hello"}) {
        ie.character = c;
        presenter->handleInput(ie);
    }
    presenter->handleInput(InputEvent{InputEvent::Type::HOME});
    EXPECT_EQ(model->getCursorPosition(), 0);
}

TEST_F(WNebulaPresenterTest, HandleInputEndGoesToEnd) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    for (char c : std::string{"Hello"}) {
        ie.character = c;
        presenter->handleInput(ie);
    }
    presenter->onHome(); // cursor at 0
    presenter->handleInput(InputEvent{InputEvent::Type::END});
    EXPECT_EQ(model->getCursorPosition(), 5);
}

TEST_F(WNebulaPresenterTest, HandleInputCtrlSClearsIsDirty) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    ie.character = 'A';
    presenter->handleInput(ie); // makes dirty
    EXPECT_TRUE(presenter->getIsDirty());

    presenter->handleInput(InputEvent{InputEvent::Type::CTRL_S});
    EXPECT_FALSE(presenter->getIsDirty());
}

TEST_F(WNebulaPresenterTest, HandleInputF1TogglesHelp) {
    presenter->handleInput(InputEvent{InputEvent::Type::F1});
    EXPECT_TRUE(view->lastState.showHelp);
}

TEST_F(WNebulaPresenterTest, HandleInputCtrlQTriggersExit) {
    // clean state — exits immediately without warning
    presenter->handleInput(InputEvent{InputEvent::Type::CTRL_Q});
    EXPECT_TRUE(view->exitCalled);
}

TEST_F(WNebulaPresenterTest, HandleInputEscapeTriggersExit) {
    // clean state — exits immediately without warning
    presenter->handleInput(InputEvent{InputEvent::Type::ESCAPE});
    EXPECT_TRUE(view->exitCalled);
}

TEST_F(WNebulaPresenterTest, HandleInputUnknownDoesNothing) {
    const int rendersBefore = view->renderCallCount;
    presenter->handleInput(InputEvent{InputEvent::Type::UNKNOWN});
    EXPECT_EQ(model->getText(), "");
    EXPECT_EQ(view->renderCallCount, rendersBefore);
    EXPECT_FALSE(presenter->getIsDirty());
}

// ============================================================================
// Exit Flow Tests
// ============================================================================

TEST_F(WNebulaPresenterTest, OnExitCleanStateExitsImmediately) {
    presenter->onExit();
    EXPECT_TRUE(view->exitCalled);
}

TEST_F(WNebulaPresenterTest, OnExitDirtyFirstPressShowsWarning) {
    presenter->onInsert('X'); // make dirty
    presenter->onExit();      // first press
    EXPECT_FALSE(view->exitCalled);
    EXPECT_TRUE(view->lastMessageIsError);
}

TEST_F(WNebulaPresenterTest, OnExitDirtySecondPressExits) {
    presenter->onInsert('X'); // make dirty
    presenter->onExit();      // first press: shows warning
    presenter->onExit();      // second press: exits
    EXPECT_TRUE(view->exitCalled);
}

TEST_F(WNebulaPresenterTest, OnExitDirtyThenOtherInputResetsWarning) {
    presenter->onInsert('X'); // make dirty
    presenter->onExit();      // first press: shows warning, sets exitWarningShown

    // a character event through handleInput resets exitWarningShown
    InputEvent ie{InputEvent::Type::CHARACTER};
    ie.character = 'Y';
    presenter->handleInput(ie);

    presenter->onExit(); // warning shown again — did NOT exit
    EXPECT_FALSE(view->exitCalled);
    EXPECT_TRUE(view->lastMessageIsError);
}

TEST_F(WNebulaPresenterTest, OnExitEscapeAliasSameBehavior) {
    presenter->onInsert('X');                                     // make dirty
    presenter->handleInput(InputEvent{InputEvent::Type::ESCAPE}); // first press via ESCAPE
    EXPECT_FALSE(view->exitCalled);
    EXPECT_TRUE(view->lastMessageIsError);
}

// ============================================================================
// ViewState Propagation Tests
// ============================================================================

TEST_F(WNebulaPresenterTest, UpdateViewPropagatesIsDirty) {
    presenter->onInsert('X');
    EXPECT_TRUE(view->lastState.isDirty);
}

TEST_F(WNebulaPresenterTest, UpdateViewPropagatesFilename) {
    presenter->loadFile("novel.txt");
    EXPECT_EQ(view->lastState.filename, "novel.txt");
}

TEST_F(WNebulaPresenterTest, UpdateViewDefaultFilenameIsUntitled) {
    presenter->onHome(); // triggers updateView without changing content
    EXPECT_EQ(view->lastState.filename, "Untitled");
}

TEST_F(WNebulaPresenterTest, UpdateViewPropagatesWordCount) {
    InputEvent ie{InputEvent::Type::CHARACTER};
    for (char c : std::string{"Hello World"}) {
        ie.character = c;
        presenter->handleInput(ie);
    }
    EXPECT_EQ(view->lastState.wordCount, 2);
}

TEST_F(WNebulaPresenterTest, UpdateViewPropagatesShowHelp) {
    presenter->onToggleHelp();
    EXPECT_TRUE(view->lastState.showHelp);
}

TEST_F(WNebulaPresenterTest, UpdateViewShowHelpTogglesOff) {
    presenter->onToggleHelp();
    presenter->onToggleHelp();
    EXPECT_FALSE(view->lastState.showHelp);
}

TEST_F(WNebulaPresenterTest, UpdateViewPropagatesCursorPosition) {
    presenter->onInsert('H');
    presenter->onInsert('i');
    presenter->onMoveCursorLeft();
    EXPECT_EQ(view->lastState.cursorPosition, 1);
}
