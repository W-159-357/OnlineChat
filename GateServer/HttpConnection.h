#pragma once
#include "const.h"

class HttpConnection:public std::enable_shared_from_this<HttpConnection>
{
public:
	friend class LogicSystem;
	HttpConnection(boost::asio::io_context& ioc);
	void Start();
	void PreParseGetParam();	// ��������
	tcp::socket& GetSocket() {
		return _socket;
	};

private:
	void CheckDeadline();		// ��ʱ���
	void WriteResponse();		// дӦ��
	void HandleReq();			// ��������
	tcp::socket _socket;
	beast::flat_buffer _buffer{8192};
	http::request<http::dynamic_body> _request;
	http::response<http::dynamic_body> _response;
	net::steady_timer _deadline{					// ��ʱ��
		_socket.get_executor(), std::chrono::seconds(60)
	};

	std::string _get_url;
	std::unordered_map<std::string, std::string> _get_params;
};

