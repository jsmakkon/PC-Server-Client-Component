#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <list>
#include <map>
#include <string>
#include "MessageHandler.h"

class TcpConnection;
class MessageHandler;

// ------------------------ TCP Server class ---------------------
// Server class to receive and listen for messages.

// TODO: Data serialization, more exception handling, timeouts, etc.

class TcpServer: private boost::noncopyable
{
	/**************** PUBLIC ****************/
public:
	typedef boost::shared_ptr<TcpConnection> tcp_pointer;

	TcpServer(int, int);

	void startServer();

	int getMaxPlayers() {return maxPlayers_;};

	void setMaxPlayers(int n) {maxPlayers_ = n;};

	void sendMessage(int id, std::string message);

	void sendMessage(std::string name, std::string message);

	void sendMessageToAll(std::string message);

	void closeServer();

	MessageHandler* getMessageHandler(){return &mHandler_;};

	/**************** PRIVATE ****************/
private:

	void workerThread(boost::asio::io_service*);

	void start_accept();

	void handle_accept(boost::shared_ptr<TcpConnection>, const boost::system::error_code&);

	boost::asio::io_service io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;

	int maxPlayers_, nextId_;
	// Connections mapped with id...
	std::map<int, boost::shared_ptr<TcpConnection>> connections_;
	// ...and names are connected to ids
	std::map<std::string, int> connectionsIds_;

	boost::thread_group worker_threads_;

	MessageHandler mHandler_;
};

#endif