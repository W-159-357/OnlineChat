#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class RPConPool {
public:
	RPConPool(std::size_t poolsize, std::string host, std::string port);
	~RPConPool();
	void Close();
	std::unique_ptr<VarifyService::Stub> getConnection();
	void returnConnection(std::unique_ptr<VarifyService::Stub> context);

private:
	std::atomic<bool> _b_stop;
	std::size_t _poolSize;
	std::string _host;
	std::string _port;
	std::queue<std::unique_ptr<VarifyService::Stub>> _connections;
	std::mutex _mutex;
	std::condition_variable _cond;

};

class VerifyGrpcClient:public Singleton<VerifyGrpcClient>
{
	friend class Singleton<VerifyGrpcClient>;
public:
	GetVarifyRsp GetVerifyCode(std::string email) {
		ClientContext context;
		GetVarifyRsp reply;
		GetVarifyReq request;
		request.set_email(email);

		auto stub = _pool->getConnection();
		Status status = stub->GetVarifyCode(&context, request, &reply);
		if (status.ok()) {
			_pool->returnConnection(std::move(stub));
			return reply;
		}
		else {
			_pool->returnConnection(std::move(stub));
			reply.set_error(ErrorCodes::RPCFailed);
			return reply;
		}
	}

private:
	VerifyGrpcClient();

	std::unique_ptr<RPConPool> _pool;
};

