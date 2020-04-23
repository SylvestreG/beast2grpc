//
// Created by syl on 4/22/20.
//

#ifndef BEAST2GRPC_HTTP_HTTPLISTENER_HH_
#define BEAST2GRPC_HTTP_HTTPLISTENER_HH_

#include <boost/asio.hpp>
#include "grpc_wrapper.hh"

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
