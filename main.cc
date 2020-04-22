//
// Created by syl on 4/20/20.
//

#include "call_rpc.hh"
#include "http/http_listener.hh"
#include "proto_reflection_descriptor_database.h"
#include <boost/asio.hpp>
#include <google/protobuf/empty.pb.h>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/util/type_resolver_util.h>
#include <grpc++/client_context.h>
#include <grpc++/completion_queue.h>
#include <grpc++/create_channel.h>
#include <memory>
#include <spdlog/spdlog.h>

namespace net = boost::asio;

int main() {
  net::io_context ioc;
  auto addr = net::ip::make_address("0.0.0.0");
  uint16_t port(4242);
  std::make_shared<http_listener>(ioc, tcp::endpoint{addr, port})->run();

  ioc.run();

  spdlog::info("beast2grpc launched");
  google::protobuf::Empty empt;
  grpc::string request;
  auto channel =
      grpc::CreateChannel("127.0.0.1:3000", grpc::InsecureChannelCredentials());
  grpc::string input(empt.SerializeAsString());
  grpc::string output;
  call_rpc rpc(channel);
  output = rpc("/com.centreon.broker.Broker/GetVersion", input);

  grpc::ProtoReflectionDescriptorDatabase db(channel);
  google::protobuf::DescriptorPool pool(&db);

  std::vector<grpc::string> _services;
  db.GetServices(&_services);
  for (auto &svc : _services) {
    spdlog::info("find svc {}", svc);
    auto s = pool.FindServiceByName(svc);
    for (int i = 0; i < s->method_count(); i++) {
      auto fn = s->method(i);
      auto inp = fn->input_type()->full_name();
      auto outp = fn->output_type()->full_name();
      spdlog::info("find /{0}/{1}({2}) -> {3}", svc, fn->name(), inp, outp);
    }
  }

  auto _resolver = google::protobuf::util::NewTypeResolverForDescriptorPool(
      "com.centreon.broker", &pool);

  std::string js;
  auto status = google::protobuf::util::BinaryToJsonString(
      _resolver, "com.centreon.broker/com.centreon.broker.Version", output,
      &js);
  spdlog::info("com.centreon.broker.Broker/GetVersion => {1}", status.ok(),
               status.error_message().as_string());
  std::cout << js << std::endl;

  return (EXIT_SUCCESS);
}