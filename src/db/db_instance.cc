#include <iostream>
#include "db/db_instance.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <memory>

// Singleton instance of the db
db_instance* db_instance::getInstance() {
    static db_instance objMongoDBInstance;
    return &objMongoDBInstance;
}

// Create a pool of connections
void db_instance::createPool(std::string uri_) {
    if (!m_client_pool){
        m_client_pool = (std::unique_ptr<mongocxx::pool>) 
        new mongocxx::pool { mongocxx::uri {uri_},  };
    }
}

// Get a client from the available pool
mongocxx::pool::entry db_instance::getClientFromPool() { 
    return m_client_pool->acquire();
}
