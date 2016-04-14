#include "reply.h"
namespace websocket
{
	namespace status_strings {

		const std::string switching_protocols =
			"HTTP/1.1 101 Switching Protocols\r\n";
		const std::string bad_request =
			"HTTP/1.1 400 Bad Request\r\n";
		const std::string internal_server_error =
			"HTTP/1.1 500 Internal Server Error\r\n";

		void add_to_buffer(reply::status_type status, std::vector <uint8_t>* buffer)
		{
			switch (status)
			{
			case reply::switching_protocols:
				for (int i = 0; i < switching_protocols.size(); i++)
				{
					buffer->push_back(switching_protocols.at(i));
				}
				return;
			case reply::bad_request:
				for (int i = 0; i < bad_request.size(); i++)
				{
					buffer->push_back(bad_request.at(i));
				}
				return;
			default:
				for (int i = 0; i < internal_server_error.size(); i++)
				{
					buffer->push_back(internal_server_error.at(i));
				}
			}
		}
	}

	namespace misc_strings {
		const char name_value_separator[] = { ':', ' ' };
		const size_t name_value_separator_size = 2;
		const char crlf[] = { '\r', '\n' };
		const size_t crlf_size = 2;
	}

	std::vector<uint8_t> reply::to_buffer()
	{
		std::vector<uint8_t> buffer;
		status_strings::add_to_buffer(status, &buffer);

		for (std::size_t i = 0; i < headers.size(); ++i)
		{
			header& h = headers[i];
			for (int i = 0; i < h.name.size(); i++)
			{
				buffer.push_back(h.name.at(i));
			}
			for (int i = 0; i < misc_strings::name_value_separator_size; i++)
			{
				buffer.push_back(misc_strings::name_value_separator[i]);
			}
			for (int i = 0; i < h.value.size(); i++)
			{
				buffer.push_back(h.value.at(i));
			}
			for (int i = 0; i < misc_strings::crlf_size; i++)
			{
				buffer.push_back(misc_strings::crlf[i]);
			}
		}

		for (int i = 0; i < misc_strings::crlf_size; i++)
		{
			buffer.push_back(misc_strings::crlf[i]);
		}

		return buffer;
	}

	reply reply::stock_reply(reply::status_type status)
	{
		reply rep;
		rep.status = status;

		return rep;
	}
}
