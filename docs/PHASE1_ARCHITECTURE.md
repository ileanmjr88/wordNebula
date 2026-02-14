# Phase 1: MVP Architecture - Visual Design

**Created**: 2026-02-14
**Status**: Design Phase
**Goal**: Simple distraction-free text editor for prose writing

---

## Table of Contents
1. [High-Level Architecture](#high-level-architecture)
2. [Component Responsibilities](#component-responsibilities)
3. [IBuffer Interface](#ibuffer-interface)
4. [GapBuffer Internals](#gapbuffer-internals)
5. [User Interaction Flows](#user-interaction-flows)
6. [Data Flow Examples](#data-flow-examples)
7. [Implementation Checklist](#implementation-checklist)

---

## High-Level Architecture

### MVP Pattern (Model-View-Presenter)

```
┌─────────────────────────────────────────────────────────────────┐
│                         USER                                     │
│                    (Types, presses keys)                         │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                      VIEW LAYER (ncurses)                        │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  WNebulaView                                              │  │
│  │  - Captures keyboard input                                │  │
│  │  - Renders text with soft line wrapping                   │  │
│  │  - Displays cursor at screen position                     │  │
│  │  - Shows status bar (word count, file name)               │  │
│  └──────────────────────────────────────────────────────────┘  │
└────────────────────────────┬────────────────────────────────────┘
                             │ Events (keypresses)
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                   PRESENTER LAYER                                │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  WNebulaPresenter                                         │  │
│  │  - Interprets user commands                               │  │
│  │  - Manages viewport (what text is visible)                │  │
│  │  - Handles file I/O (save/load)                           │  │
│  │  - Coordinates Model ↔ View                               │  │
│  │  - Implements word/paragraph navigation logic             │  │
│  └──────────────────────────────────────────────────────────┘  │
└────────────────────────────┬────────────────────────────────────┘
                             │ Operations
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                      MODEL LAYER                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  IBuffer (Interface)                                      │  │
│  │  - insertChar(char)                                       │  │
│  │  - deleteChar()                                           │  │
│  │  - moveCursor(offset)                                     │  │
│  │  - getText() / getTextRange()                             │  │
│  │  - findNextWordBoundary() / findNextParagraph()           │  │
│  └────────────────────┬─────────────────────────────────────┘  │
│                       │                                          │
│                       │ Implemented by:                          │
│                       ▼                                          │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  GapBuffer (Phase 1 Implementation)                       │  │
│  │  - Efficient editing at cursor position                   │  │
│  │  - Stores plain text (no formatting)                      │  │
│  │  - Gap moves with cursor                                  │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

---

## Component Responsibilities

### 🎨 View (WNebulaView) - "The Display"

**What it does:**
- Renders visible text in terminal window
- Wraps text based on terminal width (soft wrapping)
- Shows cursor at screen coordinates
- Displays status bar

**What it does NOT do:**
- ❌ Store text (that's Model)
- ❌ Decide what text to show (Presenter tells it)
- ❌ Handle business logic

**Key Methods:**
```cpp
class WNebulaView {
    void render(const std::string& visibleText,
                int cursorScreenRow,
                int cursorScreenCol,
                int wordCount);

    char getInput();  // Returns key pressed
};
```

---

### 🎯 Presenter (WNebulaPresenter) - "The Brain"

**What it does:**
- Receives input from View
- Translates keypresses into Model operations
- Manages viewport (scrolling)
- Handles file I/O
- Coordinates everything

**Example responsibilities:**
```cpp
class WNebulaPresenter {
    void onKeyPress(char key);           // User pressed a key
    void onCtrlRight();                  // Jump to next word
    void onCtrlS();                      // Save file
    void updateViewport();               // Recalculate visible text

private:
    int viewportStart;   // First visible character position
    int viewportEnd;     // Last visible character position
    IBuffer* buffer;     // The text content
    WNebulaView* view;   // The display
};
```

**Viewport Management:**
```
Buffer contains: "The quick brown fox jumps over the lazy dog..."
                  ^                                              ^
                  0                                            200

Terminal width: 40 chars
Terminal height: 10 lines

Viewport shows characters [0-400] (visible in 10 lines)
Cursor at position 50

If cursor moves to position 500:
→ Presenter recalculates viewport to [100-500]
→ Tells View to render getText(100, 400)
```

---

### 📦 Model (IBuffer → GapBuffer) - "The Content"

**What it does:**
- Stores the actual text
- Tracks cursor position
- Provides text operations (insert, delete)
- Finds word/paragraph boundaries

**What it does NOT do:**
- ❌ Render to screen
- ❌ Handle keypresses
- ❌ Know about viewport/scrolling
- ❌ Store formatting (bold, italic) - that comes later

---

## IBuffer Interface

### Complete Interface Definition

```cpp
namespace wnebula {

/**
 * @brief Interface for all buffer implementations
 *
 * Stores PLAIN TEXT ONLY (no formatting).
 * Cursor position is LINEAR (0 to length), not (row, col).
 *
 * Implementations:
 *   - TextBuffer (simple std::string based)
 *   - GapBuffer (efficient for typing - Phase 1)
 *   - Future: PieceTable (for undo/redo), RopeBuffer, etc.
 */
class IBuffer {
public:
    virtual ~IBuffer() = default;

    // ========================================================================
    // TEXT OPERATIONS
    // ========================================================================

    /** Insert character at current cursor position, advance cursor */
    virtual void insertChar(char c) = 0;

    /** Insert string at current cursor position, advance cursor */
    virtual void insertText(const std::string& text) = 0;

    /** Delete character BEFORE cursor (backspace), move cursor back */
    virtual void deleteChar() = 0;

    /** Delete character AT cursor (delete key), cursor stays */
    virtual void deleteForward() = 0;

    /** Delete range of characters */
    virtual void deleteText(int position, int length) = 0;

    // ========================================================================
    // TEXT ACCESS (for Presenter/View)
    // ========================================================================

    /** Get entire buffer content */
    virtual std::string getText() const = 0;

    /** Get substring [start, start+length) - for viewport */
    virtual std::string getTextRange(int start, int length) const = 0;

    /** Get total character count */
    virtual int getLength() const = 0;

    // ========================================================================
    // CURSOR MANAGEMENT
    // ========================================================================

    /** Get current cursor position (0 to length) */
    virtual int getCursorPosition() const = 0;

    /** Set cursor position (absolute) */
    virtual void setCursorPosition(int position) = 0;

    /** Move cursor by offset (relative) */
    virtual void moveCursor(int offset) = 0;

    // ========================================================================
    // SMART NAVIGATION (Ctrl+arrows, Home/End)
    // ========================================================================

    /** Find next word boundary position from given position */
    virtual int findNextWordBoundary(int fromPos) const = 0;

    /** Find previous word boundary position from given position */
    virtual int findPrevWordBoundary(int fromPos) const = 0;

    /** Find next paragraph (next \n or end) from given position */
    virtual int findNextParagraph(int fromPos) const = 0;

    /** Find previous paragraph (prev \n or start) from given position */
    virtual int findPrevParagraph(int fromPos) const = 0;

    // ========================================================================
    // STATISTICS (for status bar)
    // ========================================================================

    /** Count words in entire buffer */
    virtual int getWordCount() const = 0;

    /** Count paragraphs (number of \n + 1) */
    virtual int getParagraphCount() const = 0;
};

} // namespace wnebula
```

---

## GapBuffer Internals

### What is a Gap Buffer?

A gap buffer is like a **string with a hole in it**. The hole (gap) sits at the cursor position, making insertions and deletions very fast.

### Visual Representation

**Initial state (empty):**
```
Buffer: [_ _ _ _ _ _ _ _ _ _]
         ^                   ^
         gapStart           gapEnd
         cursor = 0
```

**After typing "Hello":**
```
Buffer: [H e l l o _ _ _ _ _]
                   ^         ^
                   gapStart  gapEnd
                   cursor = 5
```

**After pressing left arrow 3 times (cursor at position 2):**
```
Step 1: Move gap to cursor position 2
Buffer: [H e _ _ _ _ _ _ l o]
             ^         ^
             gapStart  gapEnd
             cursor = 2

Characters "l", "l", "o" moved to the right of gap
```

**After typing "X":**
```
Buffer: [H e X _ _ _ _ _ l o]
               ^       ^
               gapStart gapEnd
               cursor = 3
```

### Key Operations

#### Insert Character
```cpp
void GapBuffer::insertChar(char c) {
    // 1. Ensure gap is at cursor position
    moveGapToCursor();

    // 2. Check if gap is full (gapStart == gapEnd)
    if (gapStart >= gapEnd) {
        expandGap();  // Make gap bigger
    }

    // 3. Insert character at gap start
    buffer[gapStart] = c;
    gapStart++;
    cursor++;
}
```

#### Delete Character (Backspace)
```cpp
void GapBuffer::deleteChar() {
    if (cursor == 0) return;  // Nothing to delete

    moveGapToCursor();

    // Move gap start back (expands gap leftward)
    gapStart--;
    cursor--;
}
```

#### Move Gap to Cursor
```cpp
void GapBuffer::moveGapToCursor() {
    if (cursor == gapStart) return;  // Already there

    if (cursor < gapStart) {
        // Move gap LEFT: shift characters right
        while (gapStart > cursor) {
            gapStart--;
            gapEnd--;
            buffer[gapEnd] = buffer[gapStart];
        }
    } else {
        // Move gap RIGHT: shift characters left
        while (gapStart < cursor) {
            buffer[gapStart] = buffer[gapEnd];
            gapStart++;
            gapEnd++;
        }
    }
}
```

---

## User Interaction Flows

### Flow 1: User Types "Hello"

```
┌──────────────────┐
│  User presses 'H'│
└────────┬─────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  View.getInput() returns 'H'           │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  Presenter.onKeyPress('H')             │
│  → Checks: is it printable? Yes        │
│  → Calls: buffer->insertChar('H')      │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  GapBuffer.insertChar('H')             │
│  → buffer[gapStart] = 'H'              │
│  → gapStart++                          │
│  → cursor++                            │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  Presenter.updateViewport()            │
│  → Gets visible text from buffer       │
│  → Calculates cursor screen position   │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  View.render(visibleText, row, col)    │
│  → ncurses displays: "H_"              │
│  → Cursor blinks after H               │
└────────────────────────────────────────┘

Repeat for 'e', 'l', 'l', 'o'
Result: "Hello_"
```

---

### Flow 2: User Presses Ctrl+Right (Jump to Next Word)

```
Current buffer: "Hello World"
Cursor at position 0 (before 'H')
                 ^
                 cursor

┌──────────────────┐
│  User: Ctrl+Right│
└────────┬─────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  View.getInput() returns CTRL_RIGHT    │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  Presenter.onCtrlRight()               │
│  → Gets: currentPos = buffer->getCursorPosition()  = 0  │
│  → Calls: nextWord = buffer->findNextWordBoundary(0)    │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  GapBuffer.findNextWordBoundary(0)     │
│  → Scans from position 0               │
│  → Finds first non-space: 'H' (pos 0)  │
│  → Finds next space: ' ' (pos 5)       │
│  → Finds next non-space: 'W' (pos 6)   │
│  → Returns: 6                          │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  Presenter.onCtrlRight() continues     │
│  → Calls: buffer->setCursorPosition(6) │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  GapBuffer.setCursorPosition(6)        │
│  → cursor = 6                          │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  Presenter.updateViewport()            │
│  → Cursor moved, viewport unchanged    │
│  → Tells View to redraw with cursor    │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  View.render(...)                      │
│  → Displays: "Hello World"             │
│  → Cursor at: ------^                  │
└────────────────────────────────────────┘

Result: Cursor jumped from 'H' to 'W'
```

---

### Flow 3: User Presses Ctrl+S (Save File)

```
Buffer contains: "Hello World\nThis is a test."

┌──────────────────┐
│  User: Ctrl+S    │
└────────┬─────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  View.getInput() returns CTRL_S        │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  Presenter.onCtrlS()                   │
│  → Gets: text = buffer->getText()      │
│  → Calls: saveToFile(filename, text)   │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  Presenter.saveToFile()                │
│  → Opens file: "document.txt"          │
│  → Writes: "Hello World\nThis is..."   │
│  → Closes file                         │
│  → Sets: isDirty = false               │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  Presenter updates status              │
│  → Status: "Saved: document.txt"       │
└────────┬───────────────────────────────┘
         │
         ▼
┌────────────────────────────────────────┐
│  View.render()                         │
│  → Shows status bar: "Saved ✓"         │
└────────────────────────────────────────┘
```

---

## Data Flow Examples

### Example 1: Viewport Scrolling

```
Terminal size: 80 chars wide × 24 lines tall
Can display ~2000 characters

Buffer size: 50,000 characters (long document)
Cursor at position: 25,000 (middle of document)

┌─────────────────────────────────────────────────────────┐
│  Presenter.updateViewport()                             │
│                                                          │
│  1. Get cursor position: 25,000                         │
│  2. Get terminal size: 80×24 = ~2000 chars visible      │
│  3. Calculate viewport:                                 │
│     viewportStart = cursor - 1000  = 24,000             │
│     viewportEnd = cursor + 1000    = 26,000             │
│                                                          │
│  4. Get text: visibleText = buffer->getTextRange(24000, 2000) │
│                                                          │
│  5. Calculate cursor screen position:                   │
│     offsetInViewport = cursor - viewportStart = 1000    │
│     screenRow = offsetInViewport / 80 = 12              │
│     screenCol = offsetInViewport % 80 = 40              │
│                                                          │
│  6. Tell View: render(visibleText, row=12, col=40)      │
└─────────────────────────────────────────────────────────┘
```

### Example 2: Word Count Calculation

```
Buffer: "Hello World\nThis is a test."

┌─────────────────────────────────────────────────────────┐
│  GapBuffer.getWordCount()                               │
│                                                          │
│  1. Get all text: "Hello World\nThis is a test."        │
│  2. Scan through text:                                  │
│     - Start: inWord = false, count = 0                  │
│     - 'H': inWord = true, count = 1                     │
│     - 'e','l','l','o': inWord = true                    │
│     - ' ': inWord = false                               │
│     - 'W': inWord = true, count = 2                     │
│     - ... continue ...                                  │
│     - Final count = 6                                   │
│                                                          │
│  3. Return: 6                                           │
└─────────────────────────────────────────────────────────┘

Presenter displays in status bar: "Words: 6"
```

---

## Implementation Checklist

### Phase 1 Tasks

#### ✅ Step 1: Update IBuffer Interface
- [ ] Update `include/Model/IBuffer.hpp` with full interface
- [ ] Add new methods: `getTextRange()`, `findNextWordBoundary()`, etc.
- [ ] Add documentation comments

#### ✅ Step 2: Implement GapBuffer
- [ ] Fix existing bugs (destructor, default args)
- [ ] Implement missing methods:
  - [ ] `getText()` - extract text from buffer (skip gap)
  - [ ] `getTextRange(start, length)` - substring extraction
  - [ ] `deleteForward()` - delete at cursor
  - [ ] `insertText(string)` - insert multiple chars
  - [ ] `setCursorPosition(pos)` - absolute cursor move
  - [ ] `findNextWordBoundary(pos)` - scan for word boundaries
  - [ ] `findPrevWordBoundary(pos)`
  - [ ] `findNextParagraph(pos)` - scan for \n
  - [ ] `findPrevParagraph(pos)`
  - [ ] `getWordCount()` - count words
  - [ ] `getParagraphCount()` - count \n + 1
- [ ] Write tests for all methods
- [ ] Fix `moveGapToCursor()` right-moving logic

#### ✅ Step 3: Update Presenter
- [ ] Add viewport management
  - [ ] Track `viewportStart`, `viewportEnd`
  - [ ] Method: `updateViewport()` - calculate visible range
  - [ ] Method: `ensureCursorVisible()` - scroll if needed
- [ ] Implement navigation commands
  - [ ] `onArrowLeft()`, `onArrowRight()`
  - [ ] `onCtrlLeft()` - jump to prev word
  - [ ] `onCtrlRight()` - jump to next word
  - [ ] `onHome()` - start of paragraph
  - [ ] `onEnd()` - end of paragraph
  - [ ] `onCtrlHome()` - start of document
  - [ ] `onCtrlEnd()` - end of document
- [ ] Implement file I/O
  - [ ] `saveFile(filename)` - write buffer to file
  - [ ] `loadFile(filename)` - read file into buffer
  - [ ] Track `isDirty` flag (unsaved changes)
- [ ] Word count display
  - [ ] Call `buffer->getWordCount()` periodically
  - [ ] Update status bar

#### ✅ Step 4: Update View
- [ ] Soft line wrapping
  - [ ] Take visible text string
  - [ ] Wrap at terminal width
  - [ ] Render wrapped lines
- [ ] Cursor positioning
  - [ ] Convert linear cursor offset to (row, col)
  - [ ] Handle wrapped lines
- [ ] Status bar
  - [ ] Display: filename | word count | cursor position
  - [ ] Example: "document.txt | Words: 1,234 | Line 42"

#### ✅ Step 5: Testing
- [ ] Unit tests for GapBuffer (all methods)
- [ ] Integration tests for Presenter
- [ ] Manual testing: type a long document, navigate, save/load

---

## Future Phases (Not in MVP)

### Phase 2: Document Structure
- Chapter/section support
- Navigation between chapters
- Outline view

### Phase 3: Styling Layer
```cpp
class StyleManager {
    struct StyleRange {
        int start;
        int end;
        StyleType type;  // BOLD, ITALIC, UNDERLINE
    };

    vector<StyleRange> styles;

    void applyStyle(int start, int end, StyleType type);
    void removeStyle(int start, int end);
    vector<StyleRange> getStylesInRange(int start, int end);
};
```

### Phase 4: Export Formats
- Markdown export (`**bold**`, `_italic_`)
- Fountain export (screenplay format)
- HTML export
- PDF export

---

## Summary: What We're Building

**MVP Goal**: A terminal-based text editor where you can:
1. Type prose continuously
2. Navigate efficiently (word/paragraph jumps)
3. See word count in real-time
4. Save and load files

**Architecture**: Clean MVP pattern with:
- **View** = ncurses rendering
- **Presenter** = coordination and business logic
- **Model** = GapBuffer for efficient text storage

**Key Design Decisions**:
- ✅ Plain text only (no formatting in buffer)
- ✅ Character-stream based (linear cursor position)
- ✅ GapBuffer for efficient typing
- ✅ Interface-based design (can swap buffer implementations)
- ✅ Styling layer comes later (separate from buffer)

**Next Step**: Implement updated IBuffer interface and GapBuffer

---

*Ready to start coding when you are!*