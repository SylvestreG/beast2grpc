//
// Created by syl on 4/23/20.
//

#include "grpc_wrapper.hh"
#include <grpc++/create_channel.h>
#include <spdlog/spdlog.h>

grpc_wrapper::grpc_wrapper(const std::string &str) {
  _channel = grpc::CreateChannel(str, grpc::InsecureChannelCredentials());
  _db = std::make_shared<grpc::ProtoReflectionDescriptorDatabase>(_channel);
  _pool = std::make_shared<google::protobuf::DescriptorPool>(_db.get());

  std::vector<std::string> svc_list;
  _db->GetServices(&svc_list);

  for (auto &s : svc_list) {
    spdlog::debug("registring {}", s);
    _services.push_back(
        std::make_shared<grpc_service>(_channel, _pool, s));
  }
}
grpc_fn::ptr grpc_wrapper::find(std::string const& service_name, std::string const& fn) {
  for (auto svc: _services)
    if (*svc == service_name)
      return svc->find(fn);

  return grpc_fn::ptr();
}

