//
// Created by syl on 4/20/20.
//

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