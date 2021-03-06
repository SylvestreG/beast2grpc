/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2020, Sylvestre Gallon <ccna.syl@gmail.com>
 *     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *     FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *     DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *     SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *     CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "http_session.hh"
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>

http_session::http_session(tcp::socket socket, grpc_wrapper& wrap)
    : _sock(std::move(socket)), _wrap(wrap) {}

void http_session::run() {
  http::async_read(
      _sock, _buffer, _req,
      [self = shared_from_this()](error_code ec, std::size_t bytes) {
        self->_on_read(ec, bytes);
      });
}
void http_session::_on_read(error_code& ec, std::size_t bytes) {
  if (ec == http::error::end_of_stream) {
    _sock.shutdown(tcp::socket::shutdown_send, ec);
  }

  if (ec)
    return _fail(ec, "read");

  spdlog::debug("request {} : {}", http::to_string(_req.method()),
                _req.target());

  std::vector<std::string> splitted_path;
  boost::split(splitted_path, _req.target(), boost::is_any_of("/"));

  splitted_path.erase(
      std::remove_if(splitted_path.begin(), splitted_path.end(),
                     [](std::string const& s) { return s.empty(); }),
      splitted_path.end());

  // fav ico...
  if (splitted_path.size() == 1 && splitted_path[0] == "favicon.ico") {
    http::response<http::string_body> res{http::status::ok, _req.version()};
    http::write(_sock, res);
    return;
  } else if (splitted_path.size() != 2) {
    http::response<http::string_body> res{http::status::bad_request,
                                          _req.version()};
    res.body()
        .append("you should call the api like that\n")
        .append("http://addr:port/service_name/function");
    http::write(_sock, res);
    return;
  }

  auto fn = _wrap.find(splitted_path[0], splitted_path[1]);

  if (!fn) {
    http::response<http::string_body> res{http::status::bad_request,
                                          _req.version()};
    res.body()
        .append("fn '")
        .append(splitted_path[1])
        .append("' from service '")
        .append(splitted_path[0])
        .append("' does not exist...");
    http::write(_sock, res);
    return;
  }

  if (_req.body().empty())
    _req.body().append("{}");

  http::response<http::string_body> res{http::status::ok, _req.version()};
  res.body().append((*fn)(_req.body())).append("\n");
  http::write(_sock, res);
}

void http_session::_fail(error_code const& ec, std::string const& err) {
  spdlog::error("{0}: {1}", err, ec.message());
}
