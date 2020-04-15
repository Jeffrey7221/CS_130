// An nginx config file parser.

#ifndef NGINX_CONFIG_H
#define NGINX_CONFIG_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// the parsed representation of a single config statement
class NginxConfigStatement;

// the parsed representation of the entire config
class NginxConfig {
  public:
    std::string ToString(int depth = 0);
    std::vector<std::shared_ptr<NginxConfigStatement>> statements_;
    std::string GetConfig(std::string key);
};

#endif // NGINX_CONFIG_H