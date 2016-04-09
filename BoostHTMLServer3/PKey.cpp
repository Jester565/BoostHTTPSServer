#include "PKey.h"

namespace websocket
{
	PKey::PKey(const std::string& key)
		:key(key), runMethod(nullptr)
	{
	}

	PKey::~PKey()
	{
	}
}