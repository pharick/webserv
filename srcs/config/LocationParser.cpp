#include "LocationParser.hpp"

webserv::LocationParser::LocationParser(const std::string &uri, std::ifstream &fin)
    : _autoindex(false), _client_max_body_size(0) {
  _uri = uri;

  std::string line;
  std::getline(fin, line);

  while (trim(line) != "}") {
    if (line.empty()) {
      std::getline(fin, line);
      continue;
    }

    size_t p = line.find(';');
    if (p == std::string::npos)
      throw ConfigParserErrorException();

    line.erase(p);
    trim(line);
    if (line.empty()) {
      std::getline(fin, line);
      continue;
    }

    _parseParam(line);
    std::getline(fin, line);
  }

  if (_present_keys.find(ROOT) == _present_keys.end() && _present_keys.find(REDIRECT) == _present_keys.end()
      && _present_keys.find(CGI) == _present_keys.end())
    throw ConfigParserErrorException();

  if (_present_keys.size() > 1 && _present_keys.find(REDIRECT) != _present_keys.end())
    throw ConfigParserErrorException();

  if (_methods.empty()) {
    _methods.insert(GET);
    _methods.insert(HEAD);
  } else if (_methods.find(GET) == _methods.end()) {
    throw ConfigParserErrorException();
  }

  if (_methods.find(PUT) != _methods.end() && _present_keys.find(UPLOAD_STORE) == _present_keys.end())
    throw ConfigParserErrorException();
}

webserv::LocationParser::~LocationParser() {}

void webserv::LocationParser::_parseParam(const std::string &str) {
  std::string key_token;
  std::string value;
  std::stringstream in(str);

  in >> key_token;

  t_config_key key;
  try {
    key = LocationConfigKeys::getInstance().find(key_token);
  } catch (LocationConfigKeys::KeyDoesNotExistsException &e) {
    throw ConfigParserErrorException();
  }

  if (_present_keys.find(key) != _present_keys.end())
    throw ConfigParserErrorException();

  std::getline(in, value);
  trim(value);

  if (value.empty())
    throw ConfigParserErrorException();

  if (key == METHODS)
    _parseMethods(value);
  else if (key == REDIRECT)
    _parseRedirect(value);
  else if (key == ROOT)
    _parseRoot(value);
  else if (key == AUTOINDEX)
    _parseAutoindex(value);
  else if (key == INDEX)
    _parseIndexPages(value);
  else if (key == CGI)
    _parseCgi(value);
  else if (key == UPLOAD_STORE)
    _parseUploadStore(value);
  else if (key == CLIENT_MAX_BODY_SIZE)
    _parseClientMaxBodySize(value);
  else
    throw ConfigParserErrorException();

  _present_keys.insert(key);
}

void webserv::LocationParser::_parseMethods(const std::string &str) {
  std::stringstream in(str);
  std::string value;

  if (!_methods.empty())
    throw ConfigParserErrorException();

  while (!in.eof()) {
    in >> value;

    t_method method;
    try {
      method = Methods::getInstance().find(value);
    } catch (Methods::MethodDoesNotExistsException &e) {
      throw ConfigParserErrorException();
    }

    if (_methods.find(method) != _methods.end())
      throw ConfigParserErrorException();

    _methods.insert(method);
  }
}

void webserv::LocationParser::_parseRedirect(const std::string &str) {
  _redirect = str;
}

void webserv::LocationParser::_parseRoot(const std::string &str) {
  struct stat filestat = {};
  if (stat(str.c_str(), &filestat) != 0 || !(filestat.st_mode & S_IFDIR))
    throw ConfigParserErrorException();

  _root = str;

  if (_root.back() != '/')
    _root += "/";
}

void webserv::LocationParser::_parseAutoindex(const std::string &str) {
  if (str != "0" && str != "1")
    throw ConfigParserErrorException();
  else if (str == "1")
    _autoindex = true;
}

void webserv::LocationParser::_parseIndexPages(const std::string &str) {
  std::stringstream in(str);
  std::set<std::string> unique;
  std::string value;

  if (!_index_pages.empty())
    throw ConfigParserErrorException();

  while (!in.eof()) {
    in >> value;
    if (unique.find(value) != unique.end())
      throw ConfigParserErrorException();

    _index_pages.push_back(value);
    unique.insert(value);
  }
}

void webserv::LocationParser::_parseCgi(const std::string &str) {
  struct stat filestat = {};
  if (stat(str.c_str(), &filestat) != 0 || (filestat.st_mode & S_IFDIR))
    throw ConfigParserErrorException();

  _cgi_path = str;
}

void webserv::LocationParser::_parseUploadStore(const std::string &str) {
  struct stat filestat = {};
  if (stat(str.c_str(), &filestat) != 0 || !(filestat.st_mode & S_IFDIR))
    throw ConfigParserErrorException();

  _upload_store = str;

  if (_upload_store.back() != '/')
    _upload_store += "/";
}

void webserv::LocationParser::_parseClientMaxBodySize(const std::string &str) {
  std::stringstream in(str);
  size_t client_max_body_size;

  in >> client_max_body_size;
  if (in.fail() || !in.eof())
    throw ConfigParserErrorException();

  _client_max_body_size = client_max_body_size;
}

const std::string &webserv::LocationParser::getUri() const {
  return (_uri);
}

const webserv::LocationParser::methods &webserv::LocationParser::getMethods() const {
  return (_methods);
}

const std::string &webserv::LocationParser::getRoot() const {
  return (_root);
}

const webserv::LocationParser::index_pages &webserv::LocationParser::getIndexPages() const {
  return (_index_pages);
}

bool webserv::LocationParser::isAutoindex() const {
  return (_autoindex);
}

const std::string &webserv::LocationParser::getUploadStore() const {
  return (_upload_store);
}

const std::string &webserv::LocationParser::getRedirect() const {
  return (_redirect);
}

const std::string &webserv::LocationParser::getCgiPath() const {
  return (_cgi_path);
}

size_t webserv::LocationParser::getClientMaxBodySize() const {
  return (_client_max_body_size);
}

void webserv::LocationParser::print() const {
  std::cout << "methods: ";
  methods::const_iterator methods_it;
  for (methods_it = _methods.begin(); methods_it != _methods.end(); methods_it++) {
    std::cout << *methods_it << " ";
  }
  std::cout << std::endl;

  std::cout << "redirect: " << _redirect << std::endl
            << "root: " << _root << std::endl
            << "autoindex: " << _autoindex << std::endl;

  std::cout << "index: ";
  index_pages::const_iterator index_it;
  for (index_it = _index_pages.begin(); index_it != _index_pages.end(); index_it++) {
    std::cout << *index_it << " ";
  }
  std::cout << std::endl;

  std::cout << "cgi: " << _cgi_path << std::endl
            << "upload_store: " << _upload_store << std::endl;
}
