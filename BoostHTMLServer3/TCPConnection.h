#pragma once
#include "PacketMacros.h"
#include "reply.h"
#include <utility>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <stdint.h>

namespace websocket
{
	class PacketManager;
	class HeaderManager;
	class OPacket;
	class handshake_manager;
	class dataframe_manager;

	using namespace boost::asio::ip;

	class TCPConnection : public boost::enable_shared_from_this <TCPConnection>
	{
	public:
		TCPConnection(PacketManager* pm, tcp::socket* boundSocket);

		TCPConnection(PacketManager* pm, tcp::socket* boundSocket, HeaderManager* hm);

		void startRead();

		void send(OPacket* const oPack);

		void send(const std::vector <uint8_t>& data);

		void setErrorMode(int mode)
		{
			errorMode = mode;
		}

		int getErrorMode() const
		{
			return errorMode;
		}

		void setID(IDType id)
		{
			cID = id;
		}

		tcp::socket* getSocket()
		{
			return socket;
		}

		~TCPConnection();

	private:
		IDType cID;
		std::vector <unsigned char>* receiveData;
		std::vector <uint8_t> sendStorage;
		tcp::socket* socket;
		PacketManager* pm;
		HeaderManager* hm;
		handshake_manager* hsm;
		dataframe_manager* dfm;
		void asyncReceive(const boost::system::error_code& error, std::size_t nBytes);
		void asyncSend(const boost::system::error_code& error);
		int errorMode;
		bool handshakeComplete;
		std::string tempSendStorage;
	};
}
