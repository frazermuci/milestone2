#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"

using namespace std;

//struct for serialization

webSocket server;
ConnectionManager cm = ConnectionManager(&server);//server is not initialized..well see.

/* called when a client connects */
void openHandler(int clientID)
{
  
	cm.send("init", clientID);
	if(cm.connReady())
	{
		cm.sendIDs();//on client side, wait until "begin"
	}
}

/* called when a client disconnects */
void closeHandler(int clientID)
{   
	cm.removeConn(clientID);
}

vector<string> mVect parseMessage(string message)
{
	vector<string> mVect = vector<string>();
	ostringstream in;
	for(char c : message)
	{
		if(c != ':')
		{
			in << c;
		}
		else
		{
			mVect.push_back(in.str());
			in.str("");
		}
	}
	mVect.push_back(in.str());
	return mVect;
}
/* called when a client sends a message to the server */
void messageHandler(int clientID, string message)
{
		vector<string> mVect = parseMessage(message);
		if(strcmp(mVect.at(0).c_str(), "init") == 0)
		{
			//parse message and get id
			cm.addConn(clientID, atoi(mVect.at(1)));
			cout << "init " << clientID << " " << atoi(mVect.at(1));
			return;
		}
		if(cm.connReady())
		{
			//update model from message
			cout << "desrialize" << endl;//cm.updateModel(deserialize(message));	
		}
		if(cm.stateReady(clientID))
		{			
			//serializing new state
			cout << "sendAll\n";//cm.sendAll(cm.serializeModel());
		}
}

/* called orrnce per select() loop */
void periodicHandler(){
    static time_t next = time(NULL) + 10;
    time_t current = time(NULL);
    if (current >= next){
        ostringstream os;
        string timestring = ctime(&current);
        timestring = timestring.substr(0, timestring.size() - 1);
        os << timestring;

        vector<int> clientIDs = server.getClientIDs();
        for (int i = 0; i < clientIDs.size(); i++)
            server.wsSend(clientIDs[i], os.str());

        next = time(NULL) + 10;
    }
}

int main(int argc, char *argv[]){
    int port;

    cout << "Please set server port: ";
    cin >> port;

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    //server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(port);

    return 1;
}
