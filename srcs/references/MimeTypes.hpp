#ifndef WEBSERV_MIME_TYPES_HPP
#define WEBSERV_MIME_TYPES_HPP

#include <map>
#include <string>
#include <exception>

namespace webserv {

class MimeTypes {
  std::map<std::string, std::string> _types;

  MimeTypes();
  ~MimeTypes();
  MimeTypes(const MimeTypes &src);
  MimeTypes &operator=(const MimeTypes &other);

 public:
  static MimeTypes &getInstance();

  const std::string &find(const std::string &extension) const;

  class ExtensionDoesNotExistsException : public std::exception {
   public:
    virtual const char *what() const throw() {
      return ("MimeTypes: Extension does not exists");
    }
  };
};

}

#endif // WEBSERV_MIME_TYPES_HPP
