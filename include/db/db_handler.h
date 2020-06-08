#ifndef DBHANDLER_H
#define DBHANDLER_H    

#include <iostream>
#include <mongocxx/database.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include "db/db_instance.h"

#include "gtest/gtest.h"

// USAGE:
// db_handler db("WFH", "markdown");
// db.insertMarkdown(<markdown to be inserted>);
// db.getTenMarkdowns();

class db_handler {
    public:
        db_handler(std::string dbName, std::string collectionName);
    
        int insertMarkdown(std::string md);

        std::vector<std::string> getTenMarkdowns();

        // Give our testing class access to private data methods
        friend class DBTestFix_BadDataWriteTest_Test;
        friend class DBTestFix_BadClientWriteTest_Test;

    private:
        std::string m_dbName;
        std::string m_collectionName;
        mongocxx::database m_db;
        mongocxx::collection m_collection;
        int insert (std::string jsonDoc_, mongocxx::client& m_client);
        std::vector<std::string> DBHelper(mongocxx::client& m_client);

};

#endif 