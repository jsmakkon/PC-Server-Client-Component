#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_

#include <string>
#include "boost/lexical_cast.hpp"
#include "boost\asio.hpp"
#include "boost/thread.hpp"
#include <boost\array.hpp>
#include "MessageHandler.h"

// Client class.

// TODO: Exception handling, reconnects..

class TcpClient {
public:
	TcpClient(std::string, int);

	void startClient();
	
	void send(std::string message);

	MessageHandler* getMessageHandler(){return &mHandler_;};

	void stopClient();

	bool isAlive() {return isAlive_;};

private:

	void handleSend(const boost::system::error_code&, boost::shared_ptr<std::string>);

	void startListening();

	void workerThread(boost::asio::io_service*);

	void onConnect(const boost::system::error_code & ec);

	void handleRead(const boost::system::error_code&error, size_t len);
	

	boost::asio::io_service io_service_;

	boost::shared_ptr< boost::asio::ip::tcp::socket > socket_;

	boost::array<char, 128> readBuffer_;

	boost::thread_group worker_threads_;

	MessageHandler mHandler_;
	bool isAlive_;
};

#endif