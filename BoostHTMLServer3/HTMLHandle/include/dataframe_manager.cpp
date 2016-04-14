#include "dataframe_manager.h"
#include "dataframe.h"
#include "dataframe_parser.h"
#include <utility>

namespace websocket
{
	dataframe_manager::dataframe_manager()
	{
		df_parser = new dataframe_parser();
	}

	dataframe* dataframe_manager::parse_data(const std::vector <unsigned char>& data, size_t size)
	{
		std::vector <unsigned char>::const_iterator end;
		if (size == USE_VEC_SIZE) {
			end = data.end();
		}
		else {
			end = data.begin() + size;
		}
		dataframe* df = new dataframe();
		std::pair <tribool, std::vector <unsigned char>::const_iterator> pResult = df_parser->parse(*df, data.begin(), end);
		tribool result = pResult.first;
		if (result)
		{
			return df;
		}
		return nullptr;
	}

	dataframe_manager::~dataframe_manager()
	{
	}
}
