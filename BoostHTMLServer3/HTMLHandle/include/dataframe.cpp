#include "dataframe.h"
#include "dataframe_parser.h"

namespace websocket
{
	dataframe::dataframe() : fin(true),
		opcode(binary_frame), mask(false), fin_opcode(0),
		mask_payload_len(0), payload_len(0), extended_payload_len16(0), extended_payload_len64(0) {}

	std::vector<uint8_t> dataframe::to_buffer()
	{
		std::vector<uint8_t> buffer;

		if (fin)
		{
			fin_opcode |= 0x80;
		}

		switch (opcode)
		{
		case continuation_frame:    fin_opcode |= 0x0; break;
		case text_frame:            fin_opcode |= 0x1; break;
		case binary_frame:          fin_opcode |= 0x2; break;
		case connection_close:      fin_opcode |= 0x8; break;
		case ping:                  fin_opcode |= 0x9; break;
		case pong:                  fin_opcode |= 0xA; break;
		default:                    fin_opcode |= 0xF; break;
		}

		buffer.push_back(fin_opcode);

		if (payload.size() < 126)
		{
			mask_payload_len = static_cast<uint8_t>(payload.size());
			buffer.push_back(mask_payload_len);
			for (int i = 0; i < payload.size(); i++)
			{
				buffer.push_back(payload[i]);
			}
		}
		else if (payload.size() < 65536)
		{
			mask_payload_len = 126;
			extended_payload_len16 = static_cast<uint16_t>(payload.size());
			extended_payload_len16 = dataframe_parser::hton16(extended_payload_len16);

			buffer.push_back(mask_payload_len);
			buffer.push_back((uint8_t)((extended_payload_len16 >> 8) & 0xff));
			buffer.push_back((uint8_t)((extended_payload_len16)& 0xff));
			for (int i = 0; i < payload.size(); i++)
			{
				buffer.push_back(payload[i]);
			}
		}
		else
		{
			mask_payload_len = 127;
			buffer.push_back(mask_payload_len);
			extended_payload_len64 = payload.size();
			extended_payload_len64 = dataframe_parser::hton64(extended_payload_len64);
			buffer.push_back((uint8_t)((extended_payload_len64 >> 56) & 0xff));
			buffer.push_back((uint8_t)((extended_payload_len16 >> 48) & 0xff));
			buffer.push_back((uint8_t)((extended_payload_len16 >> 40) & 0xff));
			buffer.push_back((uint8_t)((extended_payload_len16 >> 32) & 0xff));
			buffer.push_back((uint8_t)((extended_payload_len16 >> 24) & 0xff));
			buffer.push_back((uint8_t)((extended_payload_len16 >> 16) & 0xff));
			buffer.push_back((uint8_t)((extended_payload_len16 >> 8) & 0xff));
			buffer.push_back((uint8_t)((extended_payload_len16)& 0xff));

			for (int i = 0; i < payload.size(); i++)
			{
				buffer.push_back(payload[i]);
			}
		}

		return buffer;
	}
}
