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
#include "http_listener.hh"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include "http_session.hh"

http_listener::http_listener(net::io_context &io, tcp::endpoint edp, grpc_wrapper& wrapper)
    : _ioc(io), _ep(edp), _acc(_ioc, _ep), _sock(_ioc), _wrap(wrapper) {

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
    std::make_shared<http_session>(std::move(_sock), _wrap)->run();
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
