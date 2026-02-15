/**
 * @file WNebulaPresenter.hpp
 * @brief Presenter layer for wordNebula MVP architecture
 */

#pragma once

#include <memory>
#include <string>

namespace wnebula {

class WNebulaView;
class WNebulaModel;

/**
 * @brief Presenter coordinating Model and View in MVP pattern
 *
 * The WNebulaPresenter acts as the mediator between the View (ncurses UI)
 * and the Model (text buffer). It handles:
 * - User input event routing
 * - Text editing operations with dirty tracking
 * - Navigation commands (character, word, paragraph, document level)
 * - File I/O coordination
 * - Application lifecycle management
 *
 * Design Pattern: MVP (Model-View-Presenter)
 * - Receives events from View
 * - Delegates operations to Model
 * - Updates View with new state
 *
 * @see WNebulaView for UI layer
 * @see WNebulaModel for data layer
 */
class WNebulaPresenter {
  public:
    /**
     * @brief Construct a new Presenter
     *
     * Initializes the presenter with default state (running, clean).
     * Call setup() to wire View and Model before use.
     */
    WNebulaPresenter();

    /**
     * @brief Destroy the Presenter
     *
     * Default
     */
    ~WNebulaPresenter() = default;

    // Non-copyable, non-movable
    WNebulaPresenter(const WNebulaPresenter &) = delete;
    WNebulaPresenter &operator=(const WNebulaPresenter &) = delete;
    WNebulaPresenter(WNebulaPresenter &&) = delete;
    WNebulaPresenter &operator=(WNebulaPresenter &&) = delete;

    /**
     * @brief Wire View and Model to this Presenter
     *
     * @param newView Shared pointer to View (held as weak_ptr)
     * @param newModel Shared pointer to Model
     */
    void setup(const std::shared_ptr<WNebulaView> &newView, std::shared_ptr<WNebulaModel> newModel);

    /**
     * @brief Main event loop
     *
     * Runs until onExit() is called, processing input from the View.
     */
    void run();

    // ========================================================================
    // Text Operations
    // ========================================================================

    /**
     * @brief Insert character at cursor position
     *
     * Sets isDirty flag and triggers View render.
     *
     * @param c Character to insert
     */
    void onInsert(char c);

    /**
     * @brief Delete character before cursor (Backspace)
     *
     * Sets isDirty flag and triggers View render.
     */
    void onDelete();

    /**
     * @brief Delete character at cursor (Delete key)
     *
     * Sets isDirty flag and triggers View render.
     */
    void onDeleteForward();

    // ========================================================================
    // Single Character Navigation
    // ========================================================================

    /**
     * @brief Move cursor left one character (Left arrow)
     *
     * Triggers View render with new cursor position.
     */
    void onMoveCursorLeft();

    /**
     * @brief Move cursor right one character (Right arrow)
     *
     * Triggers View render with new cursor position.
     */
    void onMoveCursorRight();

    /**
     * @brief Move cursor by offset (generic navigation)
     *
     * Bridge method for View compatibility. Moves cursor by specified offset.
     * Negative offset moves left, positive offset moves right.
     *
     * @param offset Number of positions to move (-1 = left, +1 = right)
     */
    void onMoveCursor(int offset);

    // ========================================================================
    // Smart Navigation
    // ========================================================================

    /**
     * @brief Jump to previous word boundary (Ctrl+Left)
     *
     * Uses Model's findPrevWordBoundary() for smart navigation.
     */
    void onCtrlLeft();

    /**
     * @brief Jump to next word boundary (Ctrl+Right)
     *
     * Uses Model's findNextWordBoundary() for smart navigation.
     */
    void onCtrlRight();

    /**
     * @brief Jump to previous paragraph boundary (Ctrl+Up)
     *
     * Uses Model's findPrevParagraph() for smart navigation.
     */
    void onCtrlUp();

    /**
     * @brief Jump to next paragraph boundary (Ctrl+Down)
     *
     * Uses Model's findNextParagraph() for smart navigation.
     */
    void onCtrlDown();

    /**
     * @brief Jump to start of document (Home)
     *
     * Sets cursor position to 0.
     */
    void onHome();

    /**
     * @brief Jump to end of document (End)
     *
     * Sets cursor position to buffer length.
     */
    void onEnd();

    // ========================================================================
    // Application Control
    // ========================================================================

    /**
     * @brief Exit the application (ESC key)
     *
     * Warns via log if isDirty flag is set (unsaved changes).
     * Sets isRunning to false to terminate main loop.
     */
    void onExit();

    // ========================================================================
    // File I/O
    // ========================================================================

    /**
     * @brief Save buffer to file (stubbed for Phase 1)
     *
     * Clears isDirty flag.
     * Full implementation deferred to later phase.
     *
     * @param path File path to save to
     */
    void saveFile(const std::string &path);

    /**
     * @brief Load buffer from file (stubbed for Phase 1)
     *
     * Sets currentFilePath and clears isDirty flag.
     * Full implementation deferred to later phase.
     *
     * @param path File path to load from
     */
    void loadFile(const std::string &path);

    /**
     * @brief Check if document has unsaved changes
     *
     * @return true if document has been modified since last save/load
     * @return false if document is clean
     */
    [[nodiscard]] bool getIsDirty() const;

  private:
    void updateView();

    std::weak_ptr<WNebulaView> view;
    std::shared_ptr<WNebulaModel> model;
    bool isRunning = true;
    bool isDirty = false;
    std::string currentFilePath = "";
};
} // namespace wnebula
