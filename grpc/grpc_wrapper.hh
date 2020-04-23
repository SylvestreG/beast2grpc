//
// Created by syl on 4/23/20.
//

#ifndef BEAST2GRPC_GRPC_GRPCWRAPPER_HH_
#define BEAST2GRPC_GRPC_GRPCWRAPPER_HH_

#include <grpc++/channel.h>
#include "grpc_service.hh"

class grpc_wrapper {
private:
  std::shared_ptr<grpc::Channel> _channel;
  std::shared_ptr<grpc::ProtoReflectionDescriptorDatabase> _db;
  std::shared_ptr<google::protobuf::DescriptorPool> _pool;

  std::vector<grpc_service::ptr> _services;

public:
  grpc_wrapper() = delete;
  grpc_wrapper(grpc_wrapper const &) = delete;
  grpc_wrapper &operator=(grpc_wrapper const &) = delete;

  grpc_wrapper(std::string const &str);

  grpc_fn::ptr find(std::string const&service_name, std::string const& service_func);
};

#endif // BEAST2GRPC_GRPC_GRPCWRAPPER_HH_
