//
// Created by syl on 4/23/20.
//

#include "grpc_wrapper.hh"
#include "call_rpc.hh"
#include <grpc++/create_channel.h>
#include <spdlog/spdlog.h>

grpc_wrapper::grpc_wrapper(const std::string &str) {
  _channel = grpc::CreateChannel(str, grpc::InsecureChannelCredentials());
  _db = std::make_shared<grpc::ProtoReflectionDescriptorDatabase>(_channel);
  _pool = std::make_shared<google::protobuf::DescriptorPool>(_db.get());

  _db->GetServices(&_services);
}

grpc_wrapper_fn::grpc_wrapper_fn(std::shared_ptr<grpc::Channel> chan,
                                 pb::util::TypeResolver *res, std::string name,
                                 std::string input, std::string output)
    : _channel(chan), _resolver(res), _name(std::move(name)),
      _input_name(std::move(input)), _output_name(std::move(output)) {}

std::string grpc_wrapper_fn::operator()(std::string const &input) {
  call_rpc rpc(_channel);

  std::string bin_input;

  auto status =
      pb::util::JsonToBinaryString(_resolver, _input_name, input, &bin_input);
  if (!status.ok()) {
    spdlog::error("cannot serial {0} in {1}", input, _input_name);
    return "fail";
  }

  std::string output =
      std::move(rpc("/com.centreon.broker.Broker/GetVersion", bin_input));
  std::string js;

  status = pb::util::BinaryToJsonString(_resolver, _output_name, output, &js);
  if (!status.ok()) {
    spdlog::error("cannot serial {0} in {1}", output, _output_name);
    return "fail";
  }

  return js;
}
