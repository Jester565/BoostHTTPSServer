#pragma once
#include "PacketMacros.h"
#include "reply.h"
#include <utility>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
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

	typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

	class TCPConnection : public boost::enable_shared_from_this <TCPConnection>
	{
	public:
		TCPConnection(PacketManager* pm, ssl_socket* boundSocket);

		TCPConnection(PacketManager* pm, ssl_socket* boundSocket, HeaderManager* hm);

		void startSSLHandhshake();

		void startRead();

		void send(boost::shared_ptr<OPacket> oPack);

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

		ssl_socket* getSocket()
		{
			return socket;
		}

		~TCPConnection();

	private:
		IDType cID;
		std::vector <unsigned char>* receiveData;
		std::vector <uint8_t> sendStorage;
		boost::shared_ptr<dataframe> dfSendStorage;
		ssl_socket* socket;
		PacketManager* pm;
		HeaderManager* hm;
		handshake_manager* hsm;
		dataframe_manager* dfm;
		void asyncReceive(const boost::system::error_code& error, unsigned int nBytes);
		void asyncSSLHandshake(const boost::system::error_code& error);
		void asyncSend(const boost::system::error_code& error);
		int errorMode;
		bool handshakeComplete;
		std::string tempSendStorage;
	};
}
