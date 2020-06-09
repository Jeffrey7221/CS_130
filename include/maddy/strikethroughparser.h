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
 * StrikeThroughParser
 *
 * @class
 */
class StrikeThroughParser : public LineParser
{
public:
  /**
   * Parse
   *
   * From Markdown: `text ~~text~~`
   *
   * To HTML: `text <s>text</s>`
   *
   * @method
   * @param {std::string&} line The line to interpret
   * @return {void}
   */
  void
  Parse(std::string& line) override
  {
    static std::regex re("(?!.*`.*|.*<code>.*)\\~\\~(?!.*`.*|.*<\\/code>.*)([^\\~]*)\\~\\~(?!.*`.*|.*<\\/code>.*)");
    static std::string replacement = "<s>$1</s>";

    line = std::regex_replace(line, re, replacement);
  }
}; // class StrikeThroughParser

// -----------------------------------------------------------------------------

} // namespace maddy
// GCOV_EXCL_STOP