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
		case 1:
			return Tuple(0, 1);//UP
		case 2:
			return Tuple(0, -1);//DOWN
		case 3:
			return Tuple(-1, 0);//LEFT
		default:
			return Tuple(1, 0);//RIGHT			
	}
}

void ConnectionManager::updateModel(int ClientID, Compressed c)
{
	int ID = this->IDs[ClientID];
	if(c.bonusEaten)
	{
		Snake snake = this->model.getSnake(ID);
		vector<Tuple> bonuses = this->model.getBonuses();
		for(int i = 0; i < bonuses.size(); ++i)
		{
			if(snake.getHead() == bonuses.at(i))//don't know if get head is appropriate
			{
				this->model.makeBonus(i);
				break;
			}
		}
		this->model.growSnake(ID);
	}
	this->model.changeDirection(ID, dirToVect(c.direction));
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


int VectToDir(Tuple vect)
{
	if(vect.getX()!= 0)
	{
		return vect.getX() == 1 ? 4 : 3;
	}
	else
	{
		return vect.getY() == 1 ? 1 : 2;
	}
}

/*Compressed ConnectionManager::compressModel()
{
	Compressed c = {this->model.getSnake()


	int direction;
	bool bonusEaten;
	bool loss;
	int BonusPositionX;
	int BonusPositionY;
}
}*/