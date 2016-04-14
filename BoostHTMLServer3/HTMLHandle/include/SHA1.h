/*
sha1.h - header of

============
SHA-1 in C++
============

100% Public Domain.

Original C Code
-- Steve Reid <steve@edmweb.com>
Small changes to fit into bglibs
-- Bruce Guenter <bruce@untroubled.org>
Translation to simpler C++ Code
-- Volker Grabsch <vog@notjusthosting.com>
*/

#ifndef SHA1_HPP
#define SHA1_HPP


#include <iostream>
#include <string>
#include <vector>

namespace websocket
{
	class SHA1
	{
	public:
		SHA1();
		void update(const std::string &s);
		std::vector <unsigned char> final();

	private:
		typedef unsigned long int uint32;   /* just needs to be at least 32bit */
		typedef unsigned long long uint64;  /* just needs to be at least 64bit */

		static const unsigned int DIGEST_INTS = 5;  /* number of 32bit integers per SHA1 digest */
		static const unsigned int BLOCK_INTS = 16;  /* number of 32bit integers per SHA1 block */
		static const unsigned int BLOCK_BYTES = BLOCK_INTS * 4;

		uint32 digest[DIGEST_INTS];
		std::string buffer;
		uint64 transforms;

		void reset();
		void transform(uint32 block[BLOCK_BYTES]);

		static void buffer_to_block(const std::string &buffer, uint32 block[BLOCK_BYTES]);
		static void read(std::istream &is, std::string &s, int max);
	};

	std::vector<unsigned char> to_sha1(const std::string &string);
}

#endif /* SHA1_HPP */