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
