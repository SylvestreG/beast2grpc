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
