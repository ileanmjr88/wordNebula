# MVP Architecture Design Discussion - Word Processor

**Date Started**: 2026-02-14
**Status**: Design Phase - Pending Decisions

---

##
I think I am being to ambitious in my mind with this project. But I think now is the perfect time to discuss this. The idea is that this will be a writing tool, I like the idea of being terminal base so it has less distractions and the use keeps their hands on the keyboard more. I envision this tool to be for bloggers, novel writers, screen writers, and play writers. For the last 2 options they will be able to create different characters and specify which character is saying what. They can save it in the format that is an industry standard of that.

I first was thinking the gap buffer would be easy to implement but having a conversation with you makes me understand how important is to design, I do want to start simple and add more. Can we have that conversation.

Is Phase 1 (simple text editor) a good starting point? Yes lets start with a simple text editor, but I am curious if we should start thinking in phase 1 how we will store styling in the sense of bold, underline and other items like that.

For the MVP, which of these is most important to you?

a) Word/paragraph navigation (Ctrl+arrows)
b) Word count display
c) Save/load files
d) All of the above <--- I would like all of this

Should we design the buffer interface NOW to support future screenplay features?
I think the we should design the buffer interface. I thought about making the interface from the start so we can interact with different buffers as needed. For the same example if support screenplays we can implement the best buffer for that and we use the same interface. Atleast that is what I was thinking

Is GapBuffer the right choice for MVP?
Is the GapBuffer not a good choice?

## Core Insight: Word Processor, Not Code Editor

wordNebula is a **distraction-free writing tool** (like WriteRoom, iA Writer) - NOT a code editor.

**Key Differences:**
- Text flows continuously (soft line wrapping)
- Line breaks only at paragraph boundaries (`\n`)
- Focus on prose writing without formatting
- Future: Format output via rules/templates (Markdown? Custom?)

**This changes everything about the buffer design.**

---

## MVP Pattern for Word Processor

### Why MVP Over MVC?

1. **Testability**: Presenter has all business logic, decoupled from ncurses
2. **View Passivity**: View just renders + forwards input (perfect for ncurses)
3. **Clear Ownership**: Presenter coordinates everything
4. **No View-Model Coupling**: View never touches buffer directly

### Responsibility Breakdown

| Layer | Owns |
|-------|------|
| **Model (IBuffer)** | Continuous text storage, cursor position (linear), paragraph tracking (`\n` chars) |
| **Presenter** | Viewport management, word/paragraph navigation, scroll logic, file I/O, "what should View render?" |
| **View (ncurses)** | Soft line wrapping (based on terminal width), render visible text, cursor screen position, input capture |

---

## Critical Design Decision: Buffer as Character Stream

**Buffer should NOT think in "lines"** - that's a View display concern.

### Proposed IBuffer Interface

```cpp
class IBuffer {
public:
    // Character operations (at cursor)
    virtual void insertChar(char c) = 0;
    virtual void insertText(const std::string& text) = 0;
    virtual void deleteChar() = 0;        // Backspace
    virtual void deleteForward() = 0;     // Delete key

    // Cursor movement (linear position in character stream)
    virtual void setCursorPosition(int pos) = 0;
    virtual int getCursorPosition() const = 0;
    virtual void moveCursor(int offset) = 0;  // Relative movement

    // Text access
    virtual std::string getText() const = 0;
    virtual std::string getTextRange(int start, int length) const = 0;
    virtual int getLength() const = 0;

    // Word/paragraph boundaries (for smart navigation)
    virtual int findNextWordBoundary(int pos) const = 0;
    virtual int findPrevWordBoundary(int pos) const = 0;
    virtual int findNextParagraph(int pos) const = 0;
    virtual int findPrevParagraph(int pos) const = 0;

    virtual ~IBuffer() = default;
};
```

**Key Concept**: Cursor position is a **linear character offset** (0 to length), NOT (row, col).
The View calculates display coordinates from this linear position.

---

## The Hard Problems

### 1. Word Wrapping - Who Owns It?

**Option A: View Does Wrapping** (RECOMMENDED)
- View asks Presenter: "give me text from position X to Y"
- View wraps based on terminal width
- **Pro**: View owns all display logic
- **Con**: Cursor positioning math is complex (linear → screen coords)

**Option B: Presenter Does Wrapping**
- Presenter calculates line breaks based on terminal width
- Presenter tells View: "render these wrapped lines"
- **Pro**: Easier to test wrapping logic
- **Con**: Presenter needs to know terminal dimensions

### 2. Viewport Management (Presenter Responsibility)

```
Presenter maintains:
├─ viewportStart (char position of first visible char)
├─ viewportEnd (char position of last visible char)
└─ cursorPosition (from Model)

When cursor moves outside viewport:
1. Recalculate viewport window
2. Tell View: "render text[viewportStart:viewportEnd]"
3. Tell View: "cursor offset is (cursorPos - viewportStart)"
```

**Questions:**
- How many lines of context above/below cursor?
- Center cursor when jumping (search, Ctrl+Home)?
- Smooth scroll or page-based?

### 3. GapBuffer Design

**Why GapBuffer is PERFECT for word processors:**
- Users type continuously in one location
- Occasional jumps (navigation, search)
- Lots of insert/delete at cursor
- Gap stays near cursor → O(1) edits

**Current GapBuffer Issues** (from CLAUDE.md):
- Missing `getText()`, `getCursorPosition()`, `deleteText()` implementations
- Destructor not declared in header
- `moveGapToCursor()` incomplete (right-moving logic broken)
- Default argument redefined in .cpp

**Design Question**: Should GapBuffer implement the new IBuffer interface above, or current one?

---

## Open Design Questions

### Q1: Paragraph Representation
- [ ] Single `\n` for paragraph break? (KISS approach)
- [ ] Track paragraphs explicitly? (vector of strings)
- [ ] How to handle empty paragraphs? (consecutive `\n\n`?)

### Q2: Cursor Movement Semantics

When cursor is at `|` in `"Hello |World"`:

| Command | Behavior? |
|---------|-----------|
| `moveCursorLeft()` | Move to space? Or to 'o' in "Hello"? |
| `moveCursorWordLeft()` | Start of "Hello"? Or stay in "World"? |
| `Home` key | Start of paragraph? Or start of wrapped line? |
| `End` key | End of paragraph? Or end of wrapped line? |
| `Ctrl+Home` | Start of document? |
| `Ctrl+End` | End of document? |

Need to define word processor navigation semantics.

### Q3: Future Formatting Pipeline

> "Set of rules and it could format it"

What's the vision?
- [ ] Markdown input → formatted output?
- [ ] Custom formatting language?
- [ ] Export to PDF/HTML/LaTeX?
- [ ] Does formatting metadata live in Model? Or separate?

**Impact on Design**:
- If pure text in Model → simpler
- If tracking formatting → need metadata structure

### Q4: Document Abstraction Layer?

Should there be a `Document` class above `IBuffer`?

```cpp
class Document {
    IBuffer* buffer;      // Text content
    std::string filePath; // File metadata
    bool isDirty;         // Unsaved changes?
    // Formatting rules?
    // Metadata (title, author)?
};
```

Or keep it simple - Presenter manages file metadata separately?

---

## Why This Design Matters

**Gap Buffer Optimization**:
- Word processors have highly localized edits (typing)
- Gap buffer keeps gap near cursor → O(1) inserts/deletes
- Perfect fit for use case

**Presenter Testability**:
- Can test viewport logic without ncurses
- Can test navigation without rendering
- Can mock IBuffer for unit tests

**View Simplicity**:
- View is "dumb" - just renders what it's told
- Swappable (could add GUI view later)
- ncurses complexity is isolated

---

## Next Steps (When Resuming)

1. **Decide on IBuffer interface** - Current or proposed character-stream based?
2. **Define cursor movement semantics** - Word processor navigation behavior
3. **Clarify paragraph representation** - Simple `\n` or explicit tracking?
4. **Design viewport management** - Presenter algorithm for scroll/wrap coordination
5. **Sketch user interaction flow** - User types 'a' → what happens in each MVP layer?
6. **Then implement GapBuffer** - Based on agreed interface

---

## References

- Current (broken) GapBuffer: [include/Model/GapBuffer.hpp](../include/Model/GapBuffer.hpp)
- Tests: [tests/test_GapBuffer.cpp](../tests/test_GapBuffer.cpp)
- IBuffer interface: [include/Model/IBuffer.hpp](../include/Model/IBuffer.hpp)
- Architecture notes: [CLAUDE.md](../CLAUDE.md)
- Previous session: [SESSION_2026-02-14.md](../SESSION_2026-02-14.md)

---

**End of Design Discussion - To Be Continued**
