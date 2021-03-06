#pragma once
#include "PacketMacros.h"
#include <vector>
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>

namespace websocket
{
	class dataframe;
	class OPacket;

	class IPacket
	{
		friend class HeaderManager;
	public:
		IPacket();

		IDType getSentFromID()
		{
			return sentFromID;
		}

		const char* getLocKey() const
		{
			return locKey;
		}

		const boost::shared_ptr<std::string> getData() const
		{
			return data;
		}

		std::vector <IDType>* getSendToClients()
		{
			return sendToClients;
		}

		boost::shared_ptr<dataframe> getDataFrame()
		{
			return dataFrame;
		}

		friend std::ostream& operator << (std::ostream& oStream, IPacket& iPack)
		{
			oStream << "Printing out packet: " << std::endl << "loc key: " << iPack.locKey << std::endl;
			oStream << "Sent from: " << iPack.sentFromID << std::endl;
			if (iPack.sendToClients == nullptr)
			{
				oStream << "Send to clients: size = 0" << std::endl;
			}
			else
			{
				oStream << "Send to clients: size = " << iPack.sendToClients->size() << "  Items: ";
				for (int i = 0; i < iPack.sendToClients->size(); i++)
				{
					oStream << iPack.sendToClients->at(i) << " ";
				}
				oStream << std::endl;
			}
			//oStream << "Data {" << *iPack.data << "}" << std::endl;
			return oStream;
		}

		std::vector <boost::shared_ptr<OPacket>>* convertToOPacks(bool copyData = true);

		virtual ~IPacket();

	protected:
		IDType sentFromID;
		char locKey[3];
		boost::shared_ptr<dataframe> dataFrame;
		boost::shared_ptr<std::string> data;
		std::vector <IDType>* sendToClients;
	};
}
