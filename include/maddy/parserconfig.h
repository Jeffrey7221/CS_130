/*
 * This project is licensed under the MIT license. For more information see the
 * LICENSE file.
 */
// GCOV_EXCL_START
#pragma once

// -----------------------------------------------------------------------------

namespace maddy {

// -----------------------------------------------------------------------------

/**
 * ParserConfig
 *
 * @class
 */
struct ParserConfig
{
  bool isEmphasizedParserEnabled;
  bool isHTMLWrappedInParagraph;

  ParserConfig()
    : isEmphasizedParserEnabled(true)
    , isHTMLWrappedInParagraph(true)
  {}
}; // class ParserConfig

// -----------------------------------------------------------------------------

} // namespace maddy
// GCOV_EXCL_STOP