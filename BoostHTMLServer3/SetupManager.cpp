#include "SetupManager.h"
#include "IPacket.h"
#include "OPacket.h"
#include "TCPConnection.h"
#include "ClientManager.h"
#include "Client.h"
#include <map>
#include <boost/make_shared.hpp>

namespace websocket
{
	SetupManager::SetupManager(ClientManager* cm)
		:PKeyOwner(cm->getPacketManager()), cm(cm)
	{
		addKey(new PKey("A0", this, &SetupManager::keyA0));
	}

	void SetupManager::keyA0(boost::shared_ptr<IPacket> iPack)
	{
		ProtobufPackets::PackA0 packA0;
		packA0.ParseFromString(*iPack->getData());
		boost::shared_ptr<OPacket> oPackA1 = createPackA1(iPack, packA0);
		cm->send(oPackA1);
	}

	boost::shared_ptr<OPacket> SetupManager::createPackA1(boost::shared_ptr<IPacket> iPack, const ProtobufPackets::PackA0& packA0)
	{
		ProtobufPackets::PackA1 packA1;
		packA1.set_id(iPack->getSentFromID());
		boost::shared_ptr<OPacket> oPackA1 = boost::make_shared<OPacket>("A1", 0, iPack->getSentFromID());
		oPackA1->setData(boost::make_shared<std::string>(packA1.SerializeAsString()));
		return oPackA1;
	}

	SetupManager::~SetupManager()
	{

	}
}
