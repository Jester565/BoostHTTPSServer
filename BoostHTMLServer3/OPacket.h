#pragma once
#include "PacketMacros.h"
#include <string>
#include <vector>

namespace websocket
{
	class IPacket;
	class dataframe;

	class OPacket
	{
		friend class HeaderManager;
	public:
		OPacket(char* loc = nullptr, IDType senderID = 0);

		OPacket(char* loc, IDType senderID, IDType sendToID);

		OPacket(char* loc, IDType senderID, IDType* sendToIDs, size_t sendToIDsSize);

		OPacket(char* loc, IDType senderID, std::vector <IDType> sendToIDs);

		OPacket(IPacket* iPack, bool copyData = true);
		/**
		Summary: Sets the location key of the packet, which will determine how it is processed by the client
		Params:
		- (char*) A 2 character long array with the locKey you want to set it to. If nullptr, locKey will be set to UNDEFINED_LOC (PacketMacros).
		**/
		void setLocKey(char* loc)
		{
			if (loc != nullptr)
			{
				locKey[0] = loc[0];
				locKey[1] = loc[1];
			}
			else
			{
				locKey[0] = loc[UNDEFINED_LOC];
				locKey[1] = loc[UNDEFINED_LOC];
			}
			locKey[2] = '\0';
		}

		/**
		Summary: set the sendID which is used to tell the client who originally sent this packet
		Params:
		- (IDType) IDType is by defualt a uint16_t, but IDType is defined in PacketMarcros.h
		**/
		void setSenderID(IDType id)
		{
			senderID = id;
		}

		void addSendToID(IDType id)
		{
			sendToIDs.push_back(id);
		}

		bool removeSendToID(IDType id)
		{
			for (int i = 0; i < sendToIDs.size(); i++)
			{
				if (sendToIDs.at(i) == id)
				{
					sendToIDs.erase(sendToIDs.begin() + i);
					return true;
				}
			}
			return false;
		}

		void clearSendToIDs(IDType id)
		{
			sendToIDs.clear();
		}

		/**
		Summary: Sets a string of data for sending (should be a protobuf packet)
		Params:
		- (std::string*)  A pointer to the string to be sent (the string can be on the stack, we will call new for you)
		Return:
		- (bool)  True if data was not reset, false if it was reset
		**/
		bool setData(std::string* data)
		{
			bool reset = !(data == nullptr);
			this->data = new std::string(*data);
			return reset;
		}

		const char const* getLocKey() const
		{
			return locKey;
		}

		const IDType getSenderID() const
		{
			return senderID;
		}

		virtual std::vector <IDType> getSendToIDs() const
		{
			return sendToIDs;
		}

		dataframe* getDataFrame()
		{
			return dataFrame;
		}

		friend std::ostream& operator << (std::ostream& oStream, OPacket& oPack)
		{
			oStream << "Printing out packet: " << std::endl << "loc key: " << oPack.locKey << std::endl;
			oStream << "Sender ID: " << oPack.senderID << std::endl;
			oStream << "Send to IDs: ";
			for (int i = 0; i < oPack.sendToIDs.size(); i++)
			{
				oStream << oPack.sendToIDs.at(i);
				if (i != oPack.sendToIDs.size() - 1)
				{
					oStream << ", ";
				}
			}
			oStream << std::endl;
			oStream << "Data {" << *oPack.data << "}" << std::endl;
			return oStream;
		}

		virtual ~OPacket();

	protected:
		char locKey[3];
		IDType senderID;
		std::vector <IDType> sendToIDs;
		dataframe* dataFrame;
		std::string* data;
	};
}
