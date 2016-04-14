#ifndef WEBSOCKET_HTTP_REQUEST_HANDLER_HPP
#define WEBSOCKET_HTTP_REQUEST_HANDLER_HPP

#include <string>
#include <vector>

namespace websocket
{
	struct reply;
	struct request;

	/// The handler for incoming http requests.
	class request_handler
		//: private boost::noncopyable     TODO: find substitue for noncopyable
	{
	public:
		/// Handle a request and produce a reply.
		static void handle_request(const request& req, reply& rep);
	};
}

#endif // WEBSOCKET_HTTP_REQUEST_HANDLER_HPP