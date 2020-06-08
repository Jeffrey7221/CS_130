#ifndef DBINSTANCE_H
#define DBINSTANCE_H    

#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <memory>

class db_instance {
    public:
        static db_instance* getInstance();
        void createPool(std::string uri);
        mongocxx::pool::entry getClientFromPool ();
        
    protected:
        mongocxx::instance m_dbInstance;
        std::unique_ptr<mongocxx::pool> m_client_pool;
        db_instance() {}

};


#endif