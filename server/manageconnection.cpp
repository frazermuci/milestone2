#include "tuple.h"
#include "manageconnection.h"
#include "websocket.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
//#include <string>

using namespace std;

ConnectionManager::ConnectionManager(webSocket *server, int width, int height)
{
	this->server = server;
	this->IDs = map<int,int>();//if client disconnects and reconnects, it will cause state	
	//this->stateReady = map<int, int>();	
	this->state = map<int,bool>();
	this->model	 = Model(width, height); 
	this->clientIDWithConnNum = map<int,int>();
}

void ConnectionManager::connNumWithClientID(int clientID, int connNum)
{
	this->clientIDWithConnNum[ClientID] = connNum;
}

bool ConnectionManager::connReady()
{
	return this->IDs.size() == 2;//means it won't work for more than 2
}

void ConnectionManager::addConn(int ID, int snakeID)
{
	this->IDs[ID] = snakeID;
}

void ConnectionManager::removeConn(int ID)
{
	map<int, int> temp = map<int,int>();
	map<int,int>::iterator i;
	for(i = this->IDs.begin(); i!= this->IDs.end();++i)
	{
		if(i->first != ID)
		{
			temp[i->first] = i->second;
		}
	}
	this->IDs = temp;
}

void ConnectionManager::send(int clientID, string message)
{
	this->server->wsSend(clientID, message);
}

void ConnectionManager::sendAll(string message)
{
	map<int, int>::iterator it;
    for (it = this->IDs.begin(); it != this->IDs.end(); it++)
	{
		this->server->wsSend(it->first, message);
	}
}

void ConnectionManager::sendIDs()
{
	ostringstream os;
	map<int,int>::iterator a, b;
	for(a = this->IDs.begin(); a != this->IDs.end(); a++)
	{
		for(b = this->IDs.begin(); b != this->IDs.end(); b++)
		{
			if(b->first != a->first)
			{
				os << b->second;
				this->server->wsSend(a->first, os.str());
				os.str("");
			}
		}
	}
}

bool ready(map<int,bool> state)
{
	return state.size() == 2;
}

bool ConnectionManager::stateReady(int clientID)
{
	this->state[clientID] = true;
	
	if(ready(this->state))
	{
		this->state = map<int,bool>();
	}
	return ready(this->state);
}

Tuple dirToVect(int dir)
{
	switch(dir)
	{
		case 0:
			return Tuple(1, 0);//Right
		case 1:
			return Tuple(0, 1);//Up
		case 2:
			return Tuple(-1, 0);//Left
		default:
			return Tuple(0, -1);//Down			
	}
}


void handleS1(int ID, Compressed c, ConnectionManager* cm)
{
	Snake snake = cm->model.getSnake(ID);
	vector<Tuple> bonuses = cm->model.getBonuses();
	if(c.s1BonusEaten)
	{
		for(int i = 0; i < bonuses.size(); ++i)
		{
			if(snake.getHead() == bonuses.at(i))//don't know if get head is appropriate
			{
				cm->model.makeBonus(i);
				break;
			}
		}
		model->growSnake(ID);
	}
	cm->changeDirection(ID, dirToVect(c.s1Direction));
	cm->c.s1Direction  = c.s1Direction;
	cm->c.s1BonusEaten = c.s1BonusEaten;
	cm->c.s1Loss		  = c.s1Loss;
	cm->c.s1BonusPositionX = c.s1BonusPositionX;
	cm->c.s1BonusPositionY = c.s1BonusPositionY;
}

void handleS1(int ID, Compressed c, ConnectionManager* cm)
{
	Snake snake = cm->model.getSnake(ID);
	vector<Tuple> bonuses = cm->model.getBonuses();
	if(c.s2BonusEaten)
	{
		for(int i = 0; i < bonuses.size(); ++i)
		{
			if(snake.getHead() == bonuses.at(i))//don't know if get head is appropriate
			{
				cm->model.makeBonus(i);
				break;
			}
		}
		model->growSnake(ID);
	}
	cm->changeDirection(ID, dirToVect(c.s2Direction));
	cm->c.s2Direction  = c.s2Direction;
	cm->c.s2BonusEaten = c.s2BonusEaten;
	cm->c.s2Loss		  = c.s2Loss;
	cm->c.s2BonusPositionX = c.s2BonusPositionX;
	cm->c.s2BonusPositionY = c.s2BonusPositionY;
}

void ConnectionManager::updateModel(int clientID, Compressed c)
{
	int ID = this->IDs[clientID];
	
	if(this->clientIDWithConnNum[clientID] == 0)
	{//S1		
		handleS1(ID, c, this);
	}
	else
	{//S2
		handleS2(ID, c, this);
	}
}

void ConnectionManager::addSnake(int clientID, int x, int y, Tuple direction)
{
	this->model.addSnake(this->IDs[clientID], Snake(x, y, direction, this->IDs[clientID]));
}
		
void ConnectionManager::removeSnake(int clientID)
{
	this->model.removeSnake(this->IDs[clientID]);
}

void ConnectionManager::moveModel()
{
	this->model.move();
}

void ConnectionManager::newGame()
{
	this->model.newGame();
	//this->model.isRunning = 1;
}

/*int vectToDir(Tuple vect)
{
	if(vect.getX()!= 0)
	{
		return vect.getX() == 1 ? 4 : 3;
	}
	else
	{
		return vect.getY() == 1 ? 1 : 2;
	}
}*/

/*int findClientByConnNum(int connNum)
{
	for(auto it = this->clientIDWithConnNum.begin(); it!=this->clientIDWithConnNum.end();++it)
	{
		if(it->second == connNum)
		{
			return it->first;
		}
	}
}*/

/*Compressed ConnectionManager::compressModel()
{
	return this->model.c;
	//int client0 = findClientByConnNum(0);
	//int client1 = findClientByConnNum(1);
	Compressed c = 
	{
		//int s1Direction; //1 = UP, 2 = DOWN, 3 = LEFT, 4 = RIGHT
		//bool s1BonusEaten;
		//bool s1Loss;
		//int s1BonusPositionX;
		//int s1BonusPositionY;
		//vectToDir(this->model.getSnake(this->IDs[client0]).getDirection()),
		this->model.c.s1Direction
		this->model.c.s1BonusEaten,
		this->model.c.s1Loss,
		this->model.c.s1BonusPositionX,
		this->model,
		vectToDir(this->model.getSnake(this->IDs[client0]).getDirection()),
		,
		,
		,
		,
	};
	//return c;
}*/

char* ConnectionManager::serialize()
{
    unsigned char* s = malloc(sizeof(char)*4);
    int i = 1;
    
    s[0] = 0;
    
    // s1Dir s1Dir s1Bonus s1Loss s2Dir s2Dir s2Bonus s2Loss
    if(this->c.s1Direction == 0)    // Right
        s[0] += 64;// 01 000000
    else if(this->c.s1Direction == 1) // Up
        s[0] += 128+64;// 11 000000
    else if(this->c.s1Direction == 2) // Left
        s[0] += 0; // 00 000000
    else // Right
        s[0] += 128; // 10 000000
    
    if(this->c.s1Loss)
        s[0] += 32; // 00 1 00000
    
    if(this->c.s1BonusEaten)
    {
        s[0] += 16; // 000 1 0000
        s[i] = (this->c.s1BonusPositionX % 16)*16 + (this->c.s1BonusPositionY % 16);
        i++;
    }
    
    
    if(this->c.s2Direction == 0)    // Right
        s[0] += 4;// 0000 01 00
    else if(this->c.s2Direction == 1) // Up
        s[0] += 8+4;// 0000 11 00
    else if(this->c.s2Direction == 2) // Left
        s[0] += 0; // 0000 00 00
    else // Down
        s[0] += 8; // 0000 10 00
    
    if(this->c.s1Loss)
        s[0] += 2; // 000000 1 0
    
    if(this->c.s2BonusEaten)
    {
        s[0] += 1; // 0000000 1
        s[i] = (this->c.s2BonusPositionX % 16)*16 + (this->c.s2BonusPositionY % 16);
        i++;
    }
        
    s[i] = '\0';
    
    return s;
}