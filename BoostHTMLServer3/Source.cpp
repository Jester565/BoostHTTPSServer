#include "Server.h"

int main()
{
	websocket::Server* s = new websocket::Server();
	s->createManagers();
	s->asyncInitServer(24560);
	std::string str;
	std::cin >> str;
	delete s;
	return 0;
}
