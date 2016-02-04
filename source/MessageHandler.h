#ifndef MESSAGEHANDLER_H_
#define MESSAGEHANDLER_H_

#include "boost\thread\mutex.hpp"
#include "boost\thread\thread.hpp"
#include "boost\thread\condition_variable.hpp"

//#pragma warning( disable : 4355 )
#pragma warning(disable:4996) 

#include <string>
#include <queue>

class TcpServer;
class PlayersData;

// Message handler class for multithreaded message sending between components.

// Simply create the object and start adding and taking messages (strings). 
// Currently, the functions will block, TODO: non-blocking functionality

class MessageHandler{

public:
	MessageHandler(); 

	void addMessage(std::string message);

	std::string takeMessage();

	std::vector<std::string> takeAll();

private:
	
	std::queue<std::string> messageQueue_;
	boost::mutex queueLock_;
	boost::condition_variable cond;
};

#endif