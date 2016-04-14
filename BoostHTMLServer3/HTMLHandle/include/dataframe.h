#ifndef WEBSOCKET_DATAFRAME_HPP
#define WEBSOCKET_DATAFRAME_HPP

#include <vector>

namespace websocket
{
	class dataframe
	{
	public:
		dataframe();

		bool fin;
		enum operation_code { continuation_frame, text_frame, binary_frame, connection_close, ping, pong, reserved } opcode;
		bool mask;
		int8_t fin_opcode;
		int8_t mask_payload_len;
		int8_t payload_len;
		uint16_t extended_payload_len16;
		uint64_t extended_payload_len64;
		uint8_t masking_key[4];
		std::vector<uint8_t> payload;

		std::vector<uint8_t> to_buffer();
	};
}
#endif
