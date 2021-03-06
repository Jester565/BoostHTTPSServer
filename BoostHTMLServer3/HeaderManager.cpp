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

	void HeaderManager::encryptHeader(boost::shared_ptr<OPacket> oPack)
	{
		if (bEndian)
		{
			return encryptHeaderAsBigEndian(oPack);
		}
		return encryptHeaderToBigEndian(oPack);
	}

	void HeaderManager::encryptHeaderAsBigEndian(boost::shared_ptr<OPacket> oPack)
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

	void HeaderManager::encryptHeaderToBigEndian(boost::shared_ptr<OPacket> oPack)
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

	boost::shared_ptr<IPacket> const HeaderManager::decryptHeader(char* header, unsigned int size, bool& serverRead, boost::shared_ptr<dataframe> dataFrame, IDType cID)
	{
		if (bEndian)
		{
			return decryptHeaderAsBigEndian(header, size, serverRead, dataFrame, cID);
		}
		return decryptHeaderFromBigEndian(header, size, serverRead, dataFrame, cID);
	}

	boost::shared_ptr<IPacket> const HeaderManager::decryptHeaderAsBigEndian(char* header, unsigned int size, bool& serverRead, boost::shared_ptr<dataframe> dataFrame, IDType cID)
	{
		boost::shared_ptr<IPacket> iPack = boost::make_shared<IPacket>();
		unsigned int headerPackSize = ((header[1] & 0xff) << 8) | (header[0] & 0xff);
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
		unsigned int mainPackDataSize = size - headerPackSize - HEADER_IN_SIZE;
		boost::shared_ptr<std::string> mainPackDataStr = boost::make_shared<std::string>(header + HEADER_IN_SIZE + headerPackSize, size - (HEADER_IN_SIZE + headerPackSize));
		iPack->dataFrame = dataFrame;
		iPack->data = mainPackDataStr;
		delete[] headerPackArr;
		return iPack;
	}

	boost::shared_ptr<IPacket> const HeaderManager::decryptHeaderFromBigEndian(char* header, unsigned int size, bool& serverRead, boost::shared_ptr<dataframe> dataFrame, IDType cID)
	{
		boost::shared_ptr<IPacket> iPack = boost::make_shared<IPacket>();
		unsigned int headerPackSize = ((header[0] & 0xff) << 8) | (header[1] & 0xff);
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
		unsigned int mainPackDataSize = size - headerPackSize - HEADER_IN_SIZE;
		boost::shared_ptr<std::string> mainPackDataStr = boost::make_shared<std::string>(header + HEADER_IN_SIZE + headerPackSize, size - (HEADER_IN_SIZE + headerPackSize));
		iPack->dataFrame = dataFrame;
		iPack->data = mainPackDataStr;
        delete[] headerPackArr;
		return iPack;
	}

	HeaderManager::~HeaderManager()
	{
	}
}
