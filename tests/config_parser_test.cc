#include "gtest/gtest.h"
#include "config_parser/nginx_config.h"
#include "config_parser/nginx_config_parser.h"
#include "config_parser/nginx_config_statement.h"

// testing port parsing works properly
class NginxConfigPortTestFixture : public ::testing::Test 
{
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};

// simply test setting port to 8080
TEST_F(NginxConfigPortTestFixture, PortConfig) {
  int port_num = -1;
  if (parser.Parse("example_config", &out_config)) {
    port_num = std::stoi(out_config.GetConfig("listen"));
  }
  EXPECT_EQ(port_num, 8080);
}

// should throw an exception if there is no port number
TEST_F(NginxConfigPortTestFixture, PortConfig1) {
  int port_num = -1;
  if (parser.Parse("example_config3", &out_config)) {
    ASSERT_THROW(std::stoi(out_config.GetConfig("listen")), std::exception);
  }
  EXPECT_EQ(port_num, -1);
}

// should pass within a nested statement
TEST_F(NginxConfigPortTestFixture, PortConfig2) {
  int port_num = -1;
  if (parser.Parse("example_config4", &out_config)) {
    port_num = std::stoi(out_config.GetConfig("listen"));
  }
  EXPECT_EQ(port_num, 8080);
}

// should recongize different port numbers
TEST_F(NginxConfigPortTestFixture, PortConfig3) {
  int port_num = -1;
  if (parser.Parse("example_config8", &out_config)) {
    port_num = std::stoi(out_config.GetConfig("listen"));
  }
  EXPECT_EQ(port_num, 9000);
}

// should be able to find a single port number among multiple
// right now this should return the first one
TEST_F(NginxConfigPortTestFixture, PortConfig4) {
  int port_num = -1;
  if (parser.Parse("example_config9", &out_config)) {
    port_num = std::stoi(out_config.GetConfig("listen"));
  }
  EXPECT_EQ(port_num, 1024);
}

// test empty file
TEST_F(NginxConfigPortTestFixture, PortConfig5) {
  int port_num = -1;
  if (parser.Parse("example_config2", &out_config)) {
    ASSERT_THROW(std::stoi(out_config.GetConfig("listen")), std::exception);
  }
  EXPECT_EQ(port_num, -1);
}

// testing that the Parse function works as expected
class NginxConfigParserTestFixture : public ::testing::Test 
{
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};

TEST(NginxConfigParserTest, ExampleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("example_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, SimpleConfig) {
  bool success = parser.Parse("example_config", &out_config);
  EXPECT_TRUE(success);
}

// test in case of an extra invalid right curly brace
TEST_F(NginxConfigParserTestFixture, SimpleConfig1) {
  bool failure = parser.Parse("example_config1", &out_config);
  EXPECT_FALSE(failure);
}

// test in case of an EOF
TEST_F(NginxConfigParserTestFixture, SimpleConfig2) {
  bool success = parser.Parse("example_config2", &out_config);
  EXPECT_TRUE(success);
}

// test in case of empty curly braces
TEST_F(NginxConfigParserTestFixture, SimpleConfig3) {
  bool success = parser.Parse("example_config3", &out_config);
  EXPECT_TRUE(success);
}

// test in case of consecutive right curly braces
TEST_F(NginxConfigParserTestFixture, SimpleConfig4) {
  bool success = parser.Parse("example_config4", &out_config);
  EXPECT_TRUE(success);
}

// test in case of too many left curly braces
TEST_F(NginxConfigParserTestFixture, SimpleConfig5) {
  bool failure = parser.Parse("example_config5", &out_config);
  EXPECT_FALSE(failure);
}

// test in case of too many right curly braces
TEST_F(NginxConfigParserTestFixture, SimpleConfig6) {
  bool failure = parser.Parse("example_config6", &out_config);
  EXPECT_FALSE(failure);
}

// test in case of no token before braces
TEST_F(NginxConfigParserTestFixture, SimpleConfig7) {
  bool failure = parser.Parse("example_config7", &out_config);
  EXPECT_FALSE(failure);
}

// test in case of multiple token before braces
TEST_F(NginxConfigParserTestFixture, SimpleConfig8) {
  bool success = parser.Parse("example_config8", &out_config);
  EXPECT_TRUE(success);
}

// test in case of multiple statements
TEST_F(NginxConfigParserTestFixture, SimpleConfig9) {
  bool success = parser.Parse("example_config9", &out_config);
  EXPECT_TRUE(success);
}

// test in case of escape quote within quotes
TEST_F(NginxConfigParserTestFixture, SimpleConfig10) {
  bool success = parser.Parse("example_config10", &out_config);
  EXPECT_TRUE(success);
}

// test in case of a missing semi-colon
TEST_F(NginxConfigParserTestFixture, SimpleConfig11) {
  bool failure = parser.Parse("example_config11", &out_config);
  EXPECT_FALSE(failure);
}

// test in case of an extra semi-colon
TEST_F(NginxConfigParserTestFixture, SimpleConfig12) {
  bool failure = parser.Parse("example_config12", &out_config);
  EXPECT_FALSE(failure);
}