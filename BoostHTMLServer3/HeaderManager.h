#pragma once
#include "PacketMacros.h"
#include <stdint.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace websocket
{
	class IPacket;

	class OPacket;

	class dataframe;

	class HeaderManager
	{
	public:
		static const unsigned int HEADER_OUT_SIZE = 2;
		static const unsigned int HEADER_IN_SIZE = 2;

		HeaderManager();

		virtual void encryptHeader(boost::shared_ptr<OPacket> pack);

		virtual boost::shared_ptr<IPacket> const decryptHeader(char* header, unsigned int size, bool& serverRead, boost::shared_ptr<dataframe> dataFrame, IDType cID);

		virtual ~HeaderManager();

	protected:
		virtual void encryptHeaderAsBigEndian(boost::shared_ptr<OPacket> pack);

		virtual void encryptHeaderToBigEndian(boost::shared_ptr<OPacket> pack);

		virtual boost::shared_ptr<IPacket> const decryptHeaderAsBigEndian(char* header, unsigned int size, bool& serverRead, boost::shared_ptr<dataframe> dataFrame, IDType cID);

		virtual boost::shared_ptr<IPacket> const decryptHeaderFromBigEndian(char* header, unsigned int size, bool& serverRead, boost::shared_ptr<dataframe> dataFrame, IDType cID);

		bool bEndian;
	};
}
