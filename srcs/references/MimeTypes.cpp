#include "MimeTypes.hpp"

webserv::MimeTypes &webserv::MimeTypes::getInstance() {
  static MimeTypes types;
  return (types);
}

webserv::MimeTypes::MimeTypes() {
  _types["html"] = "text/html";
  _types["htm"] = "text/html";
  _types["css"] = "text/css";

  _types["gif"] = "image/gif";
  _types["jpeg"] = "image/jpeg";
  _types["jpg"] = "image/jpeg";
  _types["png"] = "image/png";

  _types["pdf"] = "application/pdf";
  _types["zip"] = "application/zip";
  _types["gzip"] = "application/gzip";

  _types["mpeg"] = "audio/mpeg";
  _types["wav"] = "audio/wav";
  _types["mp3"] = "audio/mp3";

  _types["mp4"] = "video/mp4";
}

webserv::MimeTypes::~MimeTypes() {}

const std::string &webserv::MimeTypes::find(const std::string &name) const {
  std::map<std::string, std::string>::const_iterator it = _types.find(name);

  if (it == _types.end())
    throw ExtensionDoesNotExistsException();

  return (it->second);
}
