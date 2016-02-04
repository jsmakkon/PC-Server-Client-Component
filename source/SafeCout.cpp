#include "SafeCout.h"
#include "boost\thread\mutex.hpp"
#include <iostream>
#include <string>

void printCout(std::string message)
{
	boost::mutex::scoped_lock lock(coutLock);
	std::cout << message << std::endl;
}

void printCoutNEndl(std::string message)
{
	boost::mutex::scoped_lock lock(coutLock);
	std::cout << message;
}