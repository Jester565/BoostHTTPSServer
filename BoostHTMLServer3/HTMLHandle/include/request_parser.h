#ifndef WEBSOCKET_SERVER_REQUEST_PARSER_HPP
#define WEBSOCKET_SERVER_REQUEST_PARSER_HPP

#include <utility>
#include "tribool.h"

namespace websocket
{
	struct request;

	/// Parser for incoming requests.
	class request_parser
	{
	public:
		/// Construct ready to parse the request method.
		request_parser();

		/// Reset to initial parser state.
		void reset();

		/// Parse some data. The tribool return value is true when a complete request
		/// has been parsed, false if the data is invalid, indeterminate when more
		/// data is required. The InputIterator return value indicates how much of the
		/// input has been consumed.

		//changed returned type from boost::tuple to std::pair
		template <typename InputIterator>
		std::pair<tribool, InputIterator> parse(request& req,
			InputIterator begin, InputIterator end)
		{
			while (begin != end)
			{
				tribool result = consume(req, *begin++);
				if (result || !result)
					return std::make_pair(result, begin);
			}
			tribool result = indeterminate;
			return std::make_pair(result, begin);
		}

	private:
		/// Handle the next character of input.
		tribool consume(request& req, char input);

		/// Check if a byte is an HTTP character.
		static bool is_char(int c);

		/// Check if a byte is an HTTP control character.
		static bool is_ctl(int c);

		/// Check if a byte is defined as an HTTP tspecial character.
		static bool is_tspecial(int c);

		/// Check if a byte is a digit.
		static bool is_digit(int c);

		/// The current state of the parser.
		enum state
		{
			method_start,
			method,
			uri,
			http_version_h,
			http_version_t_1,
			http_version_t_2,
			http_version_p,
			http_version_slash,
			http_version_major_start,
			http_version_major,
			http_version_minor_start,
			http_version_minor,
			expecting_newline_1,
			header_line_start,
			header_lws,
			header_name,
			space_before_header_value,
			header_value,
			expecting_newline_2,
			expecting_newline_3
		} state_;
	};
}

#endif // WEBSOCKET_SERVER_REQUEST_PARSER_HPP