#include "gtest/gtest.h"
#include "config_parser/nginx_config.h"
#include "config_parser/nginx_config_parser.h"
#include "config_parser/nginx_config_statement.h"

// testing the ToString function from our nginx_config class
class ToStringTestFixture : public ::testing::Test {
    protected:
        void SetUp() override {
            empty_statement_config.statements_.emplace_back(new NginxConfigStatement);
            one_token_config.statements_.emplace_back(new NginxConfigStatement);
            one_token_config.statements_.back().get()->tokens_.push_back("token");
            nested_config.statements_.emplace_back(new NginxConfigStatement);
            nested_config.statements_.back().get()->tokens_.push_back("outer_token");
            nested_config.statements_.back().get()->child_block_.reset(new NginxConfig);
            nested_config.statements_.back().get()->child_block_->statements_.emplace_back(new NginxConfigStatement);
            nested_config.statements_.back().get()->child_block_->statements_.back().get()->tokens_.push_back("inner_token");
        }

        NginxConfig empty_config;
        NginxConfig empty_statement_config;
        NginxConfig one_token_config;
        NginxConfig nested_config;
};

// test the case of a newly created NginxConfig
TEST_F(ToStringTestFixture, EmptyConfig) {
    std::string emtpy_string = empty_config.ToString(0);
    EXPECT_EQ(emtpy_string, "");
}

// test the case of a NginxConfig and its empty statement at depth 0
TEST_F(ToStringTestFixture, EmptyStatement) {
    std::string emtpy_statement_string = empty_statement_config.ToString(0);
    EXPECT_EQ(emtpy_statement_string, ";\n");
}

// test the case of a NginxConfig and its empty statement at depth 1
TEST_F(ToStringTestFixture, EmptyStatementDepth1) {
    std::string emtpy_statement_string = empty_statement_config.ToString(1);
    EXPECT_EQ(emtpy_statement_string, "  ;\n");
}

// test the case where there is a token to stringify
TEST_F(ToStringTestFixture, OneToken) {
    std::string one_token_string = one_token_config.ToString(0);
    EXPECT_EQ(one_token_string, "token;\n");
}

// test the case where there is a token to stringify
TEST_F(ToStringTestFixture, NestedToken) {
    std::string nested_string = nested_config.ToString(0);
    EXPECT_EQ(nested_string, "outer_token {\n  inner_token;\n}\n");
}

// testing the GetConfig function from our nginx_config class
class GetConfigTestFixture : public ::testing::Test {
    protected:
        void SetUp() override {
            empty_statement_config.statements_.emplace_back(new NginxConfigStatement);
            two_token_config.statements_.emplace_back(new NginxConfigStatement);
            two_token_config.statements_.back().get()->tokens_.push_back("key");
            two_token_config.statements_.back().get()->tokens_.push_back("value");
            nested_config.statements_.emplace_back(new NginxConfigStatement);
            nested_config.statements_.back().get()->tokens_.push_back("out_key");
            nested_config.statements_.back().get()->tokens_.push_back("out_value");
            nested_config.statements_.back().get()->child_block_.reset(new NginxConfig);
            nested_config.statements_.back().get()->child_block_->statements_.emplace_back(new NginxConfigStatement);
            nested_config.statements_.back().get()->child_block_->statements_.back().get()->tokens_.push_back("inner_key");
            nested_config.statements_.back().get()->child_block_->statements_.back().get()->tokens_.push_back("inner_value");
        }

        NginxConfig empty_config;
        NginxConfig empty_statement_config;
        NginxConfig two_token_config;
        NginxConfig nested_config;
};

// test the case of a newly created NginxConfig
TEST_F(GetConfigTestFixture, EmptyConfig) {
    std::string no_value = empty_config.GetConfig("");
    EXPECT_EQ(no_value, "");
}

// test the case of a NginxConfig and its empty statement
TEST_F(GetConfigTestFixture, EmptyStatement) {
    std::string no_value = empty_statement_config.GetConfig("key");
    EXPECT_EQ(no_value, "");
}

// test the case of an existing key value pair
TEST_F(GetConfigTestFixture, KeyValueConfig) {
    std::string get_value = two_token_config.GetConfig("key");
    EXPECT_EQ(get_value, "value");
}

// test the case of a non-existant key value pair
TEST_F(GetConfigTestFixture, NoKeyValueConfig) {
    std::string get_value = two_token_config.GetConfig("no_key");
    EXPECT_EQ(get_value, "");
}

// test the nested case where our key value pair is unnested
TEST_F(GetConfigTestFixture, OuterKeyValueConfig) {
    std::string nested_outer_value = nested_config.GetConfig("out_key");
    EXPECT_EQ(nested_outer_value, "out_value");
}

// test the nested case where our key value pair is nested
TEST_F(GetConfigTestFixture, NestedToken) {
    std::string nested_inner_value = nested_config.GetConfig("inner_key");
    EXPECT_EQ(nested_inner_value, "inner_value");
}

// testing the GetPort function from our nginx_config class
class GetPortTestFixture : public ::testing::Test  {
    protected:
        NginxConfigParser parser;
        NginxConfig out_config;
};

// test empty file
TEST_F(GetPortTestFixture, EmptyPortConfig) {
    // should cerr an exception
    char input_file[50] = "./example_configs/example_config2";
    short port_num = out_config.GetPort(input_file);
    EXPECT_EQ(port_num, -1);
}

// simply test setting port to 8080
TEST_F(GetPortTestFixture, PortConfig) {
    char input_file[50] = "./example_configs/example_config";
    short port_num = out_config.GetPort(input_file);
    EXPECT_EQ(port_num, 8080);
}

// should throw an exception if there is no port number
TEST_F(GetPortTestFixture, NoPortConfig) {
    // should cerr an exception
    char input_file[50] = "./example_configs/example_config3";
    short port_num = out_config.GetPort(input_file);
    EXPECT_EQ(port_num, -1);
}

// should pass within a nested statement
TEST_F(GetPortTestFixture, NestedPortConfig) {
    char input_file[50] = "./example_configs/example_config4";
    short port_num = out_config.GetPort(input_file);
    EXPECT_EQ(port_num, 8080);
}

// should recongize different port numbers
TEST_F(GetPortTestFixture, PortConfigNum) {
    char input_file[50] = "./example_configs/example_config8";
    short port_num = out_config.GetPort(input_file);
    EXPECT_EQ(port_num, 9000);
}

// should be able to find a single port number among multiple
// right now this should return the first one
TEST_F(GetPortTestFixture, MultiPortConfig) {
    char input_file[50] = "./example_configs/example_config9";
    short port_num = out_config.GetPort(input_file);
    EXPECT_EQ(port_num, 1024);
}