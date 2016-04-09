#pragma once
#include "PKey.h"
#include <list>

namespace websocket
{
	class PacketManager;

	class PKeyOwner
	{
	public:
		PKeyOwner(PacketManager* pm);

		void setPacketManager(PacketManager* pm)
		{
			this->pm = pm;
		}

		virtual void addKey(PKey* pKey);

		virtual void removeKey(PKey* pKey);

		virtual void deleteKey(PKey* pKey);

		virtual void removeKeys();

		virtual void deleteKeys();

		virtual ~PKeyOwner();

	protected:
		PacketManager* pm;
		std::list <PKey*> pKeys;
	};
}