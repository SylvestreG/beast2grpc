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
#ifndef BEAST2GRPC_HTTP_HTTPSESSION_HH_
#define BEAST2GRPC_HTTP_HTTPSESSION_HH_

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "grpc_wrapper.hh"

namespace net = boost::asio;
using tcp = net::ip::tcp;
using error_code = boost::system::error_code;

namespace beast = boost::beast;
namespace http = boost::beast::http;

class http_session : public std::enable_shared_from_this<http_session> {
private:
  tcp::socket _sock;
  beast::flat_buffer _buffer;
  http::request<http::string_body> _req;
  grpc_wrapper& _wrap;

  void _on_read(error_code& ec, std::size_t bytes);
  void _fail(error_code const& ec, std::string const&err);

public:
  http_session() = delete;
  http_session(http_session const&) = delete;
  http_session& operator=(http_session const&) = delete;

  http_session(tcp::socket socket, grpc_wrapper& wrap);
  void run();
};

#endif // BEAST2GRPC_HTTP_HTTPSESSION_HH_
