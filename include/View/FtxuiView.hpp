/**
 * @file FtxuiView.hpp
 * @brief FTXUI-based implementation of IView interface
 *
 * Provides a modern terminal UI using the FTXUI library.
 * Handles rendering, input events, and cursor visualization.
 */

#pragma once

#include "View/IView.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <memory>
#include <string>

namespace wnebula {

/**
 * @brief FTXUI implementation of the View interface
 *
 * This view provides a modern terminal UI with:
 * - Cyan background cursor (optimized for dark terminals)
 * - Status bar with file info and statistics
 * - Keyboard event handling
 *
 * Implementation Strategy:
 * - Uses FTXUI's Component system for rendering
 * - Cursor rendered as character with cyan background
 * - Input events translated from FTXUI events to InputEvent
 * - State-based rendering (full re-render on state change)
 *
 * @note Word-wrapping and viewport scrolling are not yet implemented.
 *       The editor area renders all text in a single hbox row (TODO).
 */
class FtxuiView : public IView {
  public:
    FtxuiView();
    ~FtxuiView() override;

    // Disable copy/move (manages terminal state)
    FtxuiView(const FtxuiView &) = delete;
    FtxuiView &operator=(const FtxuiView &) = delete;
    FtxuiView(FtxuiView &&) = delete;
    FtxuiView &operator=(FtxuiView &&) = delete;

    /**
     * @brief Start the FTXUI event loop
     *
     * This is a blocking call that runs until exit() is called.
     * The onInput callback is invoked for each user input event.
     *
     * @param onInput Callback for input events
     */
    void run(std::function<void(const InputEvent &)> onInput) override;

    /**
     * @brief Update display with new state
     *
     * Triggers a re-render with the provided state.
     * FTXUI will redraw the entire UI based on the new state.
     *
     * @param state Complete current application state
     */
    void render(const ViewState &state) override;

    /**
     * @brief Exit the event loop gracefully
     *
     * Signals FTXUI to stop the event loop and return from run().
     */
    void exit() override;

    /**
     * @brief Get current terminal dimensions
     *
     * @return Pair of (width, height) in characters
     */
    [[nodiscard]] std::pair<int, int> getTerminalSize() const override;

    /**
     * @brief Show temporary status message
     *
     * Updates the status message in the current ViewState.
     * For persistent messages, pass via ViewState.statusMessage.
     *
     * @param message Message to display
     * @param isError Whether this is an error message (affects styling)
     */
    void showMessage(const std::string &message, bool isError = false) override;

    /**
     * @brief Convert FTXUI event to InputEvent
     *
     * Translates FTXUI's event system to our InputEvent enum.
     *
     * @param event FTXUI event
     * @return Translated InputEvent
     */
    InputEvent translateEvent(const ftxui::Event &event);

  private:
    /**
     * @brief Create the main UI component
     *
     * Builds the FTXUI component tree:
     * - Text editor area (with cursor)
     * - Status bar (filename, word count, dirty indicator)
     * - Help overlay (if showHelp is true)
     * - Status message bar
     *
     * @return FTXUI Component representing the complete UI
     */
    ftxui::Component createMainComponent();

    /**
     * @brief Render the text editor area with cursor
     *
     * Renders the visible text with cursor visualization:
     * - Text before cursor (normal styling)
     * - Character at cursor (cyan background, black text)
     * - Text after cursor (normal styling)
     *
     * @note Currently renders as a single hbox with no word-wrapping or
     *       viewport scrolling (TODO).
     *
     * @return FTXUI Element representing the editor area
     */
    ftxui::Element renderEditor();

    /**
     * @brief Render the status bar
     *
     * Displays:
     * - Filename (or "Untitled"), prefixed with "~ " when dirty
     * - Word count
     *
     * @return FTXUI Element representing the status bar
     */
    ftxui::Element renderStatusBar();

    /**
     * @brief Render temporary status message bar
     *
     * Displays temporary messages at the top of the screen,
     * such as save confirmations or error warnings.
     * Styled differently for errors vs informational messages.
     *
     * @return FTXUI Element representing the status message bar
     */
    ftxui::Element renderStatusMessage();

    /**
     * @brief Render help overlay
     *
     * Shows keyboard shortcuts when ViewState.showHelp is true.
     *
     * @return FTXUI Element representing the help overlay
     */
    ftxui::Element renderHelpOverlay();

    // FTXUI screen and components
    ftxui::ScreenInteractive screen;
    ftxui::Component mainComponent;

    // Current state (updated via render())
    ViewState currentState{};

    // Input callback (set via run())
    std::function<void(const InputEvent &)> inputCallback;

    // Temporary status message (from showMessage())
    std::string temporaryMessage;
    bool temporaryMessageIsError = false;
};

} // namespace wnebula
