#ifndef HANDSHAKE_MANAGER_HPP
#define HANDSHAKE_MANAGER_HPP

#include "reply.h"
#include <vector>

namespace websocket
{
	class request_parser;
	class request_handler;

	class handshake_manager
	{
	public:
		const static size_t USE_VEC_SIZE = 0;
		handshake_manager();

		reply parse_handshake(const std::vector <unsigned char>& data, size_t size = USE_VEC_SIZE);

		~handshake_manager();

	protected:
		request_parser* req_parser;
	};
}
#endif

