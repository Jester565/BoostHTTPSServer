#include "PKeyOwner.h"
#include "PacketManager.h"

namespace websocket
{
	PKeyOwner::PKeyOwner(PacketManager* pm)
		:pm(pm)
	{

	}

	void PKeyOwner::addKey(PKey* pKey)
	{
		pm->addPKey(pKey);
		pKeys.push_back(pKey);
	}

	void PKeyOwner::removeKey(PKey* pKey)
	{
		pm->removePKey(pKey);
		pKeys.remove(pKey);
	}

	void PKeyOwner::deleteKey(PKey* pKey)
	{
		removeKey(pKey);
		delete pKey;
	}

	void PKeyOwner::removeKeys()
	{
		while (!pKeys.empty())
			removeKey(pKeys.front());
	}

	void PKeyOwner::deleteKeys()
	{
		while (!pKeys.empty())
			deleteKey(pKeys.front());
	}

	PKeyOwner::~PKeyOwner()
	{
		deleteKeys();
	}
}
