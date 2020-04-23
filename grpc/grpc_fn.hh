//
// Created by syl on 4/23/20.
//

#ifndef BEAST2GRPC_GRPC_GRPC_FN_HH_
#define BEAST2GRPC_GRPC_GRPC_FN_HH_

#include <google/protobuf/util/type_resolver_util.h>
#include <grpc++/channel.h>

namespace pb = google::protobuf;

class grpc_fn {
private:
  std::string _name;
  std::string _input_name;
  std::string _output_name;
  std::shared_ptr<grpc::Channel> _channel;
  pb::util::TypeResolver* _resolver;

public:
  typedef std::shared_ptr<grpc_fn> ptr;

  grpc_fn() = delete;
  grpc_fn(grpc_fn const &) = delete;
  grpc_fn &operator=(grpc_fn const &) = delete;

  grpc_fn(std::shared_ptr<grpc::Channel> chan,
                  pb::util::TypeResolver* res, std::string name,
                  std::string input, std::string output);

  std::string operator()(std::string const& input);
  bool operator==(std::string const& name);
};

#endif // BEAST2GRPC_GRPC_GRPC_FN_HH_
