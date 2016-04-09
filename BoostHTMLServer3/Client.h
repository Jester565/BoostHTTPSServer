#pragma once
#include "PacketMacros.h"
#include <mutex>
#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream> //temp include

namespace websocket
{
	class TCPConnection;
	class PacketManager;

	class Client
	{
	public:
		Client(IDType id, boost::shared_ptr <TCPConnection> tcpConnection, PacketManager* pm, boost::asio::io_service* ioService);

		boost::shared_ptr <TCPConnection> getTCPConnection()
		{
			return tcpConnection;
		}

		IDType getID() const
		{
			return id;
		}

		virtual ~Client();

	protected:
		boost::shared_ptr <TCPConnection> tcpConnection;
		IDType id;
	};
}
