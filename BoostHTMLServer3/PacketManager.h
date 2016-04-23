#pragma once
#include <vector>
#include <list>
#include <mutex>
#include <queue>

namespace websocket
{
	class PKey;
	class IPacket;
	class ClientManager;

	class PacketManager
	{
	public:
		static const int USE_PKEYS_SIZE = -1;
		static const bool THROW_KEY_NOT_FOUND_EXCEPTION = false;
		PacketManager();

		void setClientManager(ClientManager* cm)
		{
			this->cm = cm;
		}

		void addPKey(PKey*);

		void addPKeys(std::vector <PKey*>);

		bool removePKey(PKey*);

		int removePKeys(std::vector <PKey*>);

		void asyncProcess(boost::shared_ptr<IPacket> iPack, bool serverRead);

		void process();

		void addToProcessing(IPacket* iPack)
		{
			syncProcessMutex.lock();
			syncProcess.push(iPack);
			syncProcessMutex.unlock();
		}

		ClientManager* getClientManager()
		{
			return cm;
		}

		~PacketManager();

	protected:
		void serverProcess(boost::shared_ptr<IPacket> iPack);

		int binarySearchKey(bool& found, std::string key, int f = 0, int l = USE_PKEYS_SIZE);
		std::vector <std::list <PKey*>> pKeys;
		std::queue <IPacket*> syncProcess;
		std::mutex pKeysMutex;
		std::mutex syncProcessMutex;
		ClientManager* cm;
	};
}
