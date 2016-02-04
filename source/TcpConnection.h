#ifndef TCP_CONNECTION_H_
#define TCP_CONNECTION_H_

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>

// Client connection class for serverclass.

// TODO: Exception handling

class MessageHandler;

using boost::asio::ip::tcp;

class TcpConnection : public boost::enable_shared_from_this<TcpConnection> ,private boost::noncopyable{
public:
	typedef boost::shared_ptr<TcpConnection> tcp_pointer;

	TcpConnection(boost::asio::io_service&, std::map<int, tcp_pointer>*,MessageHandler*, std::map<std::string, int>*);

	tcp::socket& getSocket(){return socket_;}

	void setId(int id ) {id_=id;};

	void startListening();

	void send(std::string message);

private:

	void handleSend(const boost::system::error_code&error, boost::shared_ptr<std::string>);

	void handleRead(const boost::system::error_code&, size_t);

	void handleName(const boost::system::error_code&, size_t);

	std::map<int, tcp_pointer>* connections_;
	tcp::socket socket_;
	boost::array<char, 128> readBuffer;
	std::map<std::string, int>* connectionsIds_;

	MessageHandler* mHandler_;

	int id_;
};

#endif