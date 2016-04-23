#pragma once
#include "PKeyOwner.h"
#include "Packets/Setup.pb.h"

namespace websocket
{
	class IPacket;
	class OPacket;
	class ClientManager;

	class SetupManager : public PKeyOwner
	{
	public:
		SetupManager(ClientManager* cm);

		void keyA0(boost::shared_ptr<IPacket> iPack);

		boost::shared_ptr<OPacket> createPackA1(boost::shared_ptr<IPacket> iPack, const ProtobufPackets::PackA0& packA0);

		~SetupManager();

	protected:
		ClientManager* cm;
	};
}
