#pragma once
#include "Client.h"
#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <mutex>
#include <map>

namespace websocket
{
	class TCPConnection;
	class IPacket;
	class OPacket;
	class OPacketMult;

	static const int MAX_CLIENTS = 255;

	using namespace boost::asio::ip;

	class ClientManager
	{
	public:
		ClientManager(boost::asio::io_service* ioService, PacketManager* pm);

		virtual Client* addClient(boost::shared_ptr <TCPConnection> tcpConnection);

		virtual Client* getClient(IDType id);

		virtual bool removeClient(IDType id);

		std::map <IDType, Client*> getIDClientMap() const
		{
			return clients;
		}

		virtual PacketManager* getPacketManager()
		{
			return pm;
		}

		virtual void send(boost::shared_ptr<OPacket> oPack);

		virtual void send(boost::shared_ptr<OPacket> oPack, IDType sendTo);

		virtual void send(boost::shared_ptr<OPacket> oPack, Client* sendToClient);

		virtual void sendToAll(boost::shared_ptr<OPacket> oPack);

		virtual void sendToAllExceptSender(boost::shared_ptr<OPacket> oPack);

		virtual void sendToAllExcept(boost::shared_ptr<OPacket> oPack, IDType excludeID);

		virtual void sendToAllExcept(boost::shared_ptr<OPacket> oPack, IDType* excludeIDs, unsigned int size);

		void setErrorMode(int mode)
		{
			errorMode = mode;
		}

		int getErrorMode() const
		{
			return errorMode;
		}

		virtual ~ClientManager();

	protected:

		std::map <IDType, Client*> clients;
		std::mutex clientMapMutex;
		IDType aquireNextID();
		PacketManager* pm;
		boost::asio::io_service* ioService;
		int errorMode;
	};
}