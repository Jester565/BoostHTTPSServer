#include "handshake_manager.h"
#include "request_parser.h"
#include "request.h"
#include "request_handler.h"
#include "tribool.h"
#include <utility>

namespace websocket
{
	handshake_manager::handshake_manager()
	{
		req_parser = new request_parser();
	}

	reply handshake_manager::parse_handshake(const std::vector <unsigned char>& data, size_t size)
	{
		request req;
		tribool result;
		std::vector <unsigned char>::const_iterator end;
		if (size == USE_VEC_SIZE) {
			end = data.end();
		}
		else {
			end = data.begin() + size;
		}
		std::pair<tribool, std::vector <unsigned char>::const_iterator> rPair = req_parser->parse(req, data.begin(), end);
		result = rPair.first;
		if (result)
		{
			reply rep;
			request_handler::handle_request(req, rep);
			return rep;
		}
		return reply::stock_reply(reply::bad_request);
	}

	handshake_manager::~handshake_manager()
	{
		delete req_parser;
		req_parser = nullptr;
	}
}