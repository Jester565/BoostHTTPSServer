#include "TCPAcceptor.h"
#include "TCPConnection.h"
#include "Server.h"
#include "PacketMacros.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

namespace websocket
{
	using namespace boost::asio::ip;

	TCPAcceptor::TCPAcceptor(boost::asio::io_service* ioService, PacketManager* pm, Server* server)
		:ioService(ioService), pm(pm), server(server), errorMode(DEFAULT_ERROR_MODE), sslContext(*ioService, boost::asio::ssl::context::sslv23)
	{
		sslContext.set_options(
			boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::single_dh_use);
		sslContext.set_password_callback(boost::bind(&TCPAcceptor::getCertPwd, this));
		sslContext.use_certificate_chain_file("server.crt");
		sslContext.use_private_key_file("server.key", boost::asio::ssl::context::pem);
	}

	void TCPAcceptor::detach(uint16_t tcpPort)
	{
		acceptor = new tcp::acceptor(*ioService, tcp::endpoint(tcp::v6(), tcpPort));
		tempTCPSocket = new tcp::socket(*ioService);
		acceptor->async_accept(*tempTCPSocket, boost::bind(&TCPAcceptor::asyncAccept, this, boost::asio::placeholders::error));
	}

	void TCPAcceptor::asyncAccept(const boost::system::error_code& error)
	{
		if (error)
		{
			std::cerr << "Error occured in TCPAcceptor: " << error.message() << std::endl;
			switch (errorMode)
			{
			case THROW_ON_ERROR:
				throw error;
				break;
			case RETURN_ON_ERROR:
				return;
				break;
			case RECALL_ON_ERROR:
				if (tempTCPSocket != nullptr)
				{
					delete tempTCPSocket;
					tempTCPSocket = nullptr;
				}
				tempTCPSocket = new tcp::socket(*ioService);
				acceptor->async_accept(*tempTCPSocket, boost::bind(&TCPAcceptor::asyncAccept, this, boost::asio::placeholders::error));
				return;
			};
		}
		tempTCPSocket->non_blocking(true);
		tempTCPSocket->native_non_blocking(true);
		boost::shared_ptr <TCPConnection> tcpConnection(new TCPConnection(pm, tempTCPSocket));
		tcpConnection->startRead();
		server->addClient(tcpConnection);
		tempTCPSocket = new tcp::socket(*ioService);
		acceptor->async_accept(*tempTCPSocket, boost::bind(&TCPAcceptor::asyncAccept, this, boost::asio::placeholders::error));
	}

	TCPAcceptor::~TCPAcceptor()
	{
		acceptor->cancel();
		acceptor->close();
		if (acceptor != nullptr)
		{
			delete acceptor;
			acceptor = nullptr;
		}
	}
}