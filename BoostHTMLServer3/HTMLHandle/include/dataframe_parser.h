#ifndef WEBSOCKET_DATAFRAME_PARSER_HPP
#define WEBSOCKET_DATAFRAME_PARSER_HPP

#include <cstdint>
#include "tribool.h"
#include <utility>

namespace websocket
{
	class dataframe;

	class dataframe_parser
	{
	public:
		dataframe_parser();

		template <typename InputIterator>
		std::pair<tribool, InputIterator> parse(dataframe& frame,
			InputIterator begin, InputIterator end)
		{
			reset();
			while (begin != end)
			{
				tribool result = consume(frame, *begin++);
				if (result || !result)
					return std::make_pair(result, begin);
			}
			tribool result = indeterminate;
			return std::make_pair(result, begin);
		}

		static uint16_t ntoh16(uint16_t net16);

		static uint16_t hton16(uint16_t net16);

		static uint64_t ntoh64(uint64_t net64);

		static uint64_t hton64(uint64_t net64);

	private:
		tribool consume(dataframe& frame, uint8_t input);

		uint8_t get_bits(char b, uint8_t offset, uint8_t count);

		void reset();

		enum state
		{
			fin_opcode,
			mask_payload_len,
			extended_payload_len1,
			extended_payload_len2,
			extended_payload_len3,
			extended_payload_len4,
			extended_payload_len5,
			extended_payload_len6,
			extended_payload_len7,
			extended_payload_len8,
			masking_key1,
			masking_key2,
			masking_key3,
			masking_key4,
			payload
		} state_;
	};
}
#endif
