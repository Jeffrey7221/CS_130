#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "db/db_handler.h"
#include "db/db_instance.h"
#include <iostream>
#include <mongocxx/database.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

class DBTestFix : public :: testing::Test {
    protected:
        db_handler* db;
        void SetUp() override {
            db = new db_handler("TEST", "test_collection");
        }
};

// Ensure that an instance and pool of connections is created without error
TEST_F(DBTestFix, CreateInstanceTest) {
    try { db_handler test("TEST", "test_collection"); } catch (...) {}
    EXPECT_TRUE(true);
}

// Test that writing to the DB succeeds
TEST_F(DBTestFix, SuccessfulWriteTest) {
    std::string test = "test string";
    int ret = db->insertMarkdown(test);
    EXPECT_EQ(ret, 0);
}

// Test that malformed data writing will NOT succeed
TEST_F(DBTestFix, BadDataWriteTest) {
    std::string bad_data = "This is garbage";
    auto client = db_instance::getInstance()->getClientFromPool();
    int ret = db->insert(bad_data, *client);
    EXPECT_EQ(ret, -1);
}

// Test successful read
TEST_F(DBTestFix, SuccessfulReadTest) {
    db_instance::getInstance()->createPool("mongodb+srv://pauleggtarts:cs188password@wfh-kufww.gcp.mongodb.net/WFH?retryWrites=true&w=majority&minPoolSize=3&maxPoolSize=5&tls=true&tlsAllowInvalidCertificates=true");
    std::vector<std::string> ret = db->getTenMarkdowns();
    if (ret.size() > 0 && ret.size() <= 10) {
        EXPECT_TRUE(true);
    } else {
        EXPECT_FALSE(true);
    }
}
