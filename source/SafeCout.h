#ifndef SAFE_COUT_H_
#define SAFE_COUT_H_

#include "boost\thread\mutex.hpp"

/*Helper functions for debug prints*/

// TODO: set namespaces... or don't do this

static boost::mutex coutLock;

void printCout(std::string);

void printCoutNEndl(std::string);

#endif