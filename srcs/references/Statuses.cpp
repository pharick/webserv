#include "Statuses.hpp"

webserv::Statuses &webserv::Statuses::getInstance() {
  static Statuses statuses;
  return (statuses);
}

webserv::Statuses::Statuses() {
  _statuses[100] = "Continue";
  _statuses[101] = "Switching Protocols";
  _statuses[200] = "OK";
  _statuses[201] = "Created";
  _statuses[202] = "Accepted";
  _statuses[203] = "Non-Authoritative Information";
  _statuses[204] = "No Content";
  _statuses[205] = "Reset Content";
  _statuses[206] = "Partial Content";
  _statuses[300] = "Multiple Choices";
  _statuses[301] = "Moved Permanently";
  _statuses[302] = "Found";
  _statuses[303] = "See Other";
  _statuses[304] = "Not Modified";
  _statuses[305] = "Use Proxy";
  _statuses[307] = "Temporary Redirect";
  _statuses[400] = "Bad Request";
  _statuses[401] = "Unauthorized";
  _statuses[402] = "Payment Required";
  _statuses[403] = "Forbidden";
  _statuses[404] = "Not Found";
  _statuses[405] = "Method Not Allowed";
  _statuses[406] = "Not Acceptable";
  _statuses[407] = "Proxy Authentication Required";
  _statuses[408] = "Request Timeout";
  _statuses[409] = "Conflict";
  _statuses[410] = "Gone";
  _statuses[411] = "Length Required";
  _statuses[412] = "Precondition Failed";
  _statuses[413] = "Payload Too Large";
  _statuses[414] = "URI Too Long";
  _statuses[415] = "Unsupported Media Type";
  _statuses[416] = "Range Not Satisfiable";
  _statuses[417] = "Expectation Failed";
  _statuses[426] = "Upgrade Required";
  _statuses[500] = "Internal Server Error";
  _statuses[501] = "Not Implemented";
  _statuses[502] = "Bad Gateway";
  _statuses[503] = "Service Unavailable";
  _statuses[504] = "Gateway Timeout";
  _statuses[505] = "HTTP Version Not Supported";
}

webserv::Statuses::~Statuses() {}

const std::string &webserv::Statuses::getReasonPhrase(unsigned int status) const {
  std::map<unsigned int, std::string>::const_iterator it = _statuses.find(status);

  if (it == _statuses.end())
    throw StatusDoesNotExistsException();

  return (it->second);
}
