//
// Created by syl on 4/23/20.
//

#ifndef BEAST2GRPC_GRPC_GRPC_SERVICE_HH_
#define BEAST2GRPC_GRPC_GRPC_SERVICE_HH_

#include <google/protobuf/util/type_resolver_util.h>
#include "proto_reflection_descriptor_database.h"
#include "grpc_fn.hh"

namespace pb = google::protobuf;

class grpc_service {
private:
  std::string _name;
  std::string _namespace;
  std::shared_ptr<grpc::Channel> _chan;
  std::shared_ptr<pb::DescriptorPool> _pool;
  std::vector<grpc_fn::ptr> _fns;
  pb::util::TypeResolver *_type;

public:
  typedef std::shared_ptr<grpc_service> ptr;

  grpc_service() = delete;
  grpc_service(grpc_service const &) = delete;
  grpc_service &operator=(grpc_service const &) = delete;

  grpc_service(std::shared_ptr<grpc::Channel> chan, std::shared_ptr<pb::DescriptorPool> pool, std::string name);
  bool operator==(std::string const& name);
  grpc_fn::ptr find(std::string const& service_func);
};

#endif // BEAST2GRPC_GRPC_GRPC_SERVICE_HH_
