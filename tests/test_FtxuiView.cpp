/**
 * @file test_FtxuiView.cpp
 * @brief Unit tests for FtxuiView::translateEvent()
 *
 * translateEvent() is pure mapping logic with no side effects and no terminal
 * interaction, so it can be called directly without a running event loop.
 * Constructing FtxuiView is safe in a headless environment because
 * ScreenInteractive::Fullscreen() only sets up the object — the terminal is
 * not touched until Loop() is called.
 */

#include "View/FtxuiView.hpp"
#include <gtest/gtest.h>

using namespace wnebula;

class FtxuiViewTest : public ::testing::Test {
  protected:
    FtxuiView view;
};

// ============================================================================
// Arrow keys
// ============================================================================

TEST_F(FtxuiViewTest, TranslateEventArrowLeft) {
    auto ie = view.translateEvent(ftxui::Event::ArrowLeft);
    EXPECT_EQ(ie.type, InputEvent::Type::ARROW_LEFT);
}

TEST_F(FtxuiViewTest, TranslateEventArrowRight) {
    auto ie = view.translateEvent(ftxui::Event::ArrowRight);
    EXPECT_EQ(ie.type, InputEvent::Type::ARROW_RIGHT);
}

TEST_F(FtxuiViewTest, TranslateEventArrowUp) {
    auto ie = view.translateEvent(ftxui::Event::ArrowUp);
    EXPECT_EQ(ie.type, InputEvent::Type::ARROW_UP);
}

TEST_F(FtxuiViewTest, TranslateEventArrowDown) {
    auto ie = view.translateEvent(ftxui::Event::ArrowDown);
    EXPECT_EQ(ie.type, InputEvent::Type::ARROW_DOWN);
}

// ============================================================================
// Ctrl+Arrow keys
// ============================================================================

TEST_F(FtxuiViewTest, TranslateEventCtrlLeft) {
    auto ie = view.translateEvent(ftxui::Event::ArrowLeftCtrl);
    EXPECT_EQ(ie.type, InputEvent::Type::CTRL_LEFT);
}

TEST_F(FtxuiViewTest, TranslateEventCtrlRight) {
    auto ie = view.translateEvent(ftxui::Event::ArrowRightCtrl);
    EXPECT_EQ(ie.type, InputEvent::Type::CTRL_RIGHT);
}

TEST_F(FtxuiViewTest, TranslateEventCtrlUp) {
    auto ie = view.translateEvent(ftxui::Event::ArrowUpCtrl);
    EXPECT_EQ(ie.type, InputEvent::Type::CTRL_UP);
}

TEST_F(FtxuiViewTest, TranslateEventCtrlDown) {
    auto ie = view.translateEvent(ftxui::Event::ArrowDownCtrl);
    EXPECT_EQ(ie.type, InputEvent::Type::CTRL_DOWN);
}

// ============================================================================
// Editing keys
// ============================================================================

TEST_F(FtxuiViewTest, TranslateEventBackspace) {
    auto ie = view.translateEvent(ftxui::Event::Backspace);
    EXPECT_EQ(ie.type, InputEvent::Type::BACKSPACE);
}

TEST_F(FtxuiViewTest, TranslateEventDelete) {
    auto ie = view.translateEvent(ftxui::Event::Delete);
    EXPECT_EQ(ie.type, InputEvent::Type::DELETE);
}

TEST_F(FtxuiViewTest, TranslateEventReturn) {
    auto ie = view.translateEvent(ftxui::Event::Return);
    EXPECT_EQ(ie.type, InputEvent::Type::ENTER);
}

// ============================================================================
// Navigation keys
// ============================================================================

TEST_F(FtxuiViewTest, TranslateEventHome) {
    auto ie = view.translateEvent(ftxui::Event::Home);
    EXPECT_EQ(ie.type, InputEvent::Type::HOME);
}

TEST_F(FtxuiViewTest, TranslateEventEnd) {
    auto ie = view.translateEvent(ftxui::Event::End);
    EXPECT_EQ(ie.type, InputEvent::Type::END);
}

TEST_F(FtxuiViewTest, TranslateEventPageUp) {
    auto ie = view.translateEvent(ftxui::Event::PageUp);
    EXPECT_EQ(ie.type, InputEvent::Type::PAGE_UP);
}

TEST_F(FtxuiViewTest, TranslateEventPageDown) {
    auto ie = view.translateEvent(ftxui::Event::PageDown);
    EXPECT_EQ(ie.type, InputEvent::Type::PAGE_DOWN);
}

// ============================================================================
// Ctrl+key commands
// ============================================================================

TEST_F(FtxuiViewTest, TranslateEventCtrlS) {
    auto ie = view.translateEvent(ftxui::Event::CtrlS);
    EXPECT_EQ(ie.type, InputEvent::Type::CTRL_S);
}

TEST_F(FtxuiViewTest, TranslateEventCtrlO) {
    auto ie = view.translateEvent(ftxui::Event::CtrlO);
    EXPECT_EQ(ie.type, InputEvent::Type::CTRL_O);
}

TEST_F(FtxuiViewTest, TranslateEventCtrlQ) {
    auto ie = view.translateEvent(ftxui::Event::CtrlQ);
    EXPECT_EQ(ie.type, InputEvent::Type::CTRL_Q);
}

TEST_F(FtxuiViewTest, TranslateEventCtrlW) {
    auto ie = view.translateEvent(ftxui::Event::CtrlW);
    EXPECT_EQ(ie.type, InputEvent::Type::CTRL_W);
}

TEST_F(FtxuiViewTest, TranslateEventF1) {
    auto ie = view.translateEvent(ftxui::Event::F1);
    EXPECT_EQ(ie.type, InputEvent::Type::F1);
}

TEST_F(FtxuiViewTest, TranslateEventEscape) {
    auto ie = view.translateEvent(ftxui::Event::Escape);
    EXPECT_EQ(ie.type, InputEvent::Type::ESCAPE);
}

// ============================================================================
// Printable character
// ============================================================================

TEST_F(FtxuiViewTest, TranslateEventPrintableChar) {
    auto ie = view.translateEvent(ftxui::Event::Character("A"));
    EXPECT_EQ(ie.type, InputEvent::Type::CHARACTER);
    EXPECT_EQ(ie.character, 'A');
}

// ============================================================================
// Unknown / unhandled event
// ============================================================================

TEST_F(FtxuiViewTest, TranslateEventUnknown) {
    auto ie = view.translateEvent(ftxui::Event::Custom);
    EXPECT_EQ(ie.type, InputEvent::Type::UNKNOWN);
}
