#ifndef MANAGECONNECTION_H
#define MANAGECONNECTION_H

#include <string>
#include <vector>
#include <map>
#include "websocket.h"
#include  "tuple.h"
#include "model.h"

//using namespace std;

typedef struct Compressed
{
	int s1Direction; //1 = UP, 2 = DOWN, 3 = LEFT, 4 = RIGHT
	bool s1BonusEaten;
	bool s1Loss;
	int s1BonusPositionX;
	int s1BonusPositionY;
	int s2Direction;
	bool s2BonusEaten;
	bool s2Loss;
	int s2BonusPositionX;
	int s2BonusPositionY;
} Compressed;

class ConnectionManager
{
	webSocket* server;
	Model model;
	map<int,int> IDs;// = map<int,int>();//if client disconnects and reconnects, it will cause state	
	//map<int,int> stateReady;//= map<int, int>();							//to revert
	map<int, bool> state;
	map<int, int> clientIDWithConnNum;
	Compressed c;
	
	public: 
		ConnectionManager(webSocket *server, int width, int height);
		void connNumWithClientID(int clientID, int connNum);
		bool connReady();		
		void addConn(int ID, int snakeID);		
		void removeConn(int ID);		
		void sendState();		
		void send(int clientID, string message);		
		void sendAll(string message);		
		void sendIDs();		
		string serializeModel();		
		void updateModel(int clientID, Compressed c);		
		void addSnake(int clientID, int x, int y, Tuple direction);		
		void removeSnake(int clientID);	
		bool stateReady(int clientID);		
		void moveModel();
		void newGame();
		//Compressed compressModel();
		char* serialize();
};

#endif