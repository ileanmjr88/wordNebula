# Document Format Interface Design

**Status**: Design Phase **Date**: 2026-02-15 **Updated**: 2026-02-15
**Related**: Phase 2+ (File I/O implementation)

## Overview

This document outlines the design for document format support in wordNebula. The
core principle is **distraction-free writing** through separation of content and
presentation. The native `.wnd` (WordNebula Document) format will store content,
document settings, and metadata separately, allowing writers to focus on text
without worrying about formatting.

## Design Philosophy: Distraction-Free Writing

### Core Principle

**Content ≠ Presentation**

Writers should focus on *what* they write, not *how* it looks. Document
formatting (margins, fonts, headers) should be applied *after* writing, not
*during*.

### User Workflow

```
Step 1: Write
        ↓ (focus on content only, minimal distractions)
Step 2: Configure Document Settings
        ↓ (once per document, or use defaults)
Step 3: Export with Applied Formatting
        ↓ (settings applied to content)
     .pdf, .docx, .html, etc.
```

### Separation of Concerns

| Aspect                          | When Applied    | Where Stored         |
| ------------------------------- | --------------- | -------------------- |
| **Content + Inline Formatting** | While writing   | Markdown in document |
| **Document Settings**           | At export time  | Separate JSON object |
| **Metadata**                    | Automatic       | Separate JSON object |
| **Editor State**                | Session restore | Separate JSON object |

This is the philosophy of tools like **iA Writer**, **Ulysses**, and **Draft** -
write first, format later.

### Critical Distinction: Inline vs Document Formatting

wordNebula supports **two types of formatting** with different purposes:

#### 1. Inline Formatting (Part of Content)

**Visible while writing** - semantic markers applied during the creative
process:

- **Bold**, *italic*, ~~strikethrough~~
- Headings (# Chapter 1, ## Section)
- Lists, blockquotes
- Links, code blocks

These are **part of the writing** - minimal, semantic, distraction-free.
Rendered in the terminal using markdown.

**Example while writing:**

```
# Chapter 1

This is **bold** and *italic* text.

- Item 1
- Item 2

> A quote from the character
```

#### 2. Document Settings (Applied at Export)

**Invisible while writing** - presentation concerns for professional output:

- Page margins (1" top, 1.5" left)
- Page width and size
- Headers/footers (with variables like {page}, {title})
- Font family (Courier New, Times Roman)
- Font size (12pt)
- Line spacing (1.5, 2.0)
- Paragraph spacing

These are **layout preferences** that don't affect the writing experience.
They're configured once and applied during export.

**Example in terminal (while writing):**

```
┌────────────────────────────────────┐
│ Chapter 1                          │ ← Heading rendered
│                                    │
│ This is bold and italic text.     │ ← Formatting visible
│                                    │
│ • Item 1                          │ ← List rendered
│ • Item 2                          │
└────────────────────────────────────┘

No margins, no headers, no font menu
Just content with inline formatting
```

**Example when exported to PDF:**

```
┌──────────────────────────────────────────┐
│                                    Page 1│ ← Header applied
│      Chapter 1                           │ ← Larger, centered
│                                           │
│      This is bold and italic text.       │ ← Font applied
│                                           │
│      • Item 1                            │ ← Spacing applied
│      • Item 2                            │
│                                           │
│                            My Novel - p.1 │ ← Footer applied
└──────────────────────────────────────────┘
        ↑ Margins applied (1.5" left, 1" right)
```

### Industry-Specific Templates

Different writing industries have **strict formatting standards**. wordNebula
supports industry templates that automatically apply correct document settings:

#### Screenplay (Film/TV)

**Extremely strict** - page count = runtime (1 page ≈ 1 minute):

- Font: Courier 12pt (industry standard)
- Margins: Left 1.5", Right 1.0", Top/Bottom 1.0"
- Single-spaced
- Specific indentation for dialogue, parentheticals, transitions

**Example:**

```
INT. COFFEE SHOP - DAY

JOHN sits alone. MARY enters.

                    JOHN
            (nervously)
      Would you like to sit down?
```

#### Stage Play

**Theater conventions**:

- Character names: CENTERED and UPPERCASE
- Stage directions: (in parentheses) or *italicized*
- Different margin requirements than screenplays

#### Comic Book Script

**Panel-by-panel** format for artists:

```
PAGE ONE

PANEL 1: Wide shot. City skyline at sunset.
CAPTION: "The city never sleeps..."

PANEL 2: Close-up on John's face.
JOHN: "Where is she?"
```

#### Novel Manuscript (Publishing)

**Standard manuscript format** for submissions:

- Font: Times New Roman 12pt
- Double-spaced
- 1" margins all around
- Header with title, author, page number

#### Prose (Default)

**General writing**:

- Flexible formatting
- Sensible defaults
- Easy to customize

### Template System Design

```json
{
  "documentSettings": {
    "template": "screenplay",  // Template selection

    // Template defines these automatically:
    "margins": { "left": 1.5, "right": 1.0 },
    "typography": {
      "font": "Courier New",
      "fontSize": 12,
      "lineSpacing": 1.0
    },

    // User can override if needed
    "customOverrides": {}
  }
}
```

**Benefits:**

- Writers focus on story, not formatting rules
- Templates ensure industry compliance
- Easy to switch between formats (try as novel, export as screenplay)
- Export meets professional submission standards

______________________________________________________________________

## Native Format: `.wnd` (WordNebula Document)

### File Extension

**`.wnd`** - WordNebula Document

- Short, memorable extension
- Associates with the application
- Human-readable (JSON or YAML)
- Version control friendly

### Format Options

We have two viable approaches, both achieving the same separation of concerns:

1. **Option A: JSON-based** (Recommended)
1. **Option B: Markdown with YAML Frontmatter** (Alternative)

______________________________________________________________________

## Option A: JSON-based Format (Recommended)

### Why JSON?

- **Structured settings**: Complex nested settings (margins, typography) are
  cleaner
- **Separation**: Content is a distinct field, not mixed with metadata
- **Parsing**: Standard `nlohmann/json` library (widely used in C++)
- **Human-readable**: With pretty-print, still readable and editable
- **Type safety**: Easier to validate structure

### Document Structure

```json
{
  "metadata": {
    "title": "My Novel - Chapter 1",
    "author": "John Doe",
    "created": "2026-02-15T10:30:00Z",
    "modified": "2026-02-15T14:22:00Z",
    "wordCount": 1234,
    "version": "1.0"
  },

  "content": "# Chapter 1\n\nThe story begins here.\n\nThis paragraph has **bold** and *italic* text.\n\n> A quote from the narrator.\n\nThird paragraph continues...",

  "contentFormat": "markdown",

  "documentSettings": {
    "template": "novel-manuscript",
    "pageWidth": 80,
    "margins": {
      "top": 1.0,
      "bottom": 1.0,
      "left": 1.5,
      "right": 1.0,
      "unit": "inches"
    },
    "header": {
      "enabled": true,
      "text": "Chapter {chapter} - {title}",
      "alignment": "center"
    },
    "footer": {
      "enabled": true,
      "text": "Page {page}",
      "alignment": "right"
    },
    "typography": {
      "lineSpacing": 2.0,
      "font": "Times New Roman",
      "fontSize": 12,
      "paragraphSpacing": 1.0
    }
  },

  "editorState": {
    "cursorPosition": 42,
    "scrollLine": 10,
    "focusMode": true,
    "theme": "dark"
  }
}
```

### C++ Data Structures

```cpp
/**
 * @file WordNebulaDocument.hpp
 * @brief Data structures for .wnd document format
 */

#pragma once

#include <string>

namespace wnebula {

/**
 * @brief Document metadata (auto-generated and user-provided)
 */
struct DocumentMetadata {
    std::string title;
    std::string author;
    std::string created;     // ISO 8601 timestamp
    std::string modified;    // ISO 8601 timestamp
    int wordCount = 0;
    std::string version = "1.0";
};

/**
 * @brief Page margins for export
 */
struct PageMargins {
    double top = 1.0;
    double bottom = 1.0;
    double left = 1.5;
    double right = 1.0;
    std::string unit = "inches"; // "inches", "cm", "mm"
};

/**
 * @brief Header/footer configuration
 */
struct HeaderFooter {
    bool enabled = false;
    std::string text;                    // Supports variables: {page}, {title}, {author}, {chapter}
    std::string alignment = "center";    // "left", "center", "right"
};

/**
 * @brief Typography settings (applied at export)
 */
struct Typography {
    double lineSpacing = 1.5;
    std::string font = "Courier New";
    int fontSize = 12;
    double paragraphSpacing = 1.0;
};

/**
 * @brief Industry-specific document templates
 */
enum class DocumentTemplate {
    CUSTOM,              // User-defined settings
    PROSE_STANDARD,      // General prose (default)
    NOVEL_MANUSCRIPT,    // Publishing manuscript format
    SCREENPLAY,          // Film/TV script format (Courier 12pt, specific margins)
    STAGE_PLAY,          // Theater script format
    COMIC_SCRIPT,        // Comic book script for artists
    ACADEMIC,            // Academic paper format
    BUSINESS_LETTER      // Business correspondence
};

/**
 * @brief Content format indicator
 */
enum class ContentFormat {
    PLAIN_TEXT,          // Plain text, no formatting
    MARKDOWN,            // Markdown with inline formatting (default)
    FOUNTAIN,            // Fountain format (screenplay)
    HTML                 // HTML markup (advanced)
};

/**
 * @brief Document-level formatting settings
 *
 * These settings are NOT applied during editing - they're used
 * during export to PDF, DOCX, etc. This keeps the writing
 * experience distraction-free.
 *
 * Templates provide industry-standard presets that writers can
 * select, ensuring their exports meet professional standards.
 */
struct DocumentSettings {
    DocumentTemplate template = DocumentTemplate::PROSE_STANDARD;
    int pageWidth = 80;          // Character width for word wrap
    PageMargins margins;
    HeaderFooter header;
    HeaderFooter footer;
    Typography typography;
};

/**
 * @brief Editor state for session persistence
 */
struct EditorState {
    int cursorPosition = 0;
    int scrollLine = 0;
    bool focusMode = true;
    std::string theme = "dark";  // "dark", "light", "sepia"
};

/**
 * @brief Complete WordNebula document
 */
struct WordNebulaDocument {
    DocumentMetadata metadata;
    std::string content;                              // Content with inline formatting (markdown)
    ContentFormat contentFormat = ContentFormat::MARKDOWN;  // Format of content
    DocumentSettings settings;                        // Applied only at export
    EditorState editorState;                          // Restore writing session
};

} // namespace wnebula
```

### Implementation Example

```cpp
/**
 * @file WordNebulaFormat.hpp
 * @brief Native .wnd format handler (JSON-based)
 */

#pragma once

#include "IDocumentFormat.hpp"
#include "WordNebulaDocument.hpp"
#include <nlohmann/json.hpp>

namespace wnebula {

class WordNebulaFormat : public IDocumentFormat {
  public:
    void save(const std::string& content, const std::string& filepath) override;
    std::string load(const std::string& filepath) override;

    [[nodiscard]] std::string getExtension() const override { return ".wnd"; }
    [[nodiscard]] std::string getFormatName() const override { return "wordNebula Document"; }
    [[nodiscard]] bool supportsFormatting() const override { return true; }
    [[nodiscard]] bool canHandle(const std::string& filepath) const override;

    // Extended API for full document handling
    void saveDocument(const WordNebulaDocument& doc, const std::string& filepath);
    WordNebulaDocument loadDocument(const std::string& filepath);

  private:
    nlohmann::json serializeDocument(const WordNebulaDocument& doc);
    WordNebulaDocument deserializeDocument(const nlohmann::json& json);
    std::string getCurrentTimestamp();
    int countWords(const std::string& text);
};

} // namespace wnebula
```

```cpp
/**
 * @file WordNebulaFormat.cpp
 * @brief Implementation of .wnd format handler
 */

#include "WordNebulaFormat.hpp"
#include <fstream>
#include <chrono>
#include <sstream>

namespace wnebula {

void WordNebulaFormat::save(const std::string& content, const std::string& filepath) {
    // Build minimal document with content and metadata
    WordNebulaDocument doc;
    doc.content = content;
    doc.metadata.created = getCurrentTimestamp();
    doc.metadata.modified = getCurrentTimestamp();
    doc.metadata.wordCount = countWords(content);
    doc.settings = DocumentSettings{}; // Default settings

    saveDocument(doc, filepath);
}

void WordNebulaFormat::saveDocument(const WordNebulaDocument& doc, const std::string& filepath) {
    nlohmann::json json = serializeDocument(doc);

    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filepath);
    }

    file << json.dump(2); // Pretty-print with 2-space indent
    file.close();
}

std::string WordNebulaFormat::load(const std::string& filepath) {
    WordNebulaDocument doc = loadDocument(filepath);
    return doc.content;
}

WordNebulaDocument WordNebulaFormat::loadDocument(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filepath);
    }

    nlohmann::json json;
    file >> json;
    file.close();

    return deserializeDocument(json);
}

nlohmann::json WordNebulaFormat::serializeDocument(const WordNebulaDocument& doc) {
    nlohmann::json json;

    // Metadata
    json["metadata"]["title"] = doc.metadata.title;
    json["metadata"]["author"] = doc.metadata.author;
    json["metadata"]["created"] = doc.metadata.created;
    json["metadata"]["modified"] = doc.metadata.modified;
    json["metadata"]["wordCount"] = doc.metadata.wordCount;
    json["metadata"]["version"] = doc.metadata.version;

    // Content (pure text)
    json["content"] = doc.content;

    // Document settings
    json["documentSettings"]["pageWidth"] = doc.settings.pageWidth;
    json["documentSettings"]["margins"]["top"] = doc.settings.margins.top;
    json["documentSettings"]["margins"]["bottom"] = doc.settings.margins.bottom;
    json["documentSettings"]["margins"]["left"] = doc.settings.margins.left;
    json["documentSettings"]["margins"]["right"] = doc.settings.margins.right;
    json["documentSettings"]["margins"]["unit"] = doc.settings.margins.unit;

    json["documentSettings"]["header"]["enabled"] = doc.settings.header.enabled;
    json["documentSettings"]["header"]["text"] = doc.settings.header.text;
    json["documentSettings"]["header"]["alignment"] = doc.settings.header.alignment;

    json["documentSettings"]["footer"]["enabled"] = doc.settings.footer.enabled;
    json["documentSettings"]["footer"]["text"] = doc.settings.footer.text;
    json["documentSettings"]["footer"]["alignment"] = doc.settings.footer.alignment;

    json["documentSettings"]["typography"]["lineSpacing"] = doc.settings.typography.lineSpacing;
    json["documentSettings"]["typography"]["font"] = doc.settings.typography.font;
    json["documentSettings"]["typography"]["fontSize"] = doc.settings.typography.fontSize;
    json["documentSettings"]["typography"]["paragraphSpacing"] = doc.settings.typography.paragraphSpacing;

    // Editor state
    json["editorState"]["cursorPosition"] = doc.editorState.cursorPosition;
    json["editorState"]["scrollLine"] = doc.editorState.scrollLine;
    json["editorState"]["focusMode"] = doc.editorState.focusMode;
    json["editorState"]["theme"] = doc.editorState.theme;

    return json;
}

WordNebulaDocument WordNebulaFormat::deserializeDocument(const nlohmann::json& json) {
    WordNebulaDocument doc;

    // Metadata
    doc.metadata.title = json.value("/metadata/title"_json_pointer, "");
    doc.metadata.author = json.value("/metadata/author"_json_pointer, "");
    doc.metadata.created = json.value("/metadata/created"_json_pointer, "");
    doc.metadata.modified = json.value("/metadata/modified"_json_pointer, "");
    doc.metadata.wordCount = json.value("/metadata/wordCount"_json_pointer, 0);
    doc.metadata.version = json.value("/metadata/version"_json_pointer, "1.0");

    // Content
    doc.content = json.value("/content"_json_pointer, "");

    // Document settings (use defaults if not present)
    doc.settings.pageWidth = json.value("/documentSettings/pageWidth"_json_pointer, 80);
    doc.settings.margins.top = json.value("/documentSettings/margins/top"_json_pointer, 1.0);
    doc.settings.margins.bottom = json.value("/documentSettings/margins/bottom"_json_pointer, 1.0);
    doc.settings.margins.left = json.value("/documentSettings/margins/left"_json_pointer, 1.5);
    doc.settings.margins.right = json.value("/documentSettings/margins/right"_json_pointer, 1.0);

    // ... load rest of settings ...

    // Editor state
    doc.editorState.cursorPosition = json.value("/editorState/cursorPosition"_json_pointer, 0);
    doc.editorState.scrollLine = json.value("/editorState/scrollLine"_json_pointer, 0);
    doc.editorState.focusMode = json.value("/editorState/focusMode"_json_pointer, true);
    doc.editorState.theme = json.value("/editorState/theme"_json_pointer, "dark");

    return doc;
}

bool WordNebulaFormat::canHandle(const std::string& filepath) const {
    return filepath.ends_with(".wnd");
}

// Helper methods
std::string WordNebulaFormat::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

int WordNebulaFormat::countWords(const std::string& text) {
    // Simple word count (split on whitespace)
    int count = 0;
    bool inWord = false;
    for (char c : text) {
        if (std::isspace(c)) {
            inWord = false;
        } else if (!inWord) {
            inWord = true;
            count++;
        }
    }
    return count;
}

} // namespace wnebula
```

______________________________________________________________________

## Option B: Enhanced Markdown Format (Alternative)

### Why Markdown with YAML Frontmatter?

- **Human-readable**: Pure text, editable in any text editor
- **Familiar**: Markdown is widely known and used
- **Git-friendly**: Text diffs work perfectly
- **Simpler**: Less parsing overhead for basic use cases
- **Ecosystem**: Many existing markdown tools and parsers

### Document Structure

```markdown
---
title: My Novel - Chapter 1
author: John Doe
created: 2026-02-15T10:30:00Z
modified: 2026-02-15T14:22:00Z
wordCount: 1234
version: 1.0

documentSettings:
  pageWidth: 80
  margins:
    top: 1.0
    bottom: 1.0
    left: 1.5
    right: 1.0
    unit: inches
  header:
    enabled: true
    text: "Chapter {chapter} - {title}"
    alignment: center
  footer:
    enabled: true
    text: "Page {page}"
    alignment: right
  typography:
    lineSpacing: 1.5
    font: Courier New
    fontSize: 12
    paragraphSpacing: 1.0

editorState:
  cursorPosition: 42
  scrollLine: 10
  focusMode: true
  theme: dark
---

The story begins here.

Second paragraph with some text.

Third paragraph continues the narrative...
```

### Advantages vs JSON

- **More readable**: No JSON syntax, just YAML metadata + plain text
- **Easier to edit**: Can be edited in any text editor or markdown app
- **Portable**: Markdown is universally supported

### Disadvantages vs JSON

- **YAML complexity**: YAML parsing can be tricky (whitespace-sensitive)
- **Mixed format**: Metadata + content in same file (less clean separation)
- **Parsing**: Need YAML parser + markdown parser

______________________________________________________________________

## Recommendation: JSON Format

For wordNebula, **Option A (JSON)** is recommended because:

1. **Cleaner separation**: Content, settings, and metadata are distinct JSON
   objects
1. **Structured settings**: Complex nested settings (margins, typography) are
   more natural in JSON
1. **Easier validation**: JSON schema can validate document structure
1. **Standard library**: `nlohmann/json` is widely used, well-tested
1. **Future-proof**: Easier to extend with new features

However, **Option B (Markdown)** remains viable and could be implemented later
as an import/export format.

______________________________________________________________________

## IDocumentFormat Interface

### Core Interface

```cpp
/**
 * @file IDocumentFormat.hpp
 * @brief Interface for document format import/export
 */

#pragma once

#include <string>
#include <memory>

namespace wnebula {

/**
 * @brief Interface for document format handlers
 *
 * Defines the contract for save/load operations across different
 * document formats. Implementations handle format-specific encoding,
 * metadata, and feature support.
 *
 * @see WordNebulaFormat for .wnd files (JSON-based)
 * @see PlainTextFormat for .txt files
 * @see MarkdownFormat for .md files
 */
class IDocumentFormat {
  public:
    virtual ~IDocumentFormat() = default;

    /**
     * @brief Save content to file in this format
     *
     * @param content The text content to save
     * @param filepath Absolute path to destination file
     * @throws std::runtime_error if save fails
     */
    virtual void save(const std::string& content, const std::string& filepath) = 0;

    /**
     * @brief Load content from file in this format
     *
     * @param filepath Absolute path to source file
     * @return The loaded text content
     * @throws std::runtime_error if load fails or file not found
     */
    virtual std::string load(const std::string& filepath) = 0;

    /**
     * @brief Get file extension for this format (including dot)
     *
     * @return Extension string (e.g., ".txt", ".md", ".wnd")
     */
    [[nodiscard]] virtual std::string getExtension() const = 0;

    /**
     * @brief Get human-readable format name
     *
     * @return Format name (e.g., "Plain Text", "Markdown", "wordNebula Document")
     */
    [[nodiscard]] virtual std::string getFormatName() const = 0;

    /**
     * @brief Check if format supports rich text formatting
     *
     * @return true if format preserves fonts/styles/layout, false for plain text
     */
    [[nodiscard]] virtual bool supportsFormatting() const = 0;

    /**
     * @brief Validate that a file can be handled by this format
     *
     * Checks file extension and optionally file contents.
     *
     * @param filepath Path to file to validate
     * @return true if this format can handle the file
     */
    [[nodiscard]] virtual bool canHandle(const std::string& filepath) const = 0;
};

} // namespace wnebula
```

### Factory Pattern for Format Selection

```cpp
/**
 * @file DocumentFormatFactory.hpp
 * @brief Factory for creating appropriate document format handlers
 */

#pragma once

#include "IDocumentFormat.hpp"
#include <memory>
#include <string>
#include <vector>

namespace wnebula {

/**
 * @brief Factory for creating document format instances
 *
 * Provides centralized format selection based on file extension
 * or explicit format choice.
 */
class DocumentFormatFactory {
  public:
    /**
     * @brief Create format handler based on file extension
     *
     * @param filepath Path to file (extension used for detection)
     * @return Unique pointer to appropriate format handler
     * @throws std::invalid_argument if extension not supported
     */
    static std::unique_ptr<IDocumentFormat> createFromFile(const std::string& filepath);

    /**
     * @brief Create format handler by explicit extension
     *
     * @param extension File extension (e.g., ".md", ".txt", ".wnd")
     * @return Unique pointer to format handler
     * @throws std::invalid_argument if extension not supported
     */
    static std::unique_ptr<IDocumentFormat> createFromExtension(const std::string& extension);

    /**
     * @brief Get list of all supported file extensions
     *
     * @return Vector of supported extensions (e.g., {".wnd", ".txt", ".md"})
     */
    [[nodiscard]] static std::vector<std::string> getSupportedExtensions();

    /**
     * @brief Get list of all supported format names
     *
     * @return Vector of format names (e.g., {"wordNebula Document", "Plain Text"})
     */
    [[nodiscard]] static std::vector<std::string> getSupportedFormats();

    /**
     * @brief Get the default/native format
     *
     * @return Unique pointer to WordNebulaFormat (.wnd)
     */
    static std::unique_ptr<IDocumentFormat> createDefault();
};

} // namespace wnebula
```

______________________________________________________________________

## Export Strategy: Settings Applied at Export Time

### Distraction-Free Writing → Formatted Export

```
┌─────────────────────────────────┐
│  While Writing (.wnd)           │
│  ────────────────────────        │
│  Content: "The story begins..." │
│  Settings: (invisible)          │
│  Display: Plain text in terminal│
└─────────────────────────────────┘
                │
                │ Export
                ▼
┌─────────────────────────────────┐
│  Exported Document              │
│  ──────────────────              │
│  .pdf  → Apply margins, fonts   │
│  .docx → Apply all settings     │
│  .html → Apply styles           │
│  .txt  → Content only           │
└─────────────────────────────────┘
```

### Export Implementations

```cpp
/**
 * @file DocumentExporter.hpp
 * @brief Exports .wnd documents to various formats with settings applied
 */

#pragma once

#include "WordNebulaDocument.hpp"
#include <string>

namespace wnebula {

class DocumentExporter {
  public:
    /**
     * @brief Export to plain text (content only, no settings)
     */
    static void exportToPlainText(const WordNebulaDocument& doc, const std::string& filepath);

    /**
     * @brief Export to Markdown (content only, no settings)
     */
    static void exportToMarkdown(const WordNebulaDocument& doc, const std::string& filepath);

    /**
     * @brief Export to HTML (apply typography settings)
     */
    static void exportToHTML(const WordNebulaDocument& doc, const std::string& filepath);

    /**
     * @brief Export to PDF (apply all document settings)
     *
     * Requires external library or tool (e.g., wkhtmltopdf, Pandoc)
     */
    static void exportToPDF(const WordNebulaDocument& doc, const std::string& filepath);

    /**
     * @brief Export to Word Document (apply all document settings)
     *
     * Requires library like libdocx or Pandoc
     */
    static void exportToDocx(const WordNebulaDocument& doc, const std::string& filepath);

  private:
    static std::string renderHeaderFooter(const std::string& template_text,
                                          int page,
                                          const DocumentMetadata& metadata);
};

} // namespace wnebula
```

### Example: Export to HTML with Settings

```cpp
void DocumentExporter::exportToHTML(const WordNebulaDocument& doc, const std::string& filepath) {
    std::ofstream file(filepath);

    // Apply typography settings as CSS
    file << "<!DOCTYPE html>\n";
    file << "<html>\n<head>\n";
    file << "<style>\n";
    file << "body {\n";
    file << "  font-family: '" << doc.settings.typography.font << "';\n";
    file << "  font-size: " << doc.settings.typography.fontSize << "pt;\n";
    file << "  line-height: " << doc.settings.typography.lineSpacing << ";\n";
    file << "  margin: " << doc.settings.margins.top << "in ";
    file << doc.settings.margins.right << "in ";
    file << doc.settings.margins.bottom << "in ";
    file << doc.settings.margins.left << "in;\n";
    file << "}\n";
    file << "p { margin-bottom: " << doc.settings.typography.paragraphSpacing << "em; }\n";
    file << "</style>\n";
    file << "</head>\n<body>\n";

    // Convert content paragraphs to HTML
    std::stringstream ss(doc.content);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.empty()) {
            file << "<p></p>\n";
        } else {
            file << "<p>" << line << "</p>\n";
        }
    }

    file << "</body>\n</html>\n";
    file.close();
}
```

______________________________________________________________________

## Integration with Presenter

### Current Presenter File I/O (Stubbed)

```cpp
// In WNebulaPresenter.cpp (current implementation)
void WNebulaPresenter::saveFile(const std::string &path) {
    isDirty = false;
    currentFilePath = path;
    // TODO: Actual file writing
}

void WNebulaPresenter::loadFile(const std::string &path) {
    isDirty = false;
    currentFilePath = path;
    // TODO: Actual file reading
}
```

### Future Integration with IDocumentFormat

```cpp
// In WNebulaPresenter.hpp
class WNebulaPresenter {
  private:
    std::unique_ptr<IDocumentFormat> documentFormat;
    WordNebulaDocument currentDocument; // Full document with settings
    // ... existing fields ...
};

// In WNebulaPresenter.cpp
void WNebulaPresenter::saveFile(const std::string &path) {
    try {
        // Use WordNebulaFormat for .wnd files
        if (path.ends_with(".wnd")) {
            auto wnFormat = std::make_unique<WordNebulaFormat>();

            // Update document metadata
            currentDocument.content = model->getText();
            currentDocument.metadata.modified = getCurrentTimestamp();
            currentDocument.metadata.wordCount = model->getWordCount();
            currentDocument.editorState.cursorPosition = model->getCursorPosition();

            // Save full document with settings
            wnFormat->saveDocument(currentDocument, path);
        } else {
            // Export to other formats (apply settings)
            documentFormat = DocumentFormatFactory::createFromFile(path);
            documentFormat->save(model->getText(), path);
        }

        isDirty = false;
        currentFilePath = path;
        spdlog::info("File saved: {}", path);
    } catch (const std::exception& e) {
        spdlog::error("Failed to save file: {}", e.what());
        // TODO: Notify view of error
    }
}

void WNebulaPresenter::loadFile(const std::string &path) {
    try {
        // Load from .wnd format (includes settings)
        if (path.ends_with(".wnd")) {
            auto wnFormat = std::make_unique<WordNebulaFormat>();
            currentDocument = wnFormat->loadDocument(path);

            // Apply document content to model
            model->setText(currentDocument.content);

            // Restore editor state
            model->setCursorPosition(currentDocument.editorState.cursorPosition);

            // Document settings are loaded but not applied (only used for export)
        } else {
            // Import from other formats (content only)
            documentFormat = DocumentFormatFactory::createFromFile(path);
            std::string content = documentFormat->load(path);
            model->setText(content);

            // Create new document with default settings
            currentDocument = WordNebulaDocument{};
            currentDocument.content = content;
            currentDocument.settings = DocumentSettings{}; // Default settings
        }

        isDirty = false;
        currentFilePath = path;
        updateView();
        spdlog::info("File loaded: {}", path);
    } catch (const std::exception& e) {
        spdlog::error("Failed to load file: {}", e.what());
        // TODO: Notify view of error
    }
}
```

______________________________________________________________________

## Implementation Timeline

### Phase 1: Foundation (Current Phase)

- ✅ Presenter stubbed with `saveFile()` and `loadFile()`
- ✅ `isDirty` flag implemented
- ⏳ Define `IDocumentFormat` interface
- ⏳ Define `WordNebulaDocument` data structures
- ⏳ Implement `WordNebulaFormat` (.wnd JSON-based)
- ⏳ Implement `DocumentFormatFactory`
- ⏳ Integrate with Presenter

### Phase 2: Basic Export Formats

- Add `PlainTextFormat` (.txt)
- Add `MarkdownFormat` (.md)
- Add basic `DocumentExporter` (HTML export)
- UI for document settings configuration

### Phase 3: Advanced Export Formats

- PDF export (via Pandoc or HTML → wkhtmltopdf)
- Word Document export (via Pandoc or libdocx)
- ODF export (via Pandoc)

### Phase 4: Advanced Features

- Custom templates for export
- Format conversion tools
- Batch export to multiple formats
- Import from Word/PDF (content extraction)

______________________________________________________________________

## Testing Strategy

### Unit Tests for WordNebulaFormat

```cpp
// tests/test_WordNebulaFormat.cpp
TEST(WordNebulaFormatTest, SaveAndLoadDocument) {
    WordNebulaFormat format;
    WordNebulaDocument doc;
    doc.content = "Hello, World!\nLine 2";
    doc.metadata.title = "Test Document";
    doc.metadata.author = "Test Author";
    doc.settings.pageWidth = 80;

    format.saveDocument(doc, "/tmp/test.wnd");
    WordNebulaDocument loaded = format.loadDocument("/tmp/test.wnd");

    EXPECT_EQ(loaded.content, doc.content);
    EXPECT_EQ(loaded.metadata.title, doc.metadata.title);
    EXPECT_EQ(loaded.settings.pageWidth, doc.settings.pageWidth);
}

TEST(WordNebulaFormatTest, DefaultSettingsApplied) {
    WordNebulaFormat format;
    format.save("Test content", "/tmp/test.wnd");

    WordNebulaDocument loaded = format.loadDocument("/tmp/test.wnd");

    // Should have default settings
    EXPECT_EQ(loaded.settings.pageWidth, 80);
    EXPECT_EQ(loaded.settings.margins.left, 1.5);
}
```

### Factory Tests

```cpp
TEST(DocumentFormatFactoryTest, CreatesCorrectFormat) {
    auto wndFormat = DocumentFormatFactory::createFromExtension(".wnd");
    EXPECT_EQ(wndFormat->getFormatName(), "wordNebula Document");

    auto txtFormat = DocumentFormatFactory::createFromExtension(".txt");
    EXPECT_EQ(txtFormat->getFormatName(), "Plain Text");
}

TEST(DocumentFormatFactoryTest, DefaultIsWordNebula) {
    auto defaultFormat = DocumentFormatFactory::createDefault();
    EXPECT_EQ(defaultFormat->getExtension(), ".wnd");
}
```

### Integration Tests with Presenter

```cpp
TEST(PresenterFileIOTest, SaveAndLoadWordNebulaDocument) {
    WNebulaPresenter presenter;
    presenter.setup(view, model);

    presenter.onInsert('H');
    presenter.onInsert('i');
    presenter.saveFile("/tmp/test.wnd");

    EXPECT_FALSE(presenter.getIsDirty());

    // Load in new presenter instance
    WNebulaPresenter newPresenter;
    newPresenter.setup(view, newModel);
    newPresenter.loadFile("/tmp/test.wnd");

    EXPECT_EQ(newModel->getText(), "Hi");
}

TEST(PresenterFileIOTest, DocumentSettingsPersist) {
    // Test that document settings are saved and restored
    // but not applied during editing
}
```

______________________________________________________________________

## Design Rationale

### Why Separate Content and Settings?

Traditional word processors (MS Word, Google Docs) mix content and formatting,
which leads to:

- **Distraction**: Constantly adjusting fonts, margins while writing
- **Complexity**: Documents become bloated with formatting data
- **Version control issues**: Binary formats with embedded formatting

wordNebula's approach:

- **Content is king**: Pure text, no formatting noise
- **Settings are preferences**: Applied once, used at export
- **Clean separation**: Content, metadata, settings are distinct
- **Focus on writing**: No toolbars, no font menus, just words

### Why JSON over Binary?

- **Human-readable**: Can inspect and edit `.wnd` files in text editor
- **Git-friendly**: Text diffs work, merge conflicts are resolvable
- **Debuggable**: Easy to see what's in a document
- **Portable**: No proprietary binary format
- **Future-proof**: Easy to migrate or extend

### Why Custom Format Instead of .docx?

- **Simplicity**: .docx is 5,000+ pages of spec, complex ZIP structure
- **Control**: We define exactly what we need
- **Performance**: No heavyweight parsing libraries needed
- **Focus**: Optimized for distraction-free writing, not full Office
  compatibility

______________________________________________________________________

## Related Design Patterns

### Strategy Pattern

`IDocumentFormat` is a classic **Strategy Pattern**:

- **Context**: Presenter needs to save/load files
- **Strategy Interface**: `IDocumentFormat`
- **Concrete Strategies**: `WordNebulaFormat`, `PlainTextFormat`,
  `MarkdownFormat`

### Factory Pattern

`DocumentFormatFactory` uses **Factory Pattern**:

- Encapsulates object creation logic
- Selects appropriate format based on file extension
- Allows adding new formats without modifying client code

### Adapter Pattern (Future)

When integrating third-party libraries (e.g., for .docx support), use **Adapter
Pattern**:

```cpp
class WordDocumentFormat : public IDocumentFormat {
  private:
    // Adapter wrapping third-party library
    std::unique_ptr<ExternalDocxLibrary> docxLib;
};
```

______________________________________________________________________

## Open Questions

1. **Markdown vs JSON for native format?**

   - **Decision**: JSON (better for structured settings)
   - **Alternative**: Could support both, auto-detect on load

1. **Should we support simple inline formatting in content?**

   - **Option A**: Pure plain text only
   - **Option B**: Allow simple markdown (bold, italic) in content
   - **Recommendation**: Start with plain text, add markdown support later

1. **How to handle document settings UI?**

   - **Option A**: Preferences dialog (set once, apply to all exports)
   - **Option B**: Per-document settings
   - **Recommendation**: Per-document with sensible defaults

1. **Export dependencies: Pandoc vs custom implementation?**

   - **Option A**: Use Pandoc for exports (requires external tool)
   - **Option B**: Implement exporters directly (more control)
   - **Recommendation**: Start with custom HTML exporter, add Pandoc integration
     later

______________________________________________________________________

## Vision Summary: Professional Writing Tool

wordNebula aims to be a **distraction-free, professional writing tool** that
serves multiple industries:

### The Complete Vision

1. **While Writing**: Clean terminal interface

   - Content with inline markdown formatting (**bold**, *italic*, headings)
   - No visible margins, headers, or layout clutter
   - Focus mode for deep writing sessions
   - Keyboard-driven navigation

1. **Document Configuration**: Industry templates

   - **Screenplay**: Courier 12pt, industry-standard margins
   - **Stage Play**: Theater formatting conventions
   - **Novel Manuscript**: Times New Roman, double-spaced
   - **Comic Script**: Panel-by-panel format
   - **Academic**: Citation styles, proper formatting
   - **Custom**: User-defined settings

1. **Export**: Professional output

   - `.pdf` → Camera-ready with all formatting
   - `.docx` → Microsoft Word compatible
   - `.html` → Web-ready with styling
   - `.txt` → Plain text (content only)
   - `.md` → Pure markdown (content only)

### User Experience Flow

```
┌─────────────────────────────────────────────┐
│ Step 1: Select Template                    │
│ "I'm writing a screenplay"                 │
│ → Loads Courier 12pt, screenplay margins   │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│ Step 2: Write (Distraction-Free)           │
│                                             │
│ INT. COFFEE SHOP - DAY                      │
│                                             │
│ JOHN sits alone. MARY enters.               │
│                                             │
│ No margins, headers, or toolbars visible    │
│ Just content with inline formatting         │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│ Step 3: Export                              │
│ "Export as PDF"                             │
│ → Template settings applied automatically   │
│ → Industry-standard formatting              │
│ → Ready for submission/publication          │
└─────────────────────────────────────────────┘
```

### Why This Matters

**Traditional word processors** (MS Word, Google Docs):

- ❌ Mix content and formatting (distracting)
- ❌ Require constant manual formatting
- ❌ Templates are hard to apply consistently
- ❌ Binary formats difficult to version control

**wordNebula**:

- ✅ Separate content from presentation
- ✅ Industry templates ensure compliance
- ✅ Distraction-free writing environment
- ✅ Plain text format (git-friendly)
- ✅ Professional output without manual formatting

### Target Users

1. **Screenwriters**: Need industry-standard screenplay format
1. **Playwrights**: Need theater script conventions
1. **Novelists**: Need manuscript format for publishers
1. **Comic Writers**: Need script format for artists
1. **Technical Writers**: Need consistent documentation formatting
1. **Students/Academics**: Need proper academic formatting

### Competitive Advantages

| Feature                   | wordNebula | MS Word | Final Draft     | Scrivener |
| ------------------------- | ---------- | ------- | --------------- | --------- |
| Distraction-free          | ✅         | ❌      | ❌              | ✅        |
| Industry templates        | ✅         | Limited | ✅ (screenplay) | Limited   |
| Terminal-based            | ✅         | ❌      | ❌              | ❌        |
| Git-friendly format       | ✅         | ❌      | ❌              | ❌        |
| Separate content/settings | ✅         | ❌      | Partial         | Partial   |
| Open source               | ✅         | ❌      | ❌              | ❌        |
| Free                      | ✅         | ❌      | ❌ ($249)       | ❌ ($49)  |

______________________________________________________________________

## Dependencies

### Required Libraries

- **nlohmann/json**: JSON parsing and serialization
  - Header-only library
  - Modern C++ API
  - Already common in C++ projects

### Optional Libraries (Future)

- **Pandoc**: Document conversion (if using external tool approach)
- **libdocx**: Direct .docx manipulation
- **wkhtmltopdf**: HTML to PDF conversion

______________________________________________________________________

## References

- [Strategy Pattern - Gang of Four](https://en.wikipedia.org/wiki/Strategy_pattern)
- [JSON for Modern C++ (nlohmann/json)](https://github.com/nlohmann/json)
- [YAML Specification](https://yaml.org/spec/)
- [Markdown Specification (CommonMark)](https://commonmark.org/)
- [Office Open XML Specification](https://www.ecma-international.org/publications-and-standards/standards/ecma-376/)
- [OpenDocument Format Specification](https://www.oasis-open.org/standards#opendocumentv1.3)

## Related Design Documents

- [DESIGN_MVP_WORD_PROCESSOR.md](DESIGN_MVP_WORD_PROCESSOR.md) - Overall
  architecture and MVP decisions
- [PHASE1_ARCHITECTURE.md](PHASE1_ARCHITECTURE.md) - Phase 1 implementation
  details
- Phase 2 File I/O implementation (to be created)

______________________________________________________________________

**Last Updated**: 2026-02-15 **Next Review**: When implementing File I/O phase
**Status**: Design approved, ready for implementation
