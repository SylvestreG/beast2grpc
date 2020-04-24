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
