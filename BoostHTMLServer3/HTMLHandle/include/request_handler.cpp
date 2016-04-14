#include "request_handler.h"
#include <sstream>
#include <iostream>  //temp include
#include "header.h"
#include "reply.h"
#include "request.h"
#include "base64_converter.h"
#include "SHA1.h"

namespace websocket
{
	void request_handler::handle_request(const request& req, reply& rep)
	{
		std::string key;
		for (std::vector<header>::const_iterator i = req.headers.cbegin();
		i != req.headers.cend(); ++i)
		{
			if (i->name == "Sec-WebSocket-Key")
			{
				key = i->value;
				break;
			}
		}

		if (key.empty())
		{
			rep = reply::stock_reply(reply::bad_request);
			return;
		}

		const std::string magic_guid("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
		std::string buffer(key + magic_guid);
		std::string accept(Base64::to_base64(to_sha1(buffer)));
		rep.status = reply::switching_protocols;
		rep.headers.resize(3);
		rep.headers[0].name = "Upgrade";
		rep.headers[0].value = "websocket";
		rep.headers[1].name = "Connection";
		rep.headers[1].value = "Upgrade";
		rep.headers[2].name = "Sec-WebSocket-Accept";
		rep.headers[2].value = accept;
	}
}
