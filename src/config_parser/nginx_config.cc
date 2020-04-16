// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include "config_parser/nginx_config.h"
#include "config_parser/nginx_config_parser.h"
#include "config_parser/nginx_config_statement.h"

std::string NginxConfig::ToString(int depth) {
  std::string serialized_config;
  for (const auto& statement : statements_) {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

// function allows you to get any single line parameter from the config file
std::string NginxConfig::GetConfig(std::string key) {
  std::string value = "";

  for (std::vector<std::shared_ptr<NginxConfigStatement>>::iterator
    it = this->statements_.begin(); it != this->statements_.end(); ++it) {

    // for each statement, loop through each of it's segments
    std::string prev = "";
    for (std::vector<std::string>::iterator it2 = (*it)->tokens_.begin();
      it2 != (*it)->tokens_.end(); ++it2) {
      if (prev == key) {
        return *it2;
      }

      prev = *it2;
    }
    
    // recursively enter statements which have children
    if ((*it)->child_block_ != nullptr) {
      std::string inner = (*it)->child_block_->GetConfig(key);
      if (inner.length() > 0) {
        return inner;
      }
    }
  }
  
  return "";
}

// returns the port number in use
short NginxConfig::getPort(char* config_file){
  short port_num = -1;

  // parse out server configuration
  config_parser->Parse(config_file, this);

  try {
    port_num = std::stoi(this->GetConfig("listen"));
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return port_num;
}