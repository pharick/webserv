#include "pageGenerators.hpp"

static std::string addPageLayout(const std::string &title, const std::string &body) {
  std::stringstream out;

  out << "<!DOCTYPE html>"
      << "<html>"
      << "<head>"
      << "<title>" << title << "</title>"
      << "</head>"
      << "<body>" << body << "</body>"
      << "</html>";

  return (out.str());
}

std::stringstream *webserv::generateErrorPage(size_t *len, unsigned short status) {
  std::string title = unsignedLongToString(status) + " " + Statuses::getInstance().getReasonPhrase(status);
  std::string body = "<h1>" + title + "</h1>";
  std::string page = addPageLayout(title, body);
  *len = page.size();
  return (new std::stringstream(page));
}

std::stringstream *webserv::generateAutoindexPage(size_t *len, const std::string &path, const std::string &uri) {
  DIR *dir = opendir(path.c_str());
  std::stringstream body;
  struct dirent *ent;

  body << "<ul>" << std::endl;
  while ((ent = readdir(dir)) != NULL) {
    if (!strcmp(ent->d_name, "."))
      continue;
    body << "<li><a href=\"" << uri;

    if (uri.back() != '/')
      body << "/";

    body << ent->d_name << "\">" << ent->d_name << "</a></li>" << std::endl;
  }
  body << "</ul>" << std::endl;

  std::string page = addPageLayout(uri, body.str());
  *len = page.size();
  return (new std::stringstream(page));
}
