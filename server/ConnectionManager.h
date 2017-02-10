#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <string>
#include <vector>
#include <map>
#include "websocket.h"

using namespace std;

class ConnectionManager
{
	websocket* server;
	Model model;
	vector<int> IDs = vector<int>();//if client disconnects and reconnects, it will cause state	
									//to revert
	
	public: 
		ConnectionManager(Server* server);
		
		bool connReady();
		
		void addID(int ID);
		
		void removeID(int ID);
		
		void sendState();		
}

#endif