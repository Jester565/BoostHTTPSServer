#include "Server.h"
#include "PacketManager.h"
#include "ClientManager.h"
#include "TCPAcceptor.h"
#include "SetupManager.h"

namespace websocket
{

	Server::Server()
	{
	}

	void Server::createManagers()
	{
		ioService = new boost::asio::io_service();

		pm = new PacketManager();

		cm = new ClientManager(ioService, pm);

		sm = new SetupManager(cm);

		pm->setClientManager(cm);
	}

	void Server::asyncInitServer(uint16_t port)
	{
		ioRunner = new std::thread(&Server::ioRunnerMethod, this);
		ioRunner->detach();
		tcpAcceptor = new TCPAcceptor(ioService, pm, this);
		tcpAcceptor->detach(port);
	}

	void Server::syncInitServer(uint16_t port)
	{
		tcpAcceptor = new TCPAcceptor(ioService, pm, this);
		tcpAcceptor->detach(port);
		ioService->run();
	}

	void Server::syncInitServerNoRun(uint16_t port)
	{
		tcpAcceptor = new TCPAcceptor(ioService, pm, this);
		tcpAcceptor->detach(port);
	}

	void Server::addClient(boost::shared_ptr <TCPConnection> tcpConnection)
	{
		Client* client = cm->addClient(tcpConnection);
	}

	void Server::ioRunnerMethod()
	{
		ioService->run();
	}

	Server::~Server()
	{
		if (ioRunner != nullptr)
		{
			delete ioRunner;
			ioRunner = nullptr;
		}
		if (pm != nullptr)
		{
			delete pm;
			pm = nullptr;
		}
		if (cm != nullptr)
		{
			delete cm;
			cm = nullptr;
		}
		if (tcpAcceptor != nullptr)
		{
			delete tcpAcceptor;
			tcpAcceptor = nullptr;
		}
	}

}