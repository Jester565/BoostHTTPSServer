#pragma once
#include <stdint.h>

namespace websocket
{
	typedef uint32_t IDType;
	static const size_t ID_TYPE_SIZE_BITS = 32;
	static const size_t ID_TYPE_SIZE_BYTES = ID_TYPE_SIZE_BITS / 8;
	static const size_t MAX_DATA_SIZE = 1024;
	static const char UNDEFINED_LOC = 'Z';

	static const int THROW_ON_ERROR = 0;
	static const int RETURN_ON_ERROR = 1;
	static const int RECALL_ON_ERROR = 2;
	static const int CONTINUE_ON_ERROR = 3;
	static const int DEFAULT_ERROR_MODE = 1;
}
