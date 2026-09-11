# webserv

An HTTP/1.1 server written from scratch in C++98 for the École 42 curriculum: one `poll()` loop, an nginx-style configuration file, virtual servers, static files, uploads and CGI.

Educational project of an HTTP server in C++, completed at Ecole42. The server reads a configuration file describing one or more virtual servers, listens on their addresses with non-blocking sockets, parses HTTP/1.1 requests incrementally (including chunked bodies), and answers with static files, directory listings, redirects, upload confirmations, or the output of a CGI program. The config parser, request parser, response streaming and CGI environment are all implemented directly on the POSIX socket API; no third-party libraries are used.

## Highlights

- Single-threaded `poll()` event loop over non-blocking sockets (`Server::poll`, `Connections`). Listening sockets are deduplicated by (host, port), so several virtual servers on one address share a socket; each client fd is polled for `POLLIN` or `POLLOUT` depending on whether it is in the `READ` or `SEND` state, so a slow client never blocks the others.
- Incremental request parsing as an explicit state machine (`RequestParser`): `START_LINE -> HEADERS -> BODY_LENGTH | BODY_CHUNKED_SIZE/DATA -> COMPLETE`. Bytes are appended as they arrive and parsing resumes from the current state, so a request split over any number of `recv()` calls is handled the same way as one that arrives in a single packet.
- Request bodies are streamed to a `mkstemp` temporary file instead of being held in memory. The same file is later `dup2`'d to a CGI program's stdin, or `rename`d into `upload_store` for `PUT`, so large uploads are never copied through RAM. `client_max_body_size` is enforced per location (falling back to the server value); oversized bodies are drained and answered with 413.
- Responses are streamed as well: `IResponse::sendChunk(fd)` sends at most 1 KiB per `POLLOUT` event. A `Response` body is any `std::iostream` (a file, or a generated page in a `stringstream`); `CgiResponse` re-encodes the script's output with `Transfer-Encoding: chunked` because its length is not known up front.
- nginx-like virtual host and location resolution: the server is chosen by (host, port) and then by the `Host` header against `server_names`, falling back to the first server on that address; locations match by extension (`location * gif pdf jpg`), then by exact URI, then by longest prefix.
- Configuration is validated at load time (`config/`): unknown or repeated keys, a location without `root`/`redirect`/`cgi`, `redirect` combined with other keys, `PUT` without `upload_store`, non-existent `root`/`cgi`/`upload_store` paths and duplicated `server_names` are rejected before any socket is opened.
- CGI/1.1 via `fork`/`execv`: stdin and stdout are redirected to files, `SERVER_*`, `REQUEST_METHOD`, `REQUEST_URI`, `PATH_INFO`, `QUERY_STRING`, `CONTENT_*` and one `HTTP_*` variable per request header are exported, the process is run in the script's directory, and a non-zero exit becomes a 500.
- Persistent connections: when a response completes the per-fd `Request` is reset and the socket goes back to `READ`; a malformed request is answered with 400 and the connection closed.

## How it works

```
webserv.conf ──► parseConfig() ──► list<ServerParser*> (each holding LocationParser*)
                                            │
                                            ▼
                       Server ──► Connections (listen fds, client fds, READ/SEND state)
                          │
   poll() loop:           │  POLLIN  on listen fd ─► accept(), new Request(parser, response)
                          │  POLLIN  on client fd ─► recv() ─► RequestParser::addData()
                          │                            └─ isComplete() ─► _processRequest()
                          │                                 ├─ generateErrorResponse    404 / 405 / 413
                          │                                 ├─ generateRedirectResponse 301
                          │                                 ├─ generateCgiResponse      fork/execv ─► CgiResponse
                          │                                 ├─ generateStaticResponse   file / index / autoindex
                          │                                 └─ generatePutResponse      201 / 204
                          │  POLLOUT on client fd ─► IResponse::sendChunk(); on completion reset or close
```

| Path | Role |
|---|---|
| `srcs/main.cpp` | Loads the config (default `webserv.conf`) and runs `Server::poll()` |
| `srcs/Server.{hpp,cpp}` | Event loop, request dispatch, one `Request` (parser + response) per client fd |
| `srcs/Connections.{hpp,cpp}` | `socket`/`bind`/`listen`/`accept`, `O_NONBLOCK`, `SO_REUSEADDR`, builds the `pollfd` array |
| `srcs/RequestParser.{hpp,cpp}` | HTTP request state machine, case-insensitive header map, body-to-file, server and location lookup |
| `srcs/IResponse.hpp`, `Response.*`, `CgiResponse.*` | Chunk-wise sending of status line, headers and body or CGI output |
| `srcs/responses/` | Response generators: static file, autoindex, error page, redirect, PUT, CGI |
| `srcs/config/` | `parseConfig()`, `ServerParser`, `LocationParser` and their validation rules |
| `srcs/references/` | Singleton lookup tables: methods, 41 status reason phrases, 14 MIME types, config keys |
| `srcs/pageGenerators.cpp` | Generated HTML for error pages and directory listings |
| `www/`, `cgi-bin/`, `YoupiBanane/`, `upload/` | Sample sites, CGI scripts and fixtures referenced by the shipped configs |

## Features

- Handles client connections using input/output multiplexing (the `poll` function).
- Serves static files with `Content-Type` derived from the extension, `Content-Length` and `Last-Modified`.
- Handles GET, HEAD, POST and PUT methods.
- Supports chunked body processing (`Transfer-Encoding: chunked`) as well as `Content-Length` bodies.
- Configurable through a configuration file.
- Allows creating multiple virtual servers and configuring different URL handling based on prefix or extension (`location`).
- Per-status custom error pages (`error_page`), with generated pages as fallback.
- Index files and optional autoindex directory listings.
- Per-location `301` redirects.
- File upload with `PUT` into `upload_store` (201 when created, 204 when replaced).
- Capable of forwarding requests to CGI scripts (any executable; the repo ships Python scripts and the 42 `cgi_tester`).
- Sets a `Set-Cookie` session identifier on requests that carry no `Cookie` header, and logs every connection and request to stdout.

## Building and running

Requirements: `clang++` (called by name in the Makefile), `make`, a POSIX system. Compiled with `-Wall -Wextra -Werror -std=c++98`.

```
make && ./webserv [config_file]
```

`config_file` defaults to `webserv.conf`, which binds port 80 (needs privileges). `webserv1.conf` binds ports 5000 and 8080 instead. Other targets: `make clean`, `make fclean`, `make re`.

## Configuration format

```
server {
    host                    0.0.0.0;
    port                    8080;
    server_names            webserv.ru;
    client_max_body_size    100000000;
    error_page              404 www/webserv.ru/error_pages/404.html;

    location / {
        methods             GET;
        root                www/webserv.ru;
        autoindex           1;
        index               index.html;
    }

    location * gif pdf jpg {          # matched by URI extension
        root                www/static;
    }

    location /put_test {
        methods             GET PUT;
        root                upload;
        upload_store        upload;
    }

    location /cgi_env {
        cgi                 cgi-bin/cgi_env.py;
    }
}
```

| Scope | Key | Meaning |
|---|---|---|
| server | `host` | IPv4 address to bind |
| server | `port` | TCP port |
| server | `server_names` | Names matched against the `Host` header; must be unique across servers |
| server | `error_page <code...> <path>` | Custom page for one or more status codes (may be repeated) |
| server / location | `client_max_body_size` | Body limit in bytes (server default 10000000; a location value overrides it) |
| location | `methods` | Subset of `GET HEAD POST PUT`; defaults to `GET HEAD`; must include `GET` if given |
| location | `root` | Existing directory the URI (minus the location prefix) is resolved under |
| location | `index` | Index file names tried in order for directory URIs |
| location | `autoindex` | `1` to generate a listing when no index file exists |
| location | `redirect` | Target of a 301; must be the only key in the location |
| location | `cgi` | Existing executable; the request is passed to it instead of the filesystem |
| location | `upload_store` | Existing directory for `PUT`; required when `PUT` is allowed |

A location is either a prefix (`location /path { ... }`) or an extension list (`location * ext1 ext2 { ... }`). Each location must have at least one of `root`, `redirect` or `cgi`.

## Testing

There is no automated test suite in the repository. `tester` and `cgi-bin/cgi_tester` are the x86_64 macOS binaries distributed with the 42 subject; `webserv.conf` is laid out for them (`/put_test`, `/post_body`, `/directory`, `location * bla`). `cgi-bin/cgi_env.py` and `cgi-bin/cgi_post.py` exercise environment passing and form posting; `cgi-bin/cgi_bad.py` is a deliberately invalid script.

## Limitations / notes

- macOS-oriented: the code uses `st_mtimespec`, `strlcpy`/`strlcat`, and a few libc++ conveniences (`std::string::back()`, `std::ifstream(std::string)`) that compile under `-std=c++98` with Apple clang but not with libstdc++; building on Linux needs small changes. The bundled tester binaries are Mach-O.
- CGI runs synchronously: `waitpid` blocks the event loop until the script exits. CGI output is always sent as `200` with chunked encoding; a `Status:` header from the script is not interpreted.
- No timeouts: idle connections stay open until the client closes them.
- `POST` is only handled on CGI locations.
- Learning project: HTTP/1.1 subset, no TLS, no keep-alive timeouts, single process.

## Context

École 42 `webserv` project: write an HTTP server in C++98 with `poll()`-style I/O multiplexing and no external libraries, usable from a real browser, configurable in an nginx-inspired format, and supporting CGI, uploads and multiple virtual servers.
