//
// Created by syl on 4/22/20.
//

#include "http_listener.hh"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include "http_session.hh"

http_listener::http_listener(net::io_context &io, tcp::endpoint edp)
    : _ioc(io), _ep(edp), _acc(_ioc, _ep), _sock(_ioc) {

}

void http_listener::_fail(error_code const &ec, std::string const &err) {
  if (ec == net::error::operation_aborted)
    return;

  spdlog::error("{0}: {1}", err, ec.message());
}

void http_listener::_on_accept(error_code const &ec) {
  if (ec) {
    _fail(ec, "accept");
  } else {
    spdlog::info("accepted connection from {0}:{1}",
                 _sock.remote_endpoint().address(),
                 _sock.remote_endpoint().port());
    std::make_shared<http_session>(std::move(_sock))->run();
  }

  _acc.async_accept(_sock, [self = shared_from_this()](error_code ec) {
    self->_on_accept(ec);
  });
}

void http_listener::run() {
  _acc.async_accept(_sock, [self = shared_from_this()](error_code ec) {
    self->_on_accept(ec);
  });
}
