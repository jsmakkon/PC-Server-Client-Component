#include <boost/bind.hpp>
#include <string>
#include "boost/thread/thread.hpp"
#include "SafeCout.h"
#include "TcpClient.h"

#include "MessageHandler.h"

TcpClient::TcpClient(std::string ip, int port): socket_(new boost::asio::ip::tcp::socket(io_service_)),isAlive_(false)
{
		boost::asio::ip::tcp::resolver resolver(io_service_);
		boost::asio::ip::tcp::resolver::query query(ip, boost::lexical_cast< std::string >( port ));
		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve( query );
		boost::asio::ip::tcp::endpoint endpoint = *iterator;

		socket_->async_connect( endpoint, boost::bind(&TcpClient::onConnect, this, _1) );

}

void TcpClient::startClient() {
		worker_threads_.create_thread(boost::bind( &TcpClient::workerThread, this, &io_service_ ));
}

void TcpClient::send(std::string message) {
	boost::shared_ptr<std::string> a(new std::string(message));
	socket_->async_send(boost::asio::buffer(*a),
		boost::bind(&TcpClient::handleSend,this,boost::asio::placeholders::error, a));
}

void TcpClient::handleSend(const boost::system::error_code&error, boost::shared_ptr<std::string> a) {
	if (error) {
			printCoutNEndl("SERVER SEND ERROR: ");
            printCout(boost::system::system_error(error).what());

			// TODO: Check if something needs to be done here (return info about this error)
	}
}

void TcpClient::onConnect(const boost::system::error_code & ec) {
		if (!ec) {
			startListening();
			isAlive_=true;
		}
		else 
		{
			socket_->cancel(); // TODO: try reconnect or something else depending on the error code
			socket_->close();
		}
}

void TcpClient::startListening() {
	socket_->async_read_some(boost::asio::buffer(readBuffer_),
            boost::bind(&TcpClient::handleRead,this,boost::asio::placeholders::error,_2));
}

void TcpClient::handleRead (const boost::system::error_code&error, size_t len) {

	if (error) {
		printCoutNEndl("CLIENT READ ERROR: ");
        printCout(boost::system::system_error(error).what());
		socket_->cancel();
		socket_->close();
		isAlive_ = false;
	}
	else {
		printCoutNEndl("CLIENT: read: ");
		
		std::string a(readBuffer_.begin(),len);
		printCout(a);
		mHandler_.addMessage(a);
	}

	startListening();

}

void TcpClient::stopClient()
{
	socket_->cancel();
	socket_->close();
	io_service_.stop();
}

void TcpClient::workerThread(boost::asio::io_service* io_service)
{
	while( true )
        {
                try
                {
                        boost::system::error_code ec;
                        io_service->run( ec );
						std::cout << "CLIENT:workerThread() [" << boost::this_thread::get_id()
                                        << "] Error: " << ec << std::endl;
                        if( ec )
                        {
                                std::cout << "[" << boost::this_thread::get_id()
                                        << "] Error: " << ec << std::endl;
                        }
                        break;
                }
                catch( std::exception & ex )
                {
                        std::cout << "CLIENT:workerThread() Exception: " << ex.what() << std::endl;
                }
        }

}