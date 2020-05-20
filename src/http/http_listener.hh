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
#ifndef BEAST2GRPC_HTTP_HTTPLISTENER_HH_
#define BEAST2GRPC_HTTP_HTTPLISTENER_HH_

#include <boost/asio.hpp>
#include "grpc/grpc_wrapper.hh"

namespace net = boost::asio;
using tcp = net::ip::tcp;
using error_code = boost::system::error_code;

class http_listener : public std::enable_shared_from_this<http_listener> {
private:
  net::io_context &_ioc;
  tcp::endpoint _ep;
  tcp::acceptor _acc;
  tcp::socket _sock;
  grpc_wrapper &_wrap;

  void _on_accept(error_code const& ec);
  void _fail(error_code const& ec, std::string const&err);

public:
  http_listener() = delete;
  http_listener(http_listener const&) = delete;
  http_listener& operator=(http_listener const&) = delete;

  http_listener(net::io_context &, tcp::endpoint edp, grpc_wrapper& wrapper);
  void run();
};

#endif // BEAST2GRPC_HTTP_HTTPLISTENER_HH_
