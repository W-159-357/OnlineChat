#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServicePool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& port)
	: _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port))
{
}

void CServer::Start()
{
	auto self = shared_from_this();
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	auto new_con = std::make_shared<HttpConnection>(io_context);
	_acceptor.async_accept(new_con->GetSocket(), [self, new_con](beast::error_code ec) {
		try{
			// ��������������ӣ�����������������
			if (ec) {
				self->Start();
				return;
			}

			// ���������ӣ����Ҵ���HttpConnection������������
			new_con->Start();	// HttpConnection����

			// ��������
			self->Start();
		}
		catch (std::exception& exp) {
			
		}
		});
}