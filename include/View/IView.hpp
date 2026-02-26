/**
 * @file IView.hpp
 * @brief Abstract View interface for wordNebula
 *
 * Defines the contract between Presenter and View implementations.
 * Keeps Presenter independent of specific UI libraries (FTXUI, ncurses).
 *
 * Design Philosophy:
 * - Thin interface: Only essential operations
 * - State-based: View renders complete state, doesn't track incremental changes
 * - Event-driven: View reports user input via callbacks
 * - Implementation-agnostic: Works with FTXUI, ncurses, or other libraries
 */

#pragma once

#include <functional>
#include <string>
#include <utility>

namespace wnebula {

/**
 * @brief Complete view state passed from Presenter to View
 *
 * Contains all information needed to render the current application state.
 * The View receives this and decides HOW to display it.
 */
struct ViewState {
    // Content
    std::string visibleText; // Text currently visible in viewport
    int cursorPosition;      // Linear position of cursor in visibleText

    // Status bar information
    int wordCount;        // Total word count (primary metric for writers)
    std::string filename; // Current file name (or "Untitled")
    bool isDirty;         // Unsaved changes indicator

    // UI state
    std::string statusMessage; // Temporary status message (e.g., "Saved", "Error: ...")
    bool showHelp;             // Whether to show help overlay
};

/**
 * @brief Input event from View to Presenter
 *
 * Represents a single user input action. The View translates raw
 * keyboard/mouse events into these semantic actions.
 */
struct InputEvent {
    enum class Type {
        // Character input
        CHARACTER, // Regular printable character

        // Basic navigation
        ARROW_LEFT,
        ARROW_RIGHT,
        ARROW_UP,
        ARROW_DOWN,

        // Word/paragraph navigation (Ctrl+arrows)
        CTRL_LEFT,  // Previous word
        CTRL_RIGHT, // Next word
        CTRL_UP,    // Previous paragraph
        CTRL_DOWN,  // Next paragraph

        // Line navigation
        HOME, // Start of line/document
        END,  // End of line/document

        // Page navigation
        PAGE_UP,
        PAGE_DOWN,

        // Editing
        BACKSPACE,
        DELETE,
        ENTER, // New line

        // Commands (Ctrl+key)
        CTRL_S, // Save
        CTRL_O, // Open
        CTRL_Q, // Quit
        CTRL_W, // Close

        // Function keys
        F1, // Help

        // Other
        ESCAPE,
        RESIZE, // Terminal was resized
        UNKNOWN
    };

    Type type;
    char character = '\0'; // Only valid for Type::CHARACTER
};

/**
 * @brief Abstract View interface
 *
 * This interface defines the contract between the Presenter and any View
 * implementation (FTXUI, ncurses, etc.). The Presenter only interacts
 * through this interface, maintaining clean separation of concerns.
 *
 * Key design principles:
 * - The Presenter tells the View WHAT to display (via ViewState)
 * - The View decides HOW to display it (layout, colors, etc.)
 * - User input flows from View → Presenter via callbacks
 * - The View owns the event loop and UI lifecycle
 */
class IView {
  public:
    virtual ~IView() = default;

    /**
     * @brief Initialize and start the View's main event loop
     *
     * This is a blocking call that runs until the user quits or exit() is called.
     * The View will call the onInput callback whenever the user provides input.
     *
     * @param onInput Callback invoked for each user input event
     *
     * Example usage:
     * @code
     * view->run([&](const InputEvent& event) {
     *     presenter.handleInput(event);
     * });
     * @endcode
     */
    virtual void run(std::function<void(const InputEvent &)> onInput) = 0;

    /**
     * @brief Update the display with new state
     *
     * The Presenter calls this whenever the application state changes
     * (text edited, cursor moved, file saved, etc.). The View will
     * re-render based on the new state.
     *
     * @param state Complete current state to display
     *
     * Note: The View may batch/optimize redraws internally.
     */
    virtual void render(const ViewState &state) = 0;

    /**
     * @brief Request the View to exit gracefully
     *
     * Signals the View to stop its event loop and clean up.
     * The run() method should return after this is called.
     */
    virtual void exit() = 0;

    /**
     * @brief Get current terminal dimensions
     *
     * @return Pair of (width, height) in characters
     *
     * The Presenter may use this to calculate viewport sizes or
     * adjust text wrapping behavior.
     */
    [[nodiscard]] virtual std::pair<int, int> getTerminalSize() const = 0;

    /**
     * @brief Show a temporary status message
     *
     * Convenience method for displaying temporary messages (e.g., "Saved",
     * "Error: file not found"). The message may auto-dismiss after a timeout.
     *
     * @param message Message to display
     * @param isError Whether this is an error message (may affect styling)
     *
     * Note: Alternatively, messages can be passed via ViewState.statusMessage
     */
    virtual void showMessage(const std::string &message, bool isError = false) = 0;
};

} // namespace wnebula
