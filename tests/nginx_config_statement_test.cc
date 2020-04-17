#include "gtest/gtest.h"
#include "config_parser/nginx_config.h"
#include "config_parser/nginx_config_parser.h"
#include "config_parser/nginx_config_statement.h"

// testing the ToString function from our nginx_config class
class StateToStringTestFixture : public ::testing::Test {
  protected:
    void SetUp() override {
      one_token_statement.tokens_.push_back("token");
      nested_statement.tokens_.push_back("outer_token");
      nested_statement.child_block_.reset(new NginxConfig);
      nested_statement.child_block_->statements_.emplace_back(new NginxConfigStatement);
      nested_statement.child_block_->statements_.back().get()->tokens_.push_back("inner_token");
    }

    NginxConfigStatement emtpy_statement;
    NginxConfigStatement one_token_statement;
    NginxConfigStatement nested_statement;
};

// test the case of a NginxConfig and its empty statement at depth 0
TEST_F(StateToStringTestFixture, EmptyStatement) {

  std::string emtpy_string = emtpy_statement.ToString(0);

  EXPECT_EQ(emtpy_string, ";\n");
}

// test the case of a NginxConfigStatement and its empty statement at depth 1
TEST_F(StateToStringTestFixture, EmptyStatementDepth) {

  std::string emtpy_string = emtpy_statement.ToString(1);

  EXPECT_EQ(emtpy_string, "  ;\n");
}

// test the case where there is a token to stringify
TEST_F(StateToStringTestFixture, OneToken) {

  std::string one_token_string = one_token_statement.ToString(0);

  EXPECT_EQ(one_token_string, "token;\n");
}

// test the case where there is a token to stringify
TEST_F(StateToStringTestFixture, NestedToken) {

  std::string nested_string = nested_statement.ToString(0);

  EXPECT_EQ(nested_string, "outer_token {\n  inner_token;\n}\n");
}