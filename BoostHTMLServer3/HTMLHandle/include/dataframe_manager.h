#include <vector>

namespace websocket
{
	class dataframe_parser;
	class dataframe;

	class dataframe_manager
	{
	public:
		const static size_t USE_VEC_SIZE = 0;

		dataframe_manager();

		dataframe* parse_data(const std::vector <unsigned char>& data, size_t size);

		~dataframe_manager();

	protected:
		dataframe_parser* df_parser;
	};
}
