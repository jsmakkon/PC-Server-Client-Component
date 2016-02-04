#include "MessageHandler.h"

#include "boost\algorithm\string.hpp"
#include "boost\bind.hpp"
#include "boost\thread.hpp"
#include "boost\thread\locks.hpp"

#include "TcpServer.h"

#include <vector>

MessageHandler::MessageHandler() {
}

// Adds message to queue, TODO: add max. capacity to queue and wait here
void MessageHandler::addMessage(std::string message)
{
	boost::unique_lock<boost::mutex> lock(queueLock_);

	messageQueue_.push(message);

	cond.notify_one();
}

// Take message from queue or wait until one appears (blocks)
std::string MessageHandler::takeMessage() {

	boost::unique_lock<boost::mutex> lock(queueLock_);

	while (messageQueue_.empty())
	{
		cond.wait(lock);
	}
	std::string message = messageQueue_.front().data();
	messageQueue_.pop();
	return message;
}

// Take all messages from queue, doesn't block.
std::vector<std::string> MessageHandler::takeAll()
{
	std::vector<std::string> messages;
	boost::unique_lock<boost::mutex> lock(queueLock_);

	while (!messageQueue_.empty())
	{
		messages.push_back(messageQueue_.front().data());
		messageQueue_.pop();
	}

	return messages;
}
