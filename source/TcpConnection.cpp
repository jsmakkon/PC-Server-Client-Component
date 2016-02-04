#include "TcpConnection.h"
#include "SafeCout.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "MessageHandler.h"

#include <string>

TcpConnection::TcpConnection(boost::asio::io_service& io_service, std::map<int, tcp_pointer>* connections,
	MessageHandler* mHandler, std::map<std::string, int>* connectionsIds)
		: socket_(io_service), connections_(connections), connectionsIds_(connectionsIds), mHandler_(mHandler)
	{}

void TcpConnection::startListening(){

		socket_.async_read_some(boost::asio::buffer(readBuffer),
            boost::bind(&TcpConnection::handleRead,this,boost::asio::placeholders::error,_2));

}

void TcpConnection::send(std::string message) {
	boost::shared_ptr<std::string> a(new std::string(message));
	socket_.async_send(boost::asio::buffer(*a),boost::bind(&TcpConnection::handleSend,this,boost::asio::placeholders::error, a));
}

void TcpConnection::handleSend(const boost::system::error_code&error, boost::shared_ptr<std::string> a) {
	if (error) {
			printCoutNEndl("SERVER SEND ERROR: ");
            printCout(boost::system::system_error(error).what());
	}
}

void TcpConnection::handleRead(const boost::system::error_code&error, size_t len)
    {
        printCout("Connection::handleRead()");
        if(error)
        {
            printCoutNEndl("SERVER READ ERROR: ");
            printCout(boost::system::system_error(error).what());
			socket_.cancel();
			socket_.close();
			connections_->erase(id_);
        }
        else
        {
			std::string message = std::to_string(id_);
			message += ":";
			std::string a(readBuffer.begin(),len);
			message+=a;
			mHandler_->addMessage(message);
        }
		if (!error) // Peer closed connection
			startListening();
}

