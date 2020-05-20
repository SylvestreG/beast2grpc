/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2020, Sylvestre Gallon <ccna.syl@gmail.com>
 *     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *     FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *     DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *     SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *     CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
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
