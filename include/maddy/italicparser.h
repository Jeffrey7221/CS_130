/*
 * This project is licensed under the MIT license. For more information see the
 * LICENSE file.
 */
// GCOV_EXCL_START
#pragma once

// -----------------------------------------------------------------------------

#include <string>
#include <regex>

#include "maddy/lineparser.h"

// -----------------------------------------------------------------------------

namespace maddy {

// -----------------------------------------------------------------------------

/**
 * ItalicParser
 *
 * @class
 */
class ItalicParser : public LineParser
{
public:
  /**
   * Parse
   *
   * From Markdown: `text *text*`
   *
   * To HTML: `text <i>text</i>`
   *
   * @method
   * @param {std::string&} line The line to interpret
   * @return {void}
   */
  void
  Parse(std::string& line) override
  {
    std::regex re("(?!.*`.*|.*<code>.*)\\*(?!.*`.*|.*<\\/code>.*)([^\\*]*)\\*(?!.*`.*|.*<\\/code>.*)");
    static std::string replacement = "<i>$1</i>";
    line = std::regex_replace(line, re, replacement);
  }
}; // class ItalicParser

// -----------------------------------------------------------------------------

} // namespace maddy
// GCOV_EXCL_STOP