#include "OPacket.h"
#include "IPacket.h"
#include "dataframe.h"
#include <boost/make_shared.hpp>
namespace websocket
{
	OPacket::OPacket(char* loc, IDType senderID)
		:senderID(senderID), data(nullptr)
	{
		setLocKey(loc);
		dataFrame = boost::make_shared<dataframe>();
	}

	OPacket::OPacket(char* loc, IDType senderID, IDType sendToID)
		: senderID(senderID), data(nullptr)
	{
		setLocKey(loc);
		dataFrame = boost::make_shared<dataframe>();
		addSendToID(sendToID);
	}

	OPacket::OPacket(char* loc, IDType senderID, IDType* sendToIDs, unsigned int sendToIDsSize)
		: senderID(senderID), data(nullptr)
	{
		setLocKey(loc);
		dataFrame = boost::make_shared<dataframe>();
		for (int i = 0; i < sendToIDsSize; i++)
		{
			addSendToID(sendToIDs[i]);
		}
	}

	OPacket::OPacket(char* loc, IDType senderID, std::vector <IDType> sendToIDs)
		: senderID(senderID), data(nullptr)
	{
		setLocKey(loc);
		dataFrame = boost::make_shared<dataframe>();
		this->sendToIDs = sendToIDs;
	}

	OPacket::OPacket(IPacket* iPack, bool copyData)
	{
		locKey[0] = iPack->getLocKey()[0];
		locKey[1] = iPack->getLocKey()[1];
		locKey[2] = '\0';
		senderID = iPack->getSentFromID();
		if (iPack->getSendToClients() != nullptr)
		{
			sendToIDs = *iPack->getSendToClients();
		}
		dataFrame = iPack->getDataFrame();
		if (copyData)
		{
			data = iPack->getData();
		}
	}

	OPacket::~OPacket()
	{
		
	}
}
