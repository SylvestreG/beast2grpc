//
// Created by syl on 4/22/20.
//

#include "call_rpc.hh"

void *tag(int i) { return (void *)static_cast<intptr_t>(i); }

void call_rpc::_Start(grpc::string const &method) {
  _call = _stub.PrepareCall(&_ctx, method, &_cq);
  _call->StartCall(tag(1));
  void *got_tag;
  bool ok;
  _cq.Next(&got_tag, &ok);
  GPR_ASSERT(ok);
}

void call_rpc::_Write(grpc::string const &input) {
  void *got_tag;
  bool ok;

  gpr_slice s = gpr_slice_from_copied_buffer(input.data(), input.size());
  grpc::Slice req_slice(s, grpc::Slice::STEAL_REF);
  grpc::ByteBuffer send_buffer(&req_slice, 1);
  _call->Write(send_buffer, tag(2));
  _cq.Next(&got_tag, &ok);
  GPR_ASSERT(ok);
}

void call_rpc::_WritesDone() {
  void *got_tag;
  bool ok;

  _call->WritesDone(tag(4));
  _cq.Next(&got_tag, &ok);
  GPR_ASSERT(ok);
}

bool call_rpc::_Read(grpc::string &response) {
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

grpc::Status call_rpc::_Finish() {
  void *got_tag;
  bool ok;
  grpc::Status status;

  _call->Finish(&status, tag(5));
  _cq.Next(&got_tag, &ok);
  GPR_ASSERT(ok);
  return status;
}

call_rpc::call_rpc(std::shared_ptr<grpc::Channel> chan)
    : _chan(chan), _stub(_chan) {}

grpc::string call_rpc::operator()(const grpc::string &method,
                                  const grpc::string &input) {
  grpc::string output;

  _Start(method);
  _Write(input);
  _WritesDone();
  if (!_Read(output)) {
    fprintf(stderr, "Failed to read response.\n");
  }
  _Finish();

  return output;
}
