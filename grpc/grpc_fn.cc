//
// Created by syl on 4/23/20.
//
#include <google/protobuf/util/json_util.h>
#include <spdlog/spdlog.h>
#include "grpc_fn.hh"
#include "call_rpc.hh"

grpc_fn::grpc_fn(std::shared_ptr<grpc::Channel> chan,
                 pb::util::TypeResolver *res, std::string name,
                 std::string input, std::string output)
    : _channel(chan), _resolver(res), _name(std::move(name)),
      _input_name(std::move(input)), _output_name(std::move(output)) {
  spdlog::trace("register method {0}({1}) -> {2}", _name, _input_name,
                _output_name);
}

std::string grpc_fn::operator()(std::string const &input) {
  call_rpc rpc(_channel);

  std::string bin_input;

  auto status =
      pb::util::JsonToBinaryString(_resolver, _input_name, input, &bin_input);
  if (!status.ok()) {
    spdlog::error("cannot serial {0} in {1}", input, _input_name);
    return "fail";
  }

  std::string output = std::move(rpc(_name, bin_input));
  std::string js;

  status = pb::util::BinaryToJsonString(_resolver, _output_name, output, &js);
  if (!status.ok()) {
    spdlog::error("cannot serial {0} in {1}", output, _output_name);
    return "fail";
  }

  return js;
}

bool grpc_fn::operator==(const std::string &name) {
  return (name == _name.substr(_name.rfind('/') + 1));
}
