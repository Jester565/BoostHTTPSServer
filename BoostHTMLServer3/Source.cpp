#include "Server.h"

int main()
{
	websocket::Server s;
	s.createManagers();
	s.syncInitServer(24560);
}