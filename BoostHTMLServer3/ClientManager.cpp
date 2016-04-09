#include "ClientManager.h"
#include "PacketMacros.h"
#include "IPacket.h"
#include "OPacket.h"
#include "TCPConnection.h"
#include <boost/bind.hpp>

namespace websocket
{
	using namespace boost::asio::ip;

	ClientManager::ClientManager(boost::asio::io_service* ioService, PacketManager* pm)
		:ioService(ioService), pm(pm), errorMode(DEFAULT_ERROR_MODE)
	{

	}

	Client* ClientManager::addClient(boost::shared_ptr <TCPConnection> tcpConnection)
	{
		Client* client = nullptr;
		clientMapMutex.lock();
		IDType id = aquireNextID();
		if (id < MAX_CLIENTS)
		{
			client = new Client(id, tcpConnection, pm, ioService);
			clients.emplace(id, client);
		}
		else
		{
			std::cerr << "Maximum amount of clients has been exceeded... unable to add" << std::endl;
		}
		clientMapMutex.unlock();
		return client;
	}

	Client* ClientManager::getClient(IDType id)
	{
		clientMapMutex.lock();
		std::map <IDType, Client*>::iterator mapFind = clients.find(id);
		if (mapFind != clients.end())
		{
			clientMapMutex.unlock();
			return mapFind->second;
		}
		clientMapMutex.unlock();
		return nullptr;
	}

	bool ClientManager::removeClient(IDType id)
	{
		clientMapMutex.lock();
		std::map <IDType, Client*>::iterator idFind = clients.find(id);
		if (idFind == clients.end())
		{
			clientMapMutex.unlock();
			std::cerr << "Could not find the client to remove" << std::endl;
			return false;
		}
		delete idFind->second;
		clients.erase(idFind);
		clientMapMutex.unlock();
		return true;
	}

	IDType ClientManager::aquireNextID()
	{
		int trackNum = 1;
		for (std::map <IDType, Client*>::iterator mapIter = clients.begin(); mapIter != clients.end(); mapIter++)
		{
			if (trackNum != mapIter->first)
			{
				break;
			}
			trackNum++;
		}
		return trackNum;
	}

	void ClientManager::send(OPacket* oPack)
	{
		for (int i = 0; i < oPack->getSendToIDs().size(); i++)
		{
			Client* client = getClient(oPack->getSendToIDs().at(i));
			if (client != nullptr)
			{
				send(oPack, client);
			}
			else
			{
				std::cerr << "Error occured in ClientManager:send, could not find the client id: " << oPack->getSendToIDs().at(i) << std::endl;
			}
		}
	}

	void ClientManager::send(OPacket* oPack, IDType sendToID)
	{
		Client* client = getClient(sendToID);
		if (client == nullptr)
		{
			std::cerr << "Error occured in ClientManager:send, could not find client nubmer" << sendToID << std::endl;
			switch (errorMode)
			{
			case THROW_ON_ERROR:
				throw "Error in ClientManager:send";
				break;
			case RETURN_ON_ERROR:
				return;
				break;
			case RECALL_ON_ERROR:
				send(oPack);
				return;
			};
		}
		client->getTCPConnection()->send(oPack);
	}

	void ClientManager::send(OPacket* oPack, Client* client)
	{
		if (client == nullptr)
		{
			std::cerr << "Error in ClientManager::send, the client is nullptr" << std::endl;
			switch (errorMode)
			{
			case THROW_ON_ERROR:
				throw "Error in ClientManager:send";
				break;
			case RETURN_ON_ERROR:
				return;
				break;
			case RECALL_ON_ERROR:
				send(oPack);
				return;
			};
		}
		client->getTCPConnection()->send(oPack);
	}

	void ClientManager::sendToAll(OPacket* oPack)
	{
		for (auto iter = clients.begin(); iter != clients.end(); iter++)
		{
			oPack->setSenderID(iter->first);
			send(oPack, iter->second);
		}
	}

	void ClientManager::sendToAllExceptSender(OPacket* oPack)
	{
		sendToAllExcept(oPack, oPack->getSenderID());
	}

	void ClientManager::sendToAllExcept(OPacket* oPack, IDType excludeID)
	{
		for (auto iter = clients.begin(); iter != clients.end(); iter++)
		{
			if (iter->first != excludeID)
			{
				oPack->setSenderID(iter->first);
				send(oPack, iter->second);
			}
		}
	}

	void ClientManager::sendToAllExcept(OPacket* oPack, IDType* excludeIDs, unsigned int size)
	{
		for (auto iter = clients.begin(); iter != clients.end(); iter++)
		{
			for (int i = 0; i < size; i++)
			{
				if (iter->first == excludeIDs[i])
				{
					continue;
				}
			}
			oPack->setSenderID(iter->first);
			send(oPack);
		}
	}

	ClientManager::~ClientManager()
	{
		for (std::map <IDType, Client*>::iterator mapIter = clients.begin(); mapIter != clients.end(); mapIter++)
		{
			delete mapIter->second;
			mapIter->second = nullptr;
		}
		clients.clear();
	}
}
