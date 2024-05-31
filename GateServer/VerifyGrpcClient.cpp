#include "VerifyGrpcClient.h"
#include "ConfigMgr.h"

RPConPool::RPConPool(std::size_t poolsize, std::string host, std::string port)
	:_poolSize(poolsize), _host(host), _port(port), _b_stop(false)
{
	for (std::size_t i = 0; i < poolsize; ++i) {
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
			grpc::InsecureChannelCredentials());
		_connections .push(VarifyService::NewStub(channel));
	}
}

RPConPool::~RPConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);
	Close();
	while (!_connections.empty()) {
		_connections.pop();
	}
}

void RPConPool::Close()
{
	_b_stop = true;
	_cond.notify_all();
}

std::unique_ptr<VarifyService::Stub> RPConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this]() {
		if (_b_stop) {
			return true;
		}
		return !_connections.empty();
		});

	if (_b_stop) {
		return nullptr;
	}

	auto context = std::move(_connections.front());
	_connections.pop();
	return context;
}

void RPConPool::returnConnection(std::unique_ptr<VarifyService::Stub> context)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_b_stop) {
		return;
	}

	_connections.push(std::move(context));
	_cond.notify_one();
}

// 构造函数
VerifyGrpcClient::VerifyGrpcClient()
{
	auto& gCfgMgr = ConfigMgr::Inst();
	std::string host = gCfgMgr["VarifyServer"]["Host"];
	std::string port = gCfgMgr["VarifyServer"]["Port"];
	_pool.reset(new RPConPool(5, host, port));			// 初始化五个连接
}
