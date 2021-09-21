#ifndef WEBSERV_SERVER_HPP
#define WEBSERV_SERVER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <exception>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <ctime>
#include "Connections.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "config/configParser.hpp"
#include "responses/generateStaticResponse.hpp"
#include "responses/generatePutResponse.hpp"
#include "responses/generateRedirectResponse.hpp"
#include "responses/generateCgiResponse.hpp"
#include "defines.hpp"

namespace webserv {

struct Request {
  RequestParser *parser;
  IResponse *response;
  bool disconnect;

  Request(const std::list<ServerParser *> &servers, t_client client);
  ~Request();

  void reset();
};

class Server {
 public:
  typedef std::list<ServerParser *> servers;
  typedef std::map<int, Request *> requests;

 private:
  const servers &_servers;
  Connections _connections;
  requests _requests;

  std::set<std::pair<in_addr_t, in_port_t> > _getServerAddresses() const;
  void _processPollfd(struct pollfd *fds, size_t servers_end, size_t clients_end);
  void _readClientData(int fd);
  void _setServerLocation(int fd);
  void _processRequest(int fd);
  void _sendResponseData(int fd);
  void _disconnectClient(int fd);
  static std::string _generateCookie(const t_client &client);

  Server();
  Server(const Server &src);
  Server &operator=(const Server &other);

 public:
  explicit Server(const std::list<ServerParser *> &servers);
  ~Server();

  void poll();
};

}

#endif // WEBSERV_SERVER_HPP
