//
// Created by syl on 4/20/20.
//

#include "proto_reflection_descriptor_database.h"
#include <google/protobuf/empty.pb.h>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/util/type_resolver_util.h>
#include <grpc++/client_context.h>
#include <grpc++/completion_queue.h>
#include <grpc++/create_channel.h>
#include <grpc++/generic/generic_stub.h>
#include <spdlog/spdlog.h>

void *tag(int i) { return (void *)static_cast<intptr_t>(i); }

class Call {
private:
  grpc::CompletionQueue _cq;
  std::shared_ptr<grpc::GenericStub> _stub;
  std::shared_ptr<grpc::Channel> _channel;
  grpc::ClientContext ctx;
  std::unique_ptr<grpc::GenericClientAsyncReaderWriter> _call;
  grpc::ProtoReflectionDescriptorDatabase _reflection_db;
  google::protobuf::DescriptorPool _desc_pool;

  void _Start(grpc::string const &method) {
    _call = _stub->PrepareCall(&ctx, method, &_cq);
    _call->StartCall(tag(1));
    void *got_tag;
    bool ok;
    _cq.Next(&got_tag, &ok);
    GPR_ASSERT(ok);
  }

  void _Write(grpc::string const &input) {
    void *got_tag;
    bool ok;

    gpr_slice s = gpr_slice_from_copied_buffer(input.data(), input.size());
    grpc::Slice req_slice(s, grpc::Slice::STEAL_REF);
    grpc::ByteBuffer send_buffer(&req_slice, 1);
    _call->Write(send_buffer, tag(2));
    _cq.Next(&got_tag, &ok);
    GPR_ASSERT(ok);
  }

  void _WritesDone() {
    void *got_tag;
    bool ok;

    _call->WritesDone(tag(4));
    _cq.Next(&got_tag, &ok);
    GPR_ASSERT(ok);
  }

  bool _Read(grpc::string &response) {
    void *got_tag;
    bool ok;

    grpc::ByteBuffer recv_buffer;
    _call->Read(&recv_buffer, tag(3));

    if (!_cq.Next(&got_tag, &ok) || !ok) {
      return false;
    }
    std::vector<grpc::Slice> slices;
    GPR_ASSERT(recv_buffer.Dump(&slices).ok());

    response.clear();
    for (size_t i = 0; i < slices.size(); i++) {
      response.append(reinterpret_cast<const char *>(slices[i].begin()),
                      slices[i].size());
    }
    return true;
  }

  grpc::Status _Finish() {
    void *got_tag;
    bool ok;
    grpc::Status status;

    _call->Finish(&status, tag(5));
    _cq.Next(&got_tag, &ok);
    GPR_ASSERT(ok);
    return status;
  }

public:
  Call(std::string const &server)
      : _channel(
            grpc::CreateChannel(server, grpc::InsecureChannelCredentials())),
        _reflection_db(_channel), _desc_pool(&_reflection_db) {
    _stub = std::make_shared<grpc::GenericStub>(_channel);
  };

  google::protobuf::DescriptorPool &get_desc_pool() { return _desc_pool; }

  grpc::Status call(grpc::string method, grpc::string const &input,
                    grpc::string &output) {
    _Start(method);
    _Write(input);
    _WritesDone();
    if (!_Read(output)) {
      fprintf(stderr, "Failed to read response.\n");
    }
    return _Finish();
  }
};

class Msg : public google::protobuf::Message {};

int main() {
  spdlog::info("beast2grpc launched");
  google::protobuf::Empty empt;
  grpc::string request;
  Call c("127.0.0.1:3000");
  grpc::string input(empt.SerializeAsString());
  grpc::string output;
  c.call("/com.centreon.broker.Broker/GetVersion", input, output);

  auto _resolver = google::protobuf::util::NewTypeResolverForDescriptorPool(
      "com.centreon.broker", &c.get_desc_pool());

  std::string js;
  auto status = google::protobuf::util::BinaryToJsonString(
      _resolver, "com.centreon.broker/com.centreon.broker.Version", output, &js);
  spdlog::info("/com.centreon.broker.Broker/GetVersion => {1}", status.ok(), status.error_message().as_string());
  std::cout << js << std::endl;
}