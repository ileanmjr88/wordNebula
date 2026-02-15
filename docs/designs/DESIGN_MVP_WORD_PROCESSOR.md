# wordNebula: MVP Architecture Design Rationale

**Date**: 2026-02-14 **Status**: Phase 1 - Implementation Ready

______________________________________________________________________

## Project Vision

wordNebula is a **distraction-free terminal-based writing tool** designed for:

- Bloggers drafting posts
- Novelists writing long-form prose
- Screenwriters (future: screenplay format support)
- Playwrights (future: theatrical format support)

### Core Philosophy

**Word Processor, Not Code Editor**

wordNebula is a distraction-free writing tool (like WriteRoom, iA Writer) - NOT
a code editor.

**Key Characteristics:**

- Text flows continuously with soft line wrapping
- Line breaks only at paragraph boundaries (`\n`)
- Focus on prose writing without inline formatting
- Future: Format output via rules/templates (export to Markdown, Fountain, PDF)

______________________________________________________________________

## Architectural Decisions

### Why MVP Pattern Over MVC?

The Model-View-Presenter pattern was chosen for:

1. **Testability**: Presenter contains all business logic, fully decoupled from
   ncurses
1. **View Passivity**: View only renders and forwards input (perfect for ncurses
   architecture)
1. **Clear Ownership**: Presenter coordinates all interactions between Model and
   View
1. **No View-Model Coupling**: View never directly accesses the buffer

### Responsibility Breakdown

| Layer               | Responsibilities                                                                                         |
| ------------------- | -------------------------------------------------------------------------------------------------------- |
| **Model (IBuffer)** | Continuous text storage, cursor position (linear), paragraph tracking (`\n` chars)                       |
| **Presenter**       | Viewport management, word/paragraph navigation, scroll logic, file I/O, "what should View render?"       |
| **View (ncurses)**  | Soft line wrapping (based on terminal width), render visible text, cursor screen position, input capture |

______________________________________________________________________

## Critical Design Decision: Character Stream Buffer

**Design Principle**: The buffer should NOT think in "lines" - that's a View
display concern.

### IBuffer Interface

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

**Key Concept**: Cursor position is a **linear character offset** (0 to length),
NOT (row, col). The View calculates display coordinates from this linear
position based on terminal width and wrapping.

______________________________________________________________________

## Implementation Challenges

### 1. Word Wrapping Ownership

**Decision: View Does Wrapping** (RECOMMENDED)

- View requests text from Presenter: `getTextRange(start, length)`
- View wraps based on current terminal width
- **Pro**: View owns all display logic, clean separation of concerns
- **Con**: Cursor positioning math is complex (linear position → screen
  coordinates)

**Rejected Alternative: Presenter Does Wrapping**

- Would require Presenter to know terminal dimensions
- Violates separation of concerns (Presenter shouldn't know about display)

### 2. Viewport Management

**Presenter Responsibility**:

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

**Open Questions**:

- How many lines of context above/below cursor?
- Center cursor when jumping (search, Ctrl+Home)?
- Smooth scroll or page-based?

### 3. Why GapBuffer for MVP?

**Perfect fit for word processors**:

- Users type continuously in one location (high locality of edits)
- Occasional jumps for navigation/search
- Lots of insert/delete operations at cursor position
- Gap buffer keeps gap near cursor → **O(1) insert/delete** (amortized)

**Alternative Considered**: Simple string buffer

- Easier to implement
- O(n) insertions would be acceptable for MVP
- **Rejected**: Learning opportunity, better performance characteristics

______________________________________________________________________

## Open Design Questions

### Paragraph Representation

**Options**:

- [ ] Single `\n` for paragraph break (KISS approach)
- [ ] Track paragraphs explicitly (vector of strings)
- [ ] How to handle empty paragraphs? (consecutive `\n\n`)

**Recommendation**: Start with simple `\n` approach, refactor if needed.

### Cursor Movement Semantics

When cursor is at `|` in `"Hello |World"`:

| Command                | Behavior?                                     |
| ---------------------- | --------------------------------------------- |
| `moveCursorLeft()`     | Move to space? Or to 'o' in "Hello"?          |
| `moveCursorWordLeft()` | Start of "Hello"? Or stay in "World"?         |
| `Home` key             | Start of paragraph? Or start of wrapped line? |
| `End` key              | End of paragraph? Or end of wrapped line?     |
| `Ctrl+Home`            | Start of document?                            |
| `Ctrl+End`             | End of document?                              |

**Decision Needed**: Define word processor navigation semantics before
implementation.

### Future Formatting Pipeline

**Vision**: "Set of rules and it could format it"

**Options**:

- [ ] Markdown input → formatted output
- [ ] Custom formatting language
- [ ] Export to PDF/HTML/LaTeX
- [ ] Fountain format for screenplays

**Architectural Impact**:

- If pure text in Model → simpler (Phase 1 approach)
- If tracking formatting → need metadata structure (Phase 2+)

**Decision**: Phase 1 keeps Model as plain text. Styling applied as separate
layer in later phases.

### Document Abstraction Layer

**Question**: Should there be a `Document` class above `IBuffer`?

```cpp
class Document {
    IBuffer* buffer;      // Text content
    std::string filePath; // File metadata
    bool isDirty;         // Unsaved changes?
    // Formatting rules?
    // Metadata (title, author)?
};
```

**Decision**: Phase 1 keeps it simple - Presenter manages file metadata
directly. Document abstraction can be added in Phase 2 if needed.

______________________________________________________________________

## Why This Design Matters

### Performance

**Gap Buffer Optimization**:

- Word processors have highly localized edits (typing happens in one spot)
- Gap buffer keeps gap positioned at cursor
- Insert/delete operations: **O(1) amortized**
- Perfect fit for the use case

### Testability

**Presenter Testability**:

- Can test viewport logic without ncurses
- Can test navigation without rendering
- Can mock IBuffer for unit tests
- Business logic fully isolated from UI

### Maintainability

**View Simplicity**:

- View is "dumb" - just renders what it's told
- Swappable (could add GUI view later without changing Model/Presenter)
- ncurses complexity is isolated to View layer
- Clear separation of concerns

______________________________________________________________________

## Phase 1 Scope

**MVP Features** (all included):

- [x] Word/paragraph navigation (Ctrl+arrows)
- [x] Word count display
- [x] Save/load files
- [x] Interface-based buffer design (supports future buffer implementations)

**Implementation Plan**: See [PHASE1_ARCHITECTURE.md](PHASE1_ARCHITECTURE.md)
for detailed implementation breakdown.

______________________________________________________________________

## References

- **Implementation Guide**: [PHASE1_ARCHITECTURE.md](PHASE1_ARCHITECTURE.md)
- **IBuffer Interface**:
  [../../include/Model/IBuffer.hpp](../../include/Model/IBuffer.hpp)
- **GapBuffer Header**:
  [../../include/Model/GapBuffer.hpp](../../include/Model/GapBuffer.hpp)
- **GapBuffer Tests**:
  [../../tests/test_GapBuffer.cpp](../../tests/test_GapBuffer.cpp)
- **Project Guidelines**: [../../CLAUDE.md](../../CLAUDE.md)

______________________________________________________________________

**Document Status**: Design decisions finalized, ready for Phase 1
implementation.
