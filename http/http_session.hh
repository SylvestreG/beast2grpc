//
// Created by syl on 4/22/20.
//

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
