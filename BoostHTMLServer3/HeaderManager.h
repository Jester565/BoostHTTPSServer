#pragma once
#include "PacketMacros.h"
#include <stdint.h>
#include <string>

namespace websocket
{
	class IPacket;

	class OPacket;

	class dataframe;

	class HeaderManager
	{
	public:
		static const size_t HEADER_OUT_SIZE = 2;
		static const size_t HEADER_IN_SIZE = 2;

		HeaderManager();

		virtual void encryptHeader(const OPacket* pack);

		virtual IPacket* const decryptHeader(char* header, size_t size, bool& serverRead, dataframe* dataFrame, IDType cID);

		virtual ~HeaderManager();

	protected:
		virtual void encryptHeaderAsBigEndian(const OPacket* pack);

		virtual void encryptHeaderToBigEndian(const OPacket* pack);

		virtual IPacket* const decryptHeaderAsBigEndian(char* header, size_t size, bool& serverRead, dataframe* dataFrame, IDType cID);

		virtual IPacket* const decryptHeaderFromBigEndian(char* header, size_t size, bool& serverRead, dataframe* dataFrame, IDType cID);

		bool bEndian;
	};
}