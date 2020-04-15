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
#include "config_parser/nginx_config_statement.h"

// format the statements as strings for printing purposes
std::string NginxConfigStatement::ToString(int depth) {
  std::string serialized_statement;
  for (int i = 0; i < depth; ++i) {
    serialized_statement.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i) {
    if (i != 0) {
      serialized_statement.append(" ");
    }
    serialized_statement.append(tokens_[i]);
  }
  if (child_block_.get() != nullptr) {
    serialized_statement.append(" {\n");
    serialized_statement.append(child_block_->ToString(depth + 1));
    for (int i = 0; i < depth; ++i) {
      serialized_statement.append("  ");
    }
    serialized_statement.append("}");
  } else {
    serialized_statement.append(";");
  }
  serialized_statement.append("\n");
  return serialized_statement;
}
