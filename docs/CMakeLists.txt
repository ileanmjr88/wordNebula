# ============================================================================
# Doxygen Documentation Configuration
# ============================================================================

find_package(Doxygen REQUIRED dot)

if(DOXYGEN_FOUND)
  # Set Doxygen configuration
  set(DOXYGEN_PROJECT_NAME "wordNebula")
  set(DOXYGEN_PROJECT_BRIEF "A distraction-free terminal-based word processor")
  set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs")
  set(DOXYGEN_GENERATE_HTML YES)
  set(DOXYGEN_GENERATE_LATEX NO)
  set(DOXYGEN_EXTRACT_ALL YES)
  set(DOXYGEN_EXTRACT_PRIVATE YES)
  set(DOXYGEN_EXTRACT_STATIC YES)
  set(DOXYGEN_RECURSIVE YES)
  set(DOXYGEN_HAVE_DOT YES)
  set(DOXYGEN_CALL_GRAPH YES)
  set(DOXYGEN_CALLER_GRAPH YES)
  set(DOXYGEN_UML_LOOK YES)
  set(DOXYGEN_TEMPLATE_RELATIONS YES)
  set(DOXYGEN_INCLUDE_GRAPH YES)
  set(DOXYGEN_INCLUDED_BY_GRAPH YES)
  set(DOXYGEN_COLLABORATION_GRAPH YES)
  set(DOXYGEN_CLASS_DIAGRAMS YES)

  # Define input sources
  set(DOXYGEN_INPUT "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/src"
                    "${PROJECT_SOURCE_DIR}/README.md"
  )

  # File patterns
  set(DOXYGEN_FILE_PATTERNS *.cpp *.hpp *.h *.md)

  # Exclude patterns
  set(DOXYGEN_EXCLUDE_PATTERNS */build/* */tests/*)

  # Use Doxygen awesome theme
  set(DOXYGEN_HTML_EXTRA_STYLESHEET "")

  # Configure Doxyfile
  doxygen_add_docs(
    docs ${DOXYGEN_INPUT} COMMENT "Generating API documentation with Doxygen"
  )

  message(STATUS "Doxygen documentation target created")
  message(STATUS "Run 'make docs' or 'cmake --build build --target docs' "
                 "to generate documentation"
  )
endif()
