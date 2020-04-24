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
#include <google/protobuf/util/json_util.h>
#include <spdlog/spdlog.h>
#include "grpc_fn.hh"
#include "call_rpc.hh"

grpc_fn::grpc_fn(std::shared_ptr<grpc::Channel> chan,
                 pb::util::TypeResolver *res, std::string name,
                 std::string input, std::string output)
    : _channel(chan), _resolver(res), _name(std::move(name)),
      _input_name(std::move(input)), _output_name(std::move(output)) {
  spdlog::trace("register method {0}({1}) -> {2}", _name, _input_name,
                _output_name);
}

std::string grpc_fn::operator()(std::string const &input) {
  call_rpc rpc(_channel);

  std::string bin_input;

  auto status =
      pb::util::JsonToBinaryString(_resolver, _input_name, input, &bin_input);
  if (!status.ok()) {
    spdlog::error("cannot serial {0} in {1}", input, _input_name);
    return "fail";
  }

  std::string output = std::move(rpc(_name, bin_input));
  std::string js;

  status = pb::util::BinaryToJsonString(_resolver, _output_name, output, &js);
  if (!status.ok()) {
    spdlog::error("cannot serial {0} in {1}", output, _output_name);
    return "fail";
  }

  return js;
}

bool grpc_fn::operator==(const std::string &name) {
  return (name == _name.substr(_name.rfind('/') + 1));
}
