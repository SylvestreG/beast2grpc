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
#include "grpc_wrapper.hh"
#include "http/http_listener.hh"
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace net = boost::asio;

int main() {
  net::io_context ioc;
  auto addr = net::ip::make_address("0.0.0.0");
  uint16_t port(4242);

  spdlog::set_level(spdlog::level::debug);
  spdlog::info("beast {}:{} => to grpc => {}", addr, port, "127.0.0.1:3000");
  grpc_wrapper wrap("127.0.0.1:3000");
  std::make_shared<http_listener>(ioc, tcp::endpoint{addr, port}, wrap)->run();

  ioc.run();

  return (EXIT_SUCCESS);
}