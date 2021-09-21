#include "utils.hpp"

std::string webserv::getPath(const webserv::LocationParser &location, const webserv::RequestParser &request) {
  const std::string &location_uri = location.getUri();
  const std::string &uri = request.getUri();

  if (uri==location_uri.substr(0, location_uri.size() - 1))
    return (location.getRoot());

  std::string stripped_uri = uri.substr(location_uri.size());
  std::string path = location.getRoot() + stripped_uri;
  return (path);
}

std::string webserv::getUploadPath(const webserv::LocationParser &location, const webserv::RequestParser &request) {
  const std::string &location_uri = location.getUri();
  const std::string &uri = request.getUri();

  if (uri==location_uri.substr(0, location_uri.size() - 1))
    return (location.getUploadStore());

  std::string stripped_uri = uri.substr(location_uri.size());
  std::string path = location.getUploadStore() + stripped_uri;
  return (path);
}

std::string webserv::findIndex(const std::string &path, const webserv::LocationParser::index_pages &index_pages) {
  struct stat filestat = {};
  std::list<std::string>::const_iterator index_it;
  for (index_it = index_pages.begin(); index_it!=index_pages.end(); index_it++) {
    std::string index_path = path;
    if (index_path.back()!='/')
      index_path += "/";
    index_path += *index_it;
    if (stat(index_path.c_str(), &filestat)==0)
      return (index_path);
  }
  return ("");
}
