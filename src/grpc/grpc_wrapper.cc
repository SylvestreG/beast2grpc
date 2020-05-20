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
#include "grpc_wrapper.hh"
#include <grpc++/create_channel.h>
#include <spdlog/spdlog.h>

grpc_wrapper::grpc_wrapper(const std::string &str) {
  _channel = grpc::CreateChannel(str, grpc::InsecureChannelCredentials());
  _db = std::make_shared<grpc::ProtoReflectionDescriptorDatabase>(_channel);
  _pool = std::make_shared<google::protobuf::DescriptorPool>(_db.get());

  std::vector<std::string> svc_list;
  _db->GetServices(&svc_list);

  if (svc_list.empty()) {
    spdlog::warn("Cannot find grpc services.");
    spdlog::warn("This is probably due to a configuration error.");
    spdlog::warn("Check if your grpc server ({}) is running", str);
  }

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

