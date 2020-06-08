#include <iostream>
#include "db/db_handler.h"
#include "db/db_instance.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <mongocxx/database.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>

db_handler::db_handler(std::string dbName, std::string collectionName) {
    db_instance::getInstance()->createPool("mongodb+srv://pauleggtarts:cs188password@wfh-kufww.gcp.mongodb.net/WFH?retryWrites=true&w=majority&minPoolSize=3&maxPoolSize=5&tls=true&tlsAllowInvalidCertificates=true");
    m_dbName = dbName;
    m_collectionName = collectionName;
}

// Write a new markdown submission to the database
int db_handler::insertMarkdown(std::string md) {
    bsoncxx::builder::stream::document document{};
    std::cout << md << std::endl;
    document << "md" << md;
    std::string jsonDoc_ = bsoncxx::to_json(document);
    auto client = db_instance::getInstance()->getClientFromPool();
    return this->insert(jsonDoc_, *client);
}

// Wrapper to get the 10 most recent markdown submissions
std::vector<std::string> db_handler::getTenMarkdowns() {
    auto client = db_instance::getInstance()->getClientFromPool();
    std::vector<std::string> mds = this->DBHelper(*client);
    int end = mds.size();
    int n = 0;
    std::vector<std::string> ret;
    for (std::string md : mds ) {
        if (end - n <= 10) {
            ret.push_back(md);
        }
        n += 1;
    }
    return ret;
}

// DB Read Function -> Private
std::vector<std::string> db_handler::DBHelper(mongocxx::client& m_client) {
    m_db = m_client[m_dbName];
    m_collection = m_db[m_collectionName];
    std::vector<std::string> ret;
    try{
        mongocxx::cursor cursor = m_collection.find({});
        for (auto doc : cursor ) {
            bsoncxx::stdx::string_view view = doc["md"].get_utf8().value;
            std::string md = view.to_string();
            ret.push_back(md);
        }
        return ret;
    } catch (mongocxx::operation_exception e) {
        std::string errInfo = std::string("Error in inserting document, Err Msg : ") + e.what();
        return ret;
    }
}

// DB Create Function -> Private
int db_handler::insert(std::string jsonDoc_, mongocxx::client& m_client) {
    m_db = m_client[m_dbName];
    m_collection = m_db[m_collectionName];
    try {
        // Convert JSON data to document
        auto doc_value = bsoncxx::from_json(jsonDoc_);
        //Insert the document
        auto result = m_collection.insert_one(std::move(doc_value));
    } catch(const bsoncxx::exception& e) {
        std::string errInfo = std::string("Error in converting JSONdata,Err Msg : ") + e.what();
        return -1;
    } catch (mongocxx::bulk_write_exception e) {
        std::string errInfo = std::string("Error in inserting document, Err Msg : ") + e.what();
        return -1;
    }
    std::cout << "successful write" << std::endl;
    return 0;
}