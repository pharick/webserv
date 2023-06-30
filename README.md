# webserv

**Educational project of an HTTP server in C++. The project was completed at Ecole42.**

## Key Features

- Handles client connections using input/output multiplexing (poll function).
- Serves static files.
- Handles GET, HEAD, POST, and PUSH methods.
- Supports chunked body processing.
- Configurable through a configuration file.
- Allows creating multiple virtual servers and configuring different URL handling based on prefix or extension (location).
- Capable of forwarding requests to CGI scripts.

## Build and Run

make && ./webserv [config_file]
