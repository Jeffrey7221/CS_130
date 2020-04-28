#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "logger/logger.h"
#include "http/request.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using ::testing::AtLeast; 
using ::testing::_;

// Test Fixture
class LoggerFix : public::testing::Test {
    protected:
        Logger logger = Logger::getInstance();
};

// Test that normal messages reach our log
TEST_F(LoggerFix, LogNormalTest) {
    try {
        logger.log("THIS IS A TEST LOG", NORMAL);
    } catch (...) {
        EXPECT_FALSE(true);
    }
    EXPECT_TRUE(true);
}

// Test that notification messages reach our log
TEST_F(LoggerFix, LogNotificationTest) {
    try {
        logger.log("THIS IS A TEST LOG", NOTIFICATION);
    } catch (...) {
        EXPECT_FALSE(true);
    }
    EXPECT_TRUE(true);
}

// Test that warning messages reach our log
TEST_F(LoggerFix, LogWarningTest) {
    try {
        logger.log("THIS IS A TEST LOG", WARNING);
    } catch (...) {
        EXPECT_FALSE(true);
    }
    EXPECT_TRUE(true);
}

// Test that error messages reach our log
TEST_F(LoggerFix, LogErrorTest) {
    try {
        logger.log("THIS IS A TEST LOG", ERROR);
    } catch (...) {
        EXPECT_FALSE(true);
    }
    EXPECT_TRUE(true);
}

// Test that critical messages reach our log
TEST_F(LoggerFix, LogCriticalTest) {
    try {
        logger.log("THIS IS A TEST LOG", CRITICAL);
    } catch (...) {
        EXPECT_FALSE(true);
    }
    EXPECT_TRUE(true);
}


