#ifndef WEBSOCKET_HTTP_REPLY_HPP
#define WEBSOCKET_HTTP_REPLY_HPP

#include <vector>
#include "header.h"

namespace websocket
{
	struct reply
	{
		enum status_type
		{
			switching_protocols = 101,
			bad_request = 400,
			internal_server_error = 500,
		} status;

		std::vector<header> headers;

		std::vector<uint8_t> to_buffer();

		static reply stock_reply(status_type status);
	};
}
#endif;