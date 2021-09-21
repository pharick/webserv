#ifndef WEBSERV_CONFIG_KEYS_HPP
#define WEBSERV_CONFIG_KEYS_HPP

namespace webserv {

typedef enum e_config_key {
  HOST,
  PORT,
  SERVER_NAMES,
  ERROR_PAGES,
  CLIENT_MAX_BODY_SIZE,
  METHODS,
  REDIRECT,
  ROOT,
  AUTOINDEX,
  INDEX,
  CGI,
  UPLOAD_STORE
} t_config_key;

}

#endif // WEBSERV_CONFIG_KEYS_HPP
