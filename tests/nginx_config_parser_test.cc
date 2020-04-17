#include "gtest/gtest.h"
#include "config_parser/nginx_config.h"
#include "config_parser/nginx_config_parser.h"
#include "config_parser/nginx_config_statement.h"

// testing that the Parse function with a file input works as expected
class FileParseTestFixture : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};

TEST_F(FileParseTestFixture, BasicWorking) {
  bool success = parser.Parse("example_config", &out_config);
  EXPECT_TRUE(success);
}

// test in case of an extra invalid right curly brace
TEST_F(FileParseTestFixture, ExtraRightBrace) {
  bool failure = parser.Parse("example_config1", &out_config);
  EXPECT_FALSE(failure);
}

// test in case of an EOF
TEST_F(FileParseTestFixture, EmptyFile) {
  bool success = parser.Parse("example_config2", &out_config);
  EXPECT_TRUE(success);
}

// test in case of empty curly braces
TEST_F(FileParseTestFixture, EmptyBraces) {
  bool success = parser.Parse("example_config3", &out_config);
  EXPECT_TRUE(success);
}

// test in case of consecutive right curly braces
TEST_F(FileParseTestFixture, ConsecutiveBraces) {
  bool success = parser.Parse("example_config4", &out_config);
  EXPECT_TRUE(success);
}

// test in case of too many left curly braces
TEST_F(FileParseTestFixture, ExtraLeftBrace) {
  bool failure = parser.Parse("example_config5", &out_config);
  EXPECT_FALSE(failure);
}

// test in case of too many right curly braces
TEST_F(FileParseTestFixture, ExtraRightBrace2) {
  bool failure = parser.Parse("example_config6", &out_config);
  EXPECT_FALSE(failure);
}

// test in case of no token before braces
TEST_F(FileParseTestFixture, MissingBraceToken) {
  bool failure = parser.Parse("example_config7", &out_config);
  EXPECT_FALSE(failure);
}

// test in case of multiple token before braces
TEST_F(FileParseTestFixture, MultiBraceToken) {
  bool success = parser.Parse("example_config8", &out_config);
  EXPECT_TRUE(success);
}

// test in case of multiple statements
TEST_F(FileParseTestFixture, MutipleStatements) {
  bool success = parser.Parse("example_config9", &out_config);
  EXPECT_TRUE(success);
}

// test in case of escape quote within quotes
TEST_F(FileParseTestFixture, EscapeQuoteInQuote) {
  bool success = parser.Parse("example_config10", &out_config);
  EXPECT_TRUE(success);
}

// test in case of a missing semi-colon
TEST_F(FileParseTestFixture, MissingSemicolon) {
  bool failure = parser.Parse("example_config11", &out_config);
  EXPECT_FALSE(failure);
}

// test in case of an extra semi-colon
TEST_F(FileParseTestFixture, ExtraSemicolon) {
  bool failure = parser.Parse("example_config12", &out_config);
  EXPECT_FALSE(failure);
}