#include "IPacket.h"
#include "OPacket.h"

namespace websocket
{
	IPacket::IPacket()
		: sendToClients(nullptr), data(nullptr)
	{
		locKey[0] = UNDEFINED_LOC;
		locKey[1] = UNDEFINED_LOC;
		locKey[2] = '\0';
	}

	std::vector <OPacket*>* IPacket::convertToOPacks(bool copyData)
	{
		if (sendToClients != nullptr)
		{
			std::vector <OPacket*>* oPacks = new std::vector<OPacket*>();
			for (int i = 0; i < sendToClients->size(); i++)
			{
				OPacket* oPack = new OPacket(this, copyData);
				oPack->setSenderID(sendToClients->at(i));
				oPacks->push_back(oPack);
			}
			return oPacks;
		}
		return nullptr;
	}

	IPacket::~IPacket()
	{
		if (data != nullptr)
		{
			delete data;
			data = nullptr;
		}
		if (sendToClients != nullptr)
		{
			delete sendToClients;
			sendToClients = nullptr;
		}
	}
}
