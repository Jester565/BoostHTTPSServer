#include "Server.h"
#include "PacketManager.h"
#include "ClientManager.h"
#include "TCPAcceptor.h"
#include "SetupManager.h"

namespace websocket
{

	Server::Server()
	:ioServiceThread(nullptr), ioService(nullptr), pm(nullptr), cm(nullptr), sm(nullptr), tcpAcceptor(nullptr)
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
		tcpAcceptor = new TCPAcceptor(ioService, pm, this);
		tcpAcceptor->detach(port);
		ioServiceThread = new std::thread(&Server::asyncIOService, this);
		ioServiceThread->detach();
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

	void Server::asyncIOService()
	{
		std::cout << "RUNNING" << std::endl;
		ioService->run();
	}

	Server::~Server()
	{
	  ioService->stop();
		if (tcpAcceptor != nullptr)
		{
			delete tcpAcceptor;
			tcpAcceptor = nullptr;
		}
		if (cm != nullptr)
		{
			delete cm;
			cm = nullptr;
		}
		if (sm != nullptr)
		{
			delete sm;
			sm = nullptr;
		}
		if (pm != nullptr)
		{
			delete pm;
			pm = nullptr;
		}
		ioService->reset();
		if (ioService != nullptr)
		{
			delete ioService;
			ioService = nullptr;
		}
		if (ioServiceThread != nullptr)
		{
		  delete ioServiceThread;
		  ioServiceThread = nullptr;
		}
		google::protobuf::ShutdownProtobufLibrary();
	}

}
