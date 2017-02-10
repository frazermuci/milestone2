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
void openHandler(int clientID){
    ostringstream os;
  
	cm.addID(clientID);
	if(cm.connReady())
	{
		cm.init("init");//on client side, wait until "begin"
	}
}

/* called when a client disconnects */
void closeHandler(int clientID){   
	cm.removeID(clientID);
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message){
		
		if(cm.connReady() && cm.stateReady(clientID))//for the compact object, we can do a constructor that takes in
						  //the full state and prunes it
		{
			//loading ClientState from message
			//cm.deserializeMessage(message,&state_struct);// this will be client
			cm.updateModel(clientID);
			
			
			//serializing new state
			cm.sendState(cm.serializeModel());
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
