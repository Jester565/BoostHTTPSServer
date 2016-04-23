#include "IPacket.h"
#include "OPacket.h"
#include <boost/make_shared.hpp>

namespace websocket
{
	IPacket::IPacket()
		: sendToClients(nullptr)
	{
		locKey[0] = UNDEFINED_LOC;
		locKey[1] = UNDEFINED_LOC;
		locKey[2] = '\0';
	}

	std::vector<boost::shared_ptr<OPacket>>* IPacket::convertToOPacks(bool copyData)
	{
		if (sendToClients != nullptr)
		{
			std::vector <boost::shared_ptr<OPacket>>* oPacks = new std::vector<boost::shared_ptr<OPacket>>();
			for (int i = 0; i < sendToClients->size(); i++)
			{
				boost::shared_ptr<OPacket> oPack = boost::make_shared<OPacket>(this, copyData);
				oPack->setSenderID(sendToClients->at(i));
				oPacks->push_back(oPack);
			}
			return oPacks;
		}
		return nullptr;
	}

	IPacket::~IPacket()
	{
		if (sendToClients != nullptr)
		{
			delete sendToClients;
			sendToClients = nullptr;
		}
	}
}
