#include "TcpServer.h"
#include "SafeCout.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include "MessageHandler.h"
#include "TcpConnection.h"

TcpServer::TcpServer(int maxPlayers, int port):acceptor_(io_service_),maxPlayers_(maxPlayers),nextId_(0)
	{
		boost::asio::ip::tcp::resolver resolver(io_service_);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), boost::lexical_cast<std::string>(port));
		boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
		acceptor_.open(endpoint.protocol());
		acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor_.bind(endpoint);
		acceptor_.listen();
	}

void TcpServer::startServer() {
		start_accept();
		worker_threads_.create_thread(boost::bind( &TcpServer::workerThread, this, &io_service_ ));
		
}

void TcpServer::sendMessage(std::string name, std::string message) {
	int id = connectionsIds_.find(name)->second; 
	sendMessage(id, message);
}

void TcpServer::sendMessage(int id, std::string message) {
		tcp_pointer a = connections_.find(id)->second;
		a->send(message);
}

void TcpServer::sendMessageToAll(std::string message) {
	for(std::map<int,tcp_pointer>::iterator it = connections_.begin();it != connections_.end(); it++) {
		if (it != connections_.begin())
			it->second->send(message);
	}
}

// Calls io_service->run, TODO: check if threads close
void TcpServer::workerThread(boost::asio::io_service* io_service)
{
	while( true )
	{
		try
		{
			boost::system::error_code ec;
			io_service->run( ec );
			std::cout << "[" << boost::this_thread::get_id()
				<< "] Error: " << ec << std::endl;
			if( ec )
			{
				std::cout << "[" << boost::this_thread::get_id()
					<< "] Error: " << ec << std::endl;
			}
			//break;
		}
		catch( std::exception & ex )
		{
			std::cout << "[" << boost::this_thread::get_id()
				<< "] Exception: " << ex.what() << std::endl;
		}
	}

}

void TcpServer::closeServer()
{
	acceptor_.cancel();
	acceptor_.close();
	io_service_.stop();
}

void TcpServer::start_accept(){

		tcp_pointer newConnection(
			new TcpConnection(io_service_, &connections_, &mHandler_, &connectionsIds_));

		acceptor_.async_accept(newConnection->getSocket(),
			boost::bind(&TcpServer::handle_accept, this, newConnection, boost::asio::placeholders::error));

		//std::cout << "start_accept, done"<< std::endl;
}

void TcpServer::handle_accept(tcp_pointer newConnection, const boost::system::error_code& error)
{
		if (!error)
		{
			printCout("No error, creating new connection");
			newConnection->setId(nextId_);
			nextId_++;
			connections_.insert(std::pair<int, tcp_pointer>(nextId_, newConnection));
			newConnection->startListening();
		}
		printCout("Number of connections: "+std::to_string(connections_.size()));
		start_accept();
}