//
// Created by syl on 4/22/20.
//

#include "http_session.hh"
#include <spdlog/spdlog.h>

http_session::http_session(tcp::socket socket) : _sock(std::move(socket)) {}

void http_session::run() {
  http::async_read(
      _sock, _buffer, _req,
      [self = shared_from_this()](error_code ec, std::size_t bytes) {
        self->_on_read(ec, bytes);
      });
}
void http_session::_on_read(error_code &ec, std::size_t bytes) {
  if (ec == http::error::end_of_stream) {
    _sock.shutdown(tcp::socket::shutdown_send, ec);
  }

  if (ec)
    return _fail(ec, "read");

  spdlog::info("request succeed");

  http::response<http::string_body> res{http::status::ok, _req.version()};
  res.body().append("coucou");
  http::write(_sock, res);
}
void http_session::_fail(error_code const &ec, std::string const &err) {
  spdlog::error("{0}: {1}", err, ec.message());
}
