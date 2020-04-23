//
// Created by syl on 4/23/20.
//

#ifndef BEAST2GRPC_GRPC_GRPCWRAPPER_HH_
#define BEAST2GRPC_GRPC_GRPCWRAPPER_HH_

#include "proto_reflection_descriptor_database.h"
#include <google/protobuf/util/type_resolver_util.h>
#include <grpc++/channel.h>

namespace pb = google::protobuf;

class grpc_wrapper_fn {
private:
  std::string _name;
  std::string _input_name;
  std::string _output_name;
  std::shared_ptr<grpc::Channel> _channel;
  pb::util::TypeResolver* _resolver;

public:
  grpc_wrapper_fn() = delete;
  grpc_wrapper_fn(grpc_wrapper_fn const &) = delete;
  grpc_wrapper_fn &operator=(grpc_wrapper_fn const &) = delete;

  grpc_wrapper_fn(std::shared_ptr<grpc::Channel> chan,
                  pb::util::TypeResolver* res, std::string name,
                  std::string input, std::string output);

  std::string operator()(std::string const& input);
};

class grpc_wrapper_service {
private:
};

class grpc_wrapper {
private:
  std::shared_ptr<grpc::Channel> _channel;
  std::shared_ptr<grpc::ProtoReflectionDescriptorDatabase> _db;
  std::shared_ptr<google::protobuf::DescriptorPool> _pool;

  std::vector<std::string> _services;

public:
  grpc_wrapper() = delete;
  grpc_wrapper(grpc_wrapper const &) = delete;
  grpc_wrapper &operator=(grpc_wrapper const &) = delete;

  grpc_wrapper(std::string const &str);
};

#endif // BEAST2GRPC_GRPC_GRPCWRAPPER_HH_
