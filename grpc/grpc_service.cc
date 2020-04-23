//
// Created by syl on 4/23/20.
//

#include "grpc_service.hh"
#include <spdlog/spdlog.h>

grpc_service::grpc_service(
    std::shared_ptr<grpc::Channel> chan,
    std::shared_ptr<google::protobuf::DescriptorPool> pool, std::string name)
    : _chan(chan), _pool(pool) {

  _type = google::protobuf::util::NewTypeResolverForDescriptorPool(
      "com.centreon.broker", _pool.get());

  if (std::count(name.begin(), name.end(), '.') > 0) {
    _namespace = std::move(name.substr(0, name.rfind('.')));
    _name = std::move(name.substr(name.rfind('.') + 1));
  } else {
    _name = std::move(_name);
  }

  spdlog::debug("using namespace {}", _namespace);

  auto svc = _pool->FindServiceByName(std::move(fmt::format("{}.{}", _namespace, _name)));
  if (!svc) {
    spdlog::error("cannot find svc {}", _name);
  }

  for (std::size_t i = 0; i < svc->method_count(); ++i) {
    std::string method_name =
        std::move(fmt::format("/{}.{}/{}", _namespace, _name, svc->method(i)->name()));
    std::string input = std::move(fmt::format(
        "{}/{}", _namespace, svc->method(i)->input_type()->full_name()));
    std::string output = std::move(fmt::format(
        "{}/{}", _namespace, svc->method(i)->output_type()->full_name()));

    _fns.emplace_back(
        std::make_shared<grpc_fn>(_chan, _type, method_name, input, output));
  }
}

bool grpc_service::operator==(std::string const& name) {
  return (fmt::format("{}.{}", _namespace, _name) == name);
}

grpc_fn::ptr grpc_service::find(std::string const& service_func) {
  for (auto &fn: _fns)
    if (*fn == service_func)
      return fn;

  return grpc_fn::ptr();
}
