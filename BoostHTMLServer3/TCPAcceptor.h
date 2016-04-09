#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace websocket
{
	class ClientManager;
	class PacketManager;
	class Server;

	class TCPAcceptor
	{
	public:
		TCPAcceptor(boost::asio::io_service* ioService, PacketManager* pm, Server* server);

		virtual void detach(uint16_t tcpPort);

		virtual void asyncAccept(const boost::system::error_code& error);

		virtual ~TCPAcceptor();

	protected:
		boost::asio::io_service* ioService;
		boost::asio::ip::tcp::acceptor* acceptor;
		boost::asio::ip::tcp::socket* tempTCPSocket;
		boost::asio::ssl::context sslContext;
		PacketManager* pm;
		Server* server;
		int errorMode;
		std::string getCertPwd()
		{
			return "***";
		}
	};
}
