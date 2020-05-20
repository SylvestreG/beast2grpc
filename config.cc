/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2020, Sylvestre Gallon <ccna.syl@gmail.com>
 *     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "config.hh"
#include <fstream>
#include <functional>
#include <nlohmann/json.hpp>
#include <stdexcept>

using namespace nlohmann;
using namespace std;

config::config(std::filesystem::path& path) : _log_level(spdlog::level::info) {
  _log_level = spdlog::level::info;

  _bind_addr = "0.0.0.0";
  _bind_port = 80;

  string addr_grpc("localhost");
  uint16_t port_grpc = 3000;

  try {
    ifstream ifs(path);
    if (ifs.fail())
      throw std::runtime_error("file does not exist");

    auto jf = json::parse(ifs);

    vector<tuple<string, const char*, function<bool(const json&)>,
                 function<void(const json&, std::string const&)>>>
        keys{
            {"address", "string", &json::is_string,
             [&](const json& js, std::string const key) {
               _bind_addr = js[key];
             }},
            {"port", "integer", &json::is_number_unsigned,
             [&](const json& js, std::string const key) {
               _bind_port = js[key];
             }},
            {"grpc_address", "string", &json::is_string,
             [&addr_grpc](const json& js, std::string const key) {
               addr_grpc = js[key];
             }},
            {"grpc_port", "integer", &json::is_number_unsigned,
             [&port_grpc](const json& js, std::string const key) {
               port_grpc = js[key];
             }},
        };

    for (auto& k : keys) {
      if (!jf.contains(std::get<0>(k))) {
        spdlog::warn(
            "The Json field {} is missing using default config for this field",
            std::get<0>(k));
        continue;
      } else if (!std::get<2>(k)(jf[std::get<0>(k)])) {
        spdlog::warn(
            "The Json field {} type is bad (need to use {}) using default "
            "config for this field",
            std::get<0>(k), std::get<1>(k));
        continue;
      }

      std::get<3>(k)(jf, std::get<0>(k));
    }
  } catch (json::exception const& je) {
    spdlog::warn("Bad config file for {}", path.string());
    spdlog::warn("json parse err => {}", je.what());
    spdlog::warn("using default config");
  }  catch (std::runtime_error const& re) {
    spdlog::warn("Bad config file for {}", path.string());
    spdlog::warn("json parse err => {}", re.what());
    spdlog::warn("using default config");
  }

  _connect_addr = fmt::format("{}:{}", addr_grpc, port_grpc);
}

std::string const& config::get_bind_addr() {
  return _bind_addr;
}

std::string const& config::get_connect_addr() {
  return _connect_addr;
}

std::uint16_t config::get_bind_port() {
  return _bind_port;
}