#include "Client.h"
#include "TCPConnection.h"
#include <memory>
#include <boost/shared_ptr.hpp>

namespace websocket
{

	Client::Client(IDType id, boost::shared_ptr <TCPConnection> tcpConnection, PacketManager* pm, boost::asio::io_service* ioService)
		:tcpConnection(tcpConnection), id(id)
	{
		tcpConnection->setID(id);
	}

	Client::~Client()
	{

	}
}
