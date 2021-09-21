#include <iostream>
#include <list>
#include "config/configParser.hpp"
#include "Server.hpp"

int main(int argc, char *argv[]) {
  std::string config_path = "webserv.conf";

  if (argc > 2) {
    std::cout << "Too much arguments" << std::endl;
    return (1);
  }

  if (argc == 2)
    config_path = argv[1];

  std::list<webserv::ServerParser *> servers;

  try {
    servers = webserv::parseConfig(config_path);
  } catch (webserv::ConfigParserErrorException &e) {
    std::cout << e.what() << std::endl;
    return (1);
  }

  try {
    webserv::Server server(servers);
    server.poll();
  } catch (webserv::Connections::SocketErrorException &e) {
    std::cout << e.what() << std::endl;
    return (1);
  }

  return 0;
}
