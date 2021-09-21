#ifndef WEBSERV_STATUSES_HPP
#define WEBSERV_STATUSES_HPP

#include <string>
#include <map>

namespace webserv {

class Statuses {
  std::map<unsigned int, std::string> _statuses;

  Statuses();
  ~Statuses();
  Statuses(const Statuses &src);
  Statuses &operator=(const Statuses &other);

 public:
  static Statuses &getInstance();

  const std::string &getReasonPhrase(unsigned int status) const;

  class StatusDoesNotExistsException : public std::exception {
   public:
    virtual const char *what() const throw() {
      return ("Statuses: Status does not exists");
    }
  };
};

}

#endif // WEBSERV_STATUSES_HPP
