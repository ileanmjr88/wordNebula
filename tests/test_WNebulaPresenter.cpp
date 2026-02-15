/**
 * @file test_WNebulaPresenter.cpp
 * @brief Unit tests for WNebulaPresenter class
 *
 * NOTE: These tests focus on the Presenter's state management and Model interaction.
 * View rendering is tested separately in integration tests since it requires ncurses.
 */

#include "WNebulaModel.hpp"
#include "WNebulaPresenter.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace wnebula;

class WNebulaPresenterTest : public ::testing::Test {
  protected:
    void SetUp() override {
        presenter = std::make_shared<WNebulaPresenter>();
        model = std::make_shared<WNebulaModel>();
        // Note: We skip View setup for unit tests as it requires ncurses initialization
    }

    void TearDown() override {
        model.reset();
        presenter.reset();
    }

    std::shared_ptr<WNebulaPresenter> presenter;
    std::shared_ptr<WNebulaModel> model;
};

// ============================================================================
// isDirty Flag Tests
// ============================================================================

TEST_F(WNebulaPresenterTest, IsDirtyInitiallyFalse) { EXPECT_FALSE(presenter->getIsDirty()); }

TEST_F(WNebulaPresenterTest, SaveFileClearsIsDirty) {
    presenter->saveFile("test.txt");
    EXPECT_FALSE(presenter->getIsDirty());
}

TEST_F(WNebulaPresenterTest, LoadFileClearsIsDirty) {
    presenter->loadFile("test.txt");
    EXPECT_FALSE(presenter->getIsDirty());
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
// Model Integration Tests
// ============================================================================

TEST_F(WNebulaPresenterTest, ModelTextOperationsWork) {
    model->insertChar('H');
    model->insertChar('e');
    model->insertChar('l');
    model->insertChar('l');
    model->insertChar('o');

    EXPECT_EQ(model->getText(), "Hello");
    EXPECT_EQ(model->getCursorPosition(), 5);
    EXPECT_EQ(model->getLength(), 5);
}

TEST_F(WNebulaPresenterTest, ModelDeleteOperationsWork) {
    model->insertText("Hello");
    model->deleteChar();

    EXPECT_EQ(model->getText(), "Hell");
    EXPECT_EQ(model->getCursorPosition(), 4);
}

TEST_F(WNebulaPresenterTest, ModelCursorMovement) {
    model->insertText("Hello World");

    model->moveCursor(-5);
    EXPECT_EQ(model->getCursorPosition(), 6);

    model->setCursorPosition(0);
    EXPECT_EQ(model->getCursorPosition(), 0);

    model->setCursorPosition(model->getLength());
    EXPECT_EQ(model->getCursorPosition(), 11);
}

TEST_F(WNebulaPresenterTest, ModelWordBoundaryNavigation) {
    model->insertText("Hello World");

    int nextWord = model->findNextWordBoundary(0);
    EXPECT_EQ(nextWord, 6);

    int prevWord = model->findPrevWordBoundary(11);
    EXPECT_EQ(prevWord, 6);
}

TEST_F(WNebulaPresenterTest, ModelParagraphNavigation) {
    model->insertText("Line 1\nLine 2\nLine 3");

    int nextPara = model->findNextParagraph(0);
    EXPECT_EQ(nextPara, 7);

    int prevPara = model->findPrevParagraph(14);
    EXPECT_EQ(prevPara, 7);
}

// ============================================================================
// File I/O State Tests
// ============================================================================

TEST_F(WNebulaPresenterTest, SaveLoadCycleWorksCorrectly) {
    presenter->saveFile("test1.txt");
    EXPECT_FALSE(presenter->getIsDirty());

    presenter->loadFile("test2.txt");
    EXPECT_FALSE(presenter->getIsDirty());
}
