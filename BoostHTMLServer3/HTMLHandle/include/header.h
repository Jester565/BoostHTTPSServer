#ifndef WEBSOCKET_HTTP_HEADER_HPP
#define WEBSOCKET_HTTP_HEADER_HPP

#include <string>

namespace websocket
{
	struct header
	{
		std::string name;
		std::string value;
	};
}
#endif // WEBSOCKET_HTTP_HEADER_HPP