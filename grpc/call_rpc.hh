//
// Created by syl on 4/22/20.
//

#ifndef BEAST2GRPC_GRPC_CALLRPC_HH_
#define BEAST2GRPC_GRPC_CALLRPC_HH_

#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/completion_queue.h>
#include <grpc++/generic/generic_stub.h>

class call_rpc {
private:
  std::shared_ptr<grpc::Channel> _chan;
  std::unique_ptr<grpc::GenericClientAsyncReaderWriter> _call;
  grpc::ClientContext _ctx;
  grpc::GenericStub _stub;
  grpc::CompletionQueue _cq;

  void _Start(grpc::string const &method);
  void _Write(grpc::string const &input);
  void _WritesDone();
  bool _Read(grpc::string &response);
  grpc::Status _Finish();
public:
  call_rpc(std::shared_ptr<grpc::Channel> _chan);
  grpc::string operator()(grpc::string const& method, grpc::string const&input);
};

#endif // BEAST2GRPC_GRPC_CALLRPC_HH_
