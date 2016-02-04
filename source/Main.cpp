#include "SafeCout.h"

#include <iostream>
#include <string>
#include "TcpServer.h"
#include "TcpClient.h"

/*
	Test main for server and client interaction.
*/

using namespace std;

bool running = true;
int port = 46723;


void clientThread(int i)
{
	TcpClient *client = new TcpClient("127.0.0.1", port);
	MessageHandler* mh = client->getMessageHandler();
	client->startClient();
	int a = 0;

	while (running)
	{
		string mess("Thread: "+to_string(i)+", Code: "+to_string(a));
		client->send(mess);
		a++;
		boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
	}
	client->stopClient();
}

int main() 
{
	
	string input;
	printCout("Server or client? (s/c)");
	cin >> input;

	if (input.at(0) == 's')
	{
		TcpServer *server = new TcpServer(5,port); // Send messages to this object
		MessageHandler* mh = server->getMessageHandler(); // Take messages from this object
		server->startServer();
		printCout("Server started!");
		while(1)
		{
			input = mh->takeMessage();
			printCout("Server got: "+input);
			if (input.find("exit")!=string::npos) break;
		}
		server->closeServer();
	}
	else
	{
		string input;
		
		printCout("Start spam test? (y/n)");
		cin >> input;
		if (input =="y")
		{
			// Start spam test by creating 10 threads to spam messages to server
			boost::thread_group threads_;
			for (int i = 0; i < 10; i++)
				threads_.add_thread(new boost::thread(clientThread,i));
			cin >>input;

			running = false;
			for (int i = 0; i < 10; i++)
				threads_.join_all();
		}
		else 
		{
			// Connect to server normally
			TcpClient *client = new TcpClient("127.0.0.1", port);
			MessageHandler* mh = client->getMessageHandler();
			client->startClient();
			int i=0;
			// Wait max. 5 seconds to connect
			while (!client->isAlive() || i < 5)
			{
				boost::this_thread::sleep( boost::posix_time::seconds(1) );
				i++;
			}

			// Should be connected now
			while(client->isAlive())
			{
				cin >> input;
				client->send(input);
				if (input == "exit") break;
			}
			client->stopClient();
		}
	}
	printCout("Program ended");

	return 1;
}