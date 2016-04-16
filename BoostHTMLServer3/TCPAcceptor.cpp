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
		sslContext.set_options(boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::single_dh_use);
		sslContext.set_password_callback(boost::bind(&TCPAcceptor::getCertPwd, this));
		sslContext.use_certificate_chain_file("local.crt");
		sslContext.use_private_key_file("local.key", boost::asio::ssl::context::pem);
		sslContext.use_tmp_dh_file("dh512.pem");
	}

	void TCPAcceptor::detach(uint16_t tcpPort)
	{
		acceptor = new tcp::acceptor(*ioService, tcp::endpoint(tcp::v6(), tcpPort));
		tempSSLSocket = new ssl_socket(*ioService, sslContext);
		acceptor->async_accept(tempSSLSocket->lowest_layer(), boost::bind(&TCPAcceptor::asyncAccept, this, boost::asio::placeholders::error));
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
				if (tempSSLSocket != nullptr)
				{
					delete tempSSLSocket;
					tempSSLSocket = nullptr;
				}
				tempSSLSocket = new ssl_socket(*ioService, sslContext);
				acceptor->async_accept(tempSSLSocket->lowest_layer(), boost::bind(&TCPAcceptor::asyncAccept, this, boost::asio::placeholders::error));
				return;
			};
		}
		boost::shared_ptr <TCPConnection> tcpConnection(new TCPConnection(pm, tempSSLSocket));
		tcpConnection->startSSLHandhshake();
		server->addClient(tcpConnection);
		tempSSLSocket = new ssl_socket(*ioService, sslContext);
		acceptor->async_accept(tempSSLSocket->lowest_layer(), boost::bind(&TCPAcceptor::asyncAccept, this, boost::asio::placeholders::error));
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
