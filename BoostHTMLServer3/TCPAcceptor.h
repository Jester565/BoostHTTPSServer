#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

//this is a comment to test git
namespace websocket
{
	class ClientManager;
	class PacketManager;
	class Server;

	typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

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
		ssl_socket* tempSSLSocket;
		boost::asio::ssl::context sslContext;
		PacketManager* pm;
		Server* server;
		int errorMode;
	};
}
