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
	map<int,int> IDs = map<int,int>();//if client disconnects and reconnects, it will cause state	
	map<int,int> stateReady = map<int, int>();							//to revert
	
	public: 
		ConnectionManager(Server* server);
		
		bool connReady();
		
		void addConn(int ID, int snakeID);
		
		void removeConn(int ID);
		
		void sendState();
		
		void send(string message, int clientID);
		
		void sendAll(string message);
		
		void sendIDs();
		
		string serializeModel();
}

#endif