#include "HeaderManager.h"
#include "IPacket.h"
#include "OPacket.h"
#include "dataframe.h"
#include "Packets/PackFW.pb.h"
#include <iostream>
#include <vector>

namespace websocket
{
	HeaderManager::HeaderManager()
	{
		union {
			uint32_t i;
			char c[4];
		} tbend = { 0x01020304 };
		bEndian = (tbend.c[0] == 1);
	}

	void HeaderManager::encryptHeader(const OPacket* oPack)
	{
		if (bEndian)
		{
			return encryptHeaderAsBigEndian(oPack);
		}
		return encryptHeaderToBigEndian(oPack);
	}

	void HeaderManager::encryptHeaderAsBigEndian(const OPacket* oPack)
	{
		if (oPack->data == nullptr)
			throw std::invalid_argument("The packet did not provide data");
		ProtobufPackets::PackHeaderOut headerPackOut;
		headerPackOut.set_lockey(oPack->getLocKey());
		headerPackOut.set_sentfromid(oPack->getSenderID());
		std::string headerPackStr = headerPackOut.SerializeAsString();
		char header[HEADER_OUT_SIZE];
		header[0] = headerPackStr.size() & 0xff;
		header[1] = (headerPackStr.size() >> 8) & 0xff;
		oPack->dataFrame->payload.clear();
		oPack->dataFrame->payload.reserve(HEADER_OUT_SIZE + headerPackStr.size() + oPack->data->size());
		for (int i = 0; i < HEADER_OUT_SIZE; i++)
		{
			oPack->dataFrame->payload.at(i) = header[i];
		}
		for (int i = 0; i < headerPackStr.size(); i++)
		{
			oPack->dataFrame->payload.at(i + HEADER_OUT_SIZE) = headerPackStr.at(i);
		}
		for (int i = 0; i < oPack->data->size(); i++)
		{
			oPack->dataFrame->payload.at(i + HEADER_OUT_SIZE + headerPackStr.size()) = oPack->data->at(i);
		}
	}

	void HeaderManager::encryptHeaderToBigEndian(const OPacket* oPack)
	{
		if (oPack->data == nullptr)
			throw std::invalid_argument("The packet did not provide data");
		ProtobufPackets::PackHeaderOut headerPackOut;
		headerPackOut.set_lockey(oPack->getLocKey());
		headerPackOut.set_sentfromid(oPack->getSenderID());
		std::string headerPackStr = headerPackOut.SerializeAsString();
		char header[HEADER_OUT_SIZE];
		header[1] = headerPackStr.size() & 0xff;
		header[0] = (headerPackStr.size() >> 8) & 0xff;
		oPack->dataFrame->payload.clear();
		oPack->dataFrame->payload.reserve(HEADER_OUT_SIZE + headerPackStr.size() + oPack->data->size());
		for (int i = 0; i < HEADER_OUT_SIZE; i++)
		{
			oPack->dataFrame->payload.push_back(header[i]);
		}
		for (int i = 0; i < headerPackStr.size(); i++)
		{
			oPack->dataFrame->payload.push_back(headerPackStr.at(i));
		}
		for (int i = 0; i < oPack->data->size(); i++)
		{
			oPack->dataFrame->payload.push_back(oPack->data->at(i));
		}
	}

	IPacket* const HeaderManager::decryptHeader(char* header, size_t size, bool& serverRead, dataframe* dataFrame, IDType cID)
	{
		if (bEndian)
		{
			return decryptHeaderAsBigEndian(header, size, serverRead, dataFrame, cID);
		}
		return decryptHeaderFromBigEndian(header, size, serverRead, dataFrame, cID);
	}

	IPacket* const HeaderManager::decryptHeaderAsBigEndian(char* header, size_t size, bool& serverRead, dataframe* dataFrame, IDType cID)
	{
		IPacket* iPack = new IPacket();
		size_t headerPackSize = ((header[1] & 0xff) << 8) | (header[0] & 0xff);
		char* headerPackArr = new char[headerPackSize];
		strncpy(headerPackArr, header + HEADER_IN_SIZE, HEADER_IN_SIZE + headerPackSize);
		ProtobufPackets::PackHeaderIn headerPackIn;
		headerPackIn.ParseFromArray(headerPackArr, headerPackSize);
		iPack->sentFromID = cID;
		iPack->locKey[0] = headerPackIn.lockey()[0];
		iPack->locKey[1] = headerPackIn.lockey()[1];
		iPack->sendToClients = new std::vector <IDType>();
		iPack->sendToClients->reserve(headerPackIn.sendtoids_size());
		for (int i = 0; i < headerPackIn.sendtoids_size(); i++)
		{
			iPack->sendToClients->at(i) = headerPackIn.sendtoids().Get(i);
		}
		serverRead = headerPackIn.serverread();
		size_t mainPackDataSize = size - headerPackSize - HEADER_IN_SIZE;
		std::string* mainPackDataStr = new std::string(header + HEADER_IN_SIZE + headerPackSize, size - (HEADER_IN_SIZE + headerPackSize));
		iPack->dataFrame = dataFrame;
		iPack->data = mainPackDataStr;
		return iPack;
	}

	IPacket* const HeaderManager::decryptHeaderFromBigEndian(char* header, size_t size, bool& serverRead, dataframe* dataFrame, IDType cID)
	{
		IPacket* iPack = new IPacket();
		size_t headerPackSize = ((header[0] & 0xff) << 8) | (header[1] & 0xff);
		std::cout << headerPackSize << std::endl;
		char* headerPackArr = new char[headerPackSize];
		for (int i = 0; i < headerPackSize; i++)
		{
			headerPackArr[i] = header[i + HEADER_IN_SIZE];
		}
		std::cout << headerPackArr << std::endl;
		ProtobufPackets::PackHeaderIn headerPackIn;
		headerPackIn.ParseFromArray(headerPackArr, headerPackSize);
		iPack->sentFromID = cID;
		iPack->locKey[0] = headerPackIn.lockey()[0];
		iPack->locKey[1] = headerPackIn.lockey()[1];
		iPack->sendToClients = new std::vector <IDType>();
		iPack->sendToClients->reserve(headerPackIn.sendtoids_size());
		for (int i = 0; i < headerPackIn.sendtoids_size(); i++)
		{
			iPack->sendToClients->at(i) = headerPackIn.sendtoids().Get(i);
		}
		serverRead = headerPackIn.serverread();
		size_t mainPackDataSize = size - headerPackSize - HEADER_IN_SIZE;
		std::string* mainPackDataStr = new std::string(header + HEADER_IN_SIZE + headerPackSize, size - (HEADER_IN_SIZE + headerPackSize));
		iPack->dataFrame = dataFrame;
		iPack->data = mainPackDataStr;
		return iPack;
	}

	HeaderManager::~HeaderManager()
	{
	}
}
