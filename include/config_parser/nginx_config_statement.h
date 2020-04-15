// An nginx config file parser.

#ifndef NGINX_CONFIG_STATEMENT_H
#define NGINX_CONFIG_STATEMENT_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class NginxConfig;

// the parsed representation of a single config statement
class NginxConfigStatement {
  public:
    std::string ToString(int depth);
    std::vector<std::string> tokens_;
    std::unique_ptr<NginxConfig> child_block_;
};

#endif // NGINX_CONFIG_STATEMENT_H