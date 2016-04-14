#ifndef BASE64_CONVERTER_HPP
#define BASE64_CONVERTER_HPP

#include <vector>
#include <string>

namespace websocket
{
	class Base64
	{
	public:
		static std::string to_base64(std::vector <unsigned char>& data);

	private:
		static const std::string base64Chars;
	};
}
#endif
