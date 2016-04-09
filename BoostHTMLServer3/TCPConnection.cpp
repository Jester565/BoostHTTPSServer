#include "TCPConnection.h"
#include "PacketManager.h"
#include "HeaderManager.h"
#include "IPacket.h"
#include "OPacket.h"
#include "Reply.h"
#include "ClientManager.h"
#include "dataframe_manager.h"
#include "handshake_manager.h"
#include "dataframe.h"
#include <boost/bind.hpp>

namespace websocket
{
	TCPConnection::TCPConnection(PacketManager* pm, tcp::socket* boundSocket)
		:pm(pm), socket(boundSocket), receiveData(nullptr), errorMode(DEFAULT_ERROR_MODE), cID(0), handshakeComplete(false)
	{
		hm = new HeaderManager();
		dfm = new dataframe_manager();
		hsm = new handshake_manager();
	}

	TCPConnection::TCPConnection(PacketManager* pm, tcp::socket* boundSocket, HeaderManager* hm)
		:pm(pm), socket(boundSocket), hm(hm), receiveData(nullptr), errorMode(DEFAULT_ERROR_MODE)
	{
		dfm = new dataframe_manager();
		hsm = new handshake_manager();
	}

	void TCPConnection::startRead()
	{
		receiveData = new std::vector <unsigned char>();
		receiveData->resize(MAX_DATA_SIZE);
		socket->async_read_some(boost::asio::buffer(*receiveData), boost::bind(&TCPConnection::asyncReceive, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	void TCPConnection::asyncReceive(const boost::system::error_code& error, std::unsigned int nBytes)
	{
		if (error)
		{
			if (error == boost::asio::error::connection_reset)
			{
				pm->getClientManager()->removeClient(cID);
				return;
			}
			std::cerr << "Error occured in TCP Reading: " << error << " - " << error.message() << std::endl;
			switch (errorMode)
			{
			case THROW_ON_ERROR:
				throw error;
				break;
			case RETURN_ON_ERROR:
				return;
			case RECALL_ON_ERROR:
				delete receiveData;
				startRead();
				return;
			};
		}
		if (handshakeComplete)
		{
			dataframe* dataFrame = dfm->parse_data(*receiveData, nBytes);
			if (dataFrame != nullptr)
			{
				if (dataFrame->opcode == dataframe::binary_frame)
				{
					char* dataArr = new char[dataFrame->payload_len];
					for (int i = 0; i < dataFrame->payload_len; i++)
					{
						dataArr[i] = dataFrame->payload.data()[i];
					}
					bool serverRead = false;
					IPacket* iPack = hm->decryptHeader(dataArr, dataFrame->payload_len, serverRead, dataFrame, cID);
					pm->asyncProcess(iPack, serverRead);
				}
				else if (dataFrame->opcode == dataframe::text_frame)
				{
					std::cout << "text_frame was received: " << std::string(dataFrame->payload.at(0), dataFrame->payload_len) << std::endl;
				}
				else if (dataFrame->opcode == dataframe::connection_close)
				{
					std::cout << "connection was closed" << std::endl;
					pm->getClientManager()->removeClient(cID);
					return;
				}
				else
				{
					std::cerr << "A packet with opcode: " << dataFrame->opcode << " was received" << std::endl;
				}
			}
		}
		else
		{
			reply rep = hsm->parse_handshake(*receiveData, nBytes);
			if (rep.status != reply::bad_request)
			{
				handshakeComplete = true;
			}
			send(rep.to_buffer());
		}
		startRead();
	}

	void TCPConnection::send(OPacket* oPack)
	{
		hm->encryptHeader(oPack);
		std::cout << "Sent: " << *oPack << std::endl;
		socket->async_write_some(boost::asio::buffer(oPack->getDataFrame()->to_buffer()), boost::bind(&TCPConnection::asyncSend, shared_from_this(), boost::asio::placeholders::error));
	}

	void TCPConnection::send(const std::vector <uint8_t>& data)
	{
		sendStorage = data;
		async_write(*socket, boost::asio::buffer(sendStorage), boost::bind(&TCPConnection::asyncSend, shared_from_this(), boost::asio::placeholders::error));
	}

	void TCPConnection::asyncSend(const boost::system::error_code& error)
	{
		if (error)
		{
			if (error == boost::asio::error::connection_reset)
			{
				pm->getClientManager()->removeClient(cID);
				return;
			}
			std::cerr << "An error occured in TCP Sending: " << error.message() << std::endl;
			switch (errorMode)
			{
			case THROW_ON_ERROR:
				throw error;
				break;
			case RETURN_ON_ERROR:
				return;
				break;
			};
		}
	}

	TCPConnection::~TCPConnection()
	{
		if (hm != nullptr)
		{
			delete hm;
			hm = nullptr;
		}
		if (receiveData != nullptr)
		{
			delete receiveData;
			receiveData = nullptr;
		}
		boost::system::error_code ec;
		socket->shutdown(tcp::socket::shutdown_both, ec);
		if (ec)
		{
			std::cerr << "Error when closing TCPConnection: " << ec.message() << std::endl;
		}
		socket->close();
		delete socket;
		socket = nullptr;

	}
}
