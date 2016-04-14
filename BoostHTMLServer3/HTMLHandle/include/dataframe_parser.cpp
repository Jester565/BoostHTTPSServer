#include "dataframe_parser.h"
#include "dataframe.h"

namespace websocket
{
	dataframe_parser::dataframe_parser()
		: state_(fin_opcode)
	{
	}

	void dataframe_parser::reset()
	{
		state_ = fin_opcode;
	}

	uint8_t dataframe_parser::get_bits(char b, uint8_t offset, uint8_t count)
	{
		return (b >> offset) & ((1 << count) - 1);
	}

	uint16_t dataframe_parser::ntoh16(uint16_t net16)
	{
		static const int32_t num = 42;

		// Check the endianness.
		if (*reinterpret_cast<const char*>(&num) == num)
		{
			// Convert to the little-endian.
			uint16_t host16 = ((net16 & 0x00FFULL) << 8) | ((net16 & 0xFF00ULL) >> 8);
			return host16;
		}

		return net16;
	}

	uint16_t dataframe_parser::hton16(uint16_t host16)
	{
		static const int32_t num = 42;

		// Check the endianness.
		if (*reinterpret_cast<const char*>(&num) == num)
		{
			// Convert to the big-endian.
			uint16_t net16 = ((host16 & 0x00FFULL) << 8) | ((host16 & 0xFF00ULL) >> 8);
			return net16;
		}

		return host16;
	}

	uint64_t dataframe_parser::ntoh64(uint64_t net64)
	{
		static const int32_t num = 42;

		// Check the endianness.
		if (*reinterpret_cast<const char*>(&num) == num)
		{
			// Convert to the little-endian.
			uint64_t host64 =
				((net64 & 0x00000000000000FFULL) << 56) |
				((net64 & 0x000000000000FF00ULL) << 40) |
				((net64 & 0x0000000000FF0000ULL) << 24) |
				((net64 & 0x00000000FF000000ULL) << 8) |
				((net64 & 0x000000FF00000000ULL) >> 8) |
				((net64 & 0x0000FF0000000000ULL) >> 24) |
				((net64 & 0x00FF000000000000ULL) >> 40) |
				((net64 & 0xFF00000000000000ULL) >> 56);

			return host64;
		}

		return net64;
	}

	uint64_t dataframe_parser::hton64(uint64_t host64)
	{
		static const int32_t num = 42;

		// Check the endianness.
		if (*reinterpret_cast<const char*>(&num) == num)
		{
			// Convert to the big-endian.
			uint64_t net64 =
				((host64 & 0x00000000000000FFULL) << 56) |
				((host64 & 0x000000000000FF00ULL) << 40) |
				((host64 & 0x0000000000FF0000ULL) << 24) |
				((host64 & 0x00000000FF000000ULL) << 8) |
				((host64 & 0x000000FF00000000ULL) >> 8) |
				((host64 & 0x0000FF0000000000ULL) >> 24) |
				((host64 & 0x00FF000000000000ULL) >> 40) |
				((host64 & 0xFF00000000000000ULL) >> 56);

			return net64;
		}

		return host64;
	}

	tribool dataframe_parser::consume(dataframe& frame, uint8_t input)
	{
		switch (state_)
		{
		case fin_opcode:
		{
			frame.fin = get_bits(input, 7, 1) == 1;

			switch (get_bits(input, 0, 4))
			{
			case 0:
				frame.opcode = dataframe::continuation_frame;
				break;
			case 0x1:
				frame.opcode = dataframe::text_frame;
				break;
			case 0x2:
				frame.opcode = dataframe::binary_frame;
				break;
			case 0x8:
				frame.opcode = dataframe::connection_close;
				break;
			case 0x9:
				frame.opcode = dataframe::ping;
				break;
			case 0xA:
				frame.opcode = dataframe::pong;
				break;
			default:
				frame.opcode = dataframe::reserved;
			}

			state_ = mask_payload_len;

			return indeterminate;
		}
		case mask_payload_len:
		{
			frame.mask = get_bits(input, 7, 1) == 1;
			frame.payload_len = get_bits(input, 0, 7);

			if (frame.payload_len == 0)
			{
				return true;
			}
			else if (frame.payload_len == 126 || frame.payload_len == 127)
			{
				state_ = extended_payload_len1;
			}
			else
			{
				if (frame.mask)
				{
					state_ = masking_key1;
				}
				else
				{
					state_ = payload;
				}
			}

			return indeterminate;
		}
		case extended_payload_len1:
		{
			if (frame.payload_len == 126)
			{
				frame.extended_payload_len16 = input;
			}
			else if (frame.payload_len == 127)
			{
				frame.extended_payload_len64 = input;
			}

			state_ = extended_payload_len2;

			return indeterminate;
		}
		case extended_payload_len2:
		{
			if (frame.payload_len == 126)
			{
				uint16_t temp = input;
				temp = temp << 8;
				frame.extended_payload_len16 |= temp;
				frame.extended_payload_len16 = ntoh16(frame.extended_payload_len16);

				if (frame.mask)
				{
					state_ = masking_key1;
				}
				else
				{
					state_ = payload;
				}
			}
			else if (frame.payload_len == 127)
			{
				uint64_t temp = input;
				temp = temp << 8;
				frame.extended_payload_len64 |= temp;

				state_ = extended_payload_len3;
			}

			return indeterminate;
		}

		case extended_payload_len3:
		{
			uint64_t temp = input;
			temp = temp << 16;
			frame.extended_payload_len64 |= temp;

			state_ = extended_payload_len4;

			return indeterminate;
		}

		case extended_payload_len4:
		{
			uint64_t temp = input;
			temp = temp << 24;
			frame.extended_payload_len64 |= temp;

			state_ = extended_payload_len5;

			return indeterminate;
		}
		case extended_payload_len5:
		{
			uint64_t temp = input;
			temp = temp << 32;
			frame.extended_payload_len64 |= temp;

			state_ = extended_payload_len6;

			return indeterminate;
		}
		case extended_payload_len6:
		{
			uint64_t temp = input;
			temp = temp << 40;
			frame.extended_payload_len64 |= temp;

			state_ = extended_payload_len7;

			return indeterminate;
		}
		case extended_payload_len7:
		{
			uint64_t temp = input;
			temp = temp << 48;
			frame.extended_payload_len64 |= temp;

			state_ = extended_payload_len8;

			return indeterminate;
		}
		case extended_payload_len8:
		{
			uint64_t temp = input;
			temp = temp << 56;
			frame.extended_payload_len64 |= temp;
			frame.extended_payload_len64 = ntoh64(frame.extended_payload_len64);

			if (frame.mask)
			{
				state_ = masking_key1;
			}
			else
			{
				state_ = payload;
			}

			return indeterminate;
		}
		case masking_key1:
		{
			frame.masking_key[0] = input;
			state_ = masking_key2;

			return indeterminate;
		}
		case masking_key2:
		{
			frame.masking_key[1] = input;
			state_ = masking_key3;

			return indeterminate;
		}
		case masking_key3:
		{
			frame.masking_key[2] = input;
			state_ = masking_key4;

			return indeterminate;
		}
		case masking_key4:
		{
			frame.masking_key[3] = input;
			state_ = payload;

			return indeterminate;
		}
		case payload:
		{
			uint8_t mask = frame.masking_key[frame.payload.size() % 4];
			frame.payload.push_back(input ^ mask);

			if (frame.payload_len == 127)
			{
				if (frame.payload.size() == frame.extended_payload_len64)
					return true;
				else
					return indeterminate;
			}
			else if (frame.payload_len == 126)
			{
				if (frame.payload.size() == frame.extended_payload_len16)
					return true;
				else
					return indeterminate;
			}
			else
			{
				if (frame.payload.size() == static_cast<std::vector<char>::size_type>(frame.payload_len))
					return true;
				else
					return indeterminate;
			}
		}

		default:
			return false;
		}
	}
}
