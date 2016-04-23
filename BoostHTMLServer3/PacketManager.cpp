#include "PacketManager.h"
#include "ClientManager.h"
#include "IPacket.h"
#include "PKey.h"
#include <iostream>
#include <list>

namespace websocket
{
	PacketManager::PacketManager()
	{

	}

	void PacketManager::addPKey(PKey* pKey)
	{
		bool found = false;
		pKeysMutex.lock();
		int index = binarySearchKey(found, pKey->getKey());
		if (!found)
			pKeys.insert(pKeys.begin() + index, std::list<PKey*>());
 		pKeys.at(index).push_back(pKey);
		pKeysMutex.unlock();
	}

	void PacketManager::addPKeys(std::vector <PKey*> aPKeys)
	{
		for (int i = 0; i < aPKeys.size(); i++)
			addPKey(aPKeys[i]);
	}

	bool PacketManager::removePKey(PKey* pKey)
	{
		bool found = false;
		pKeysMutex.lock();
		int index = binarySearchKey(found, pKey->getKey());
		if (found)
		{
			for (std::list <PKey*>::iterator it = pKeys.at(index).begin(); it != pKeys.at(index).end(); it++)
			{
				if (pKey == *it)
				{
					pKeys.at(index).erase(it);
					if (pKeys.size() == 0)
					{
						pKeys.erase(pKeys.begin() + index);
					}
					pKeysMutex.unlock();
					return true;
				}
			}
		}
		pKeysMutex.unlock();
		return false;
	}

	int PacketManager::removePKeys(std::vector <PKey*> rPKeys)
	{
		int foundCount = 0;
		for (int i = 0; i < rPKeys.size(); i++)
		{
			foundCount += removePKey(rPKeys[i]);
		}
		return rPKeys.size() - foundCount;
	}

	void PacketManager::asyncProcess(boost::shared_ptr<IPacket> iPack, bool serverRead)
	{
		if (serverRead)
		{
			serverProcess(iPack);
		}
	}

	void PacketManager::process()
	{
		while (true)
		{
			syncProcessMutex.lock();
			if (syncProcess.empty())
			{
				syncProcessMutex.unlock();
				break;
			}
			IPacket* iPack = syncProcess.front();
			syncProcess.pop();
			syncProcessMutex.unlock();
			bool found = false;
			pKeysMutex.lock();
			int keyI = binarySearchKey(found, iPack->getLocKey());
			if (!found)
			{
				std::cerr << "Could not find locKey " << iPack->getLocKey() << std::endl;
				if (THROW_KEY_NOT_FOUND_EXCEPTION)
					throw std::invalid_argument("Could not find the locKey!");
			}
			else
			{
				for (std::list <PKey*>::const_iterator it = pKeys.at(keyI).begin(); it != pKeys.at(keyI).end(); it++)
					(*it)->run(iPack);
			}
			pKeysMutex.unlock();
		}
	}

	int PacketManager::binarySearchKey(bool& found, std::string key, int f, int l)
	{
		if (l == USE_PKEYS_SIZE)
		{
			l = pKeys.size() - 1;
		}
		if (l < f)
		{
			found = false;
			return f;
		}
		std::string midKey = pKeys[(l + f) / 2].front()->getKey();
		if (midKey < key)
			return binarySearchKey(found, key, (l + f) / 2 + 1, l);
		else if (midKey > key)
			return binarySearchKey(found, key, f, (l + f) / 2 - 1);
		found = true;
		return (l + f) / 2;
	}

	void PacketManager::serverProcess(boost::shared_ptr<IPacket> iPack)
	{
		pKeysMutex.lock();
		bool found = false;
		int keyI = binarySearchKey(found, iPack->getLocKey());
		if (!found)
		{
			std::cerr << "Could not find locKey " << iPack->getLocKey() << std::endl;
			if (THROW_KEY_NOT_FOUND_EXCEPTION)
				throw std::invalid_argument("Could not find the locKey!");
		}
		else
		{
			for (std::list <PKey*>::const_iterator it = pKeys.at(keyI).begin(); it != pKeys.at(keyI).end(); it++)
				(*it)->run(iPack);
		}
		pKeysMutex.unlock();
	}

	PacketManager::~PacketManager()
	{
		while (!syncProcess.empty())
		{
			delete syncProcess.front();
			syncProcess.pop();
		}
		while (!pKeys.empty())
		{
			while (!pKeys.at(pKeys.size() - 1).empty())
			{
				delete pKeys.at(pKeys.size() - 1).front();
				pKeys.at(pKeys.size() - 1).pop_front();
			}
			pKeys.pop_back();
		}
	}
}
