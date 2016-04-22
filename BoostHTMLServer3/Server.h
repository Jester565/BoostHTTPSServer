#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>
#include <thread>
#include <memory>
namespace websocket
{
	class ClientManager;
	class PacketManager;
	class TCPConnection;
	class TCPAcceptor;
	class SetupManager;
	
	class Server
	{
	public:
		Server();

		virtual void createManagers();

		virtual void asyncInitServer(uint16_t port);

		virtual void syncInitServer(uint16_t port);

		virtual void syncInitServerNoRun(uint16_t port);

		void addClient(boost::shared_ptr <TCPConnection> tcpConnection);

		virtual ClientManager* getClientManager() const
		{
			return cm;
		}

		virtual PacketManager* getPacketManager()
		{
			return pm;
		}

		virtual ~Server();

	private:
		boost::asio::io_service* ioService;
		std::thread* ioServiceThread;
		void asyncIOService();
		PacketManager* pm;
		SetupManager* sm;
		ClientManager* cm;
		TCPAcceptor* tcpAcceptor;
	};


}

