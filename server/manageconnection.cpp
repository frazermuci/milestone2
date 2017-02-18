#include "tuple.h"
#include "manageconnection.h"
#include "websocket.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
//#include <string>

using namespace std;

ConnectionManager::ConnectionManager(webSocket *server, int width, int height)
{
	this->server = server;
	this->IDs = map<int,int>();//if client disconnects and reconnects, it will cause state
	this->state = map<int,bool>();
	this->model	 = Model(width, height); 
	this->clientIDWithConnNum = map<int,int>();
}

void ConnectionManager::printIDs()
{
	map<int,int>::iterator it;
	for(it = this->IDs.begin(); it != this->IDs.end(); ++it)
	{
		cout << "clientID: " << it->first << " UserID: " << it->second << endl;
	}
}

void ConnectionManager::printPos()
{
	vector<Snake*> snakes = model.getSnakes();
	for(vector<Snake*>::iterator it = snakes.begin(); it != snakes.end(); ++it)
	{
		vector<Tuple> bod = (*it)->getBody();
		cout << "snake: " << endl;
		for(vector<Tuple>::iterator inner = bod.begin(); inner != bod.end(); ++inner)
		{
			cout << "x: "<<inner->getX() << " y: " << inner->getY() << endl;
		}
		cout << endl;
	}
}
void ConnectionManager::connNumWithClientID(int clientID, int connNum)
{
	this->clientIDWithConnNum[clientID] = connNum;
}

int ConnectionManager::getConnNum(int clientID)
{
	return this->clientIDWithConnNum[clientID];
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
				os <<"start:"<< b->second << ":"<<this->clientIDWithConnNum[b->first];
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
	bool r = ready(this->state);
	if(r)
	{
		this->state = map<int,bool>();
	}
	return r;
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


void ConnectionManager::handleS1(int ID, Compressed c)
{
	Snake* snake = this->model.getSnake(ID);
	vector<Tuple> bonuses = this->model.getBonuses();
	if(c.s1BonusEaten)
	{
		for(int i = 0; i < bonuses.size(); ++i)
		{
			if(snake->getHead() == bonuses.at(i))//don't know if get head is appropriate
			{
				this->model.makeBonus(i);
				break;
			}
		}
		this->model.growSnake(ID);
	}
	this->model.changeDirection(ID, dirToVect(c.s1Direction));
	this->c.s1Direction  = c.s1Direction;
	this->c.s1BonusEaten = c.s1BonusEaten;
	this->c.s1Loss		  = c.s1Loss;
	this->c.s1BonusPositionX = c.s1BonusPositionX;
	this->c.s1BonusPositionY = c.s1BonusPositionY;
}

void ConnectionManager::handleS2(int ID, Compressed c)
{
	Snake* snake = this->model.getSnake(ID);
	vector<Tuple> bonuses = this->model.getBonuses();
	if(c.s2BonusEaten)
	{
		for(int i = 0; i < bonuses.size(); ++i)
		{
			if(snake->getHead() == bonuses.at(i))//don't know if get head is appropriate
			{
				this->model.makeBonus(i);
				break;
			}
		}
		this->model.growSnake(ID);
	}
	this->model.changeDirection(ID, dirToVect(c.s2Direction));
	this->c.s2Direction  = c.s2Direction;
	this->c.s2BonusEaten = c.s2BonusEaten;
	this->c.s2Loss		  = c.s2Loss;
	this->c.s2BonusPositionX = c.s2BonusPositionX;
	this->c.s2BonusPositionY = c.s2BonusPositionY;
}

void ConnectionManager::updateModel(int clientID, int newDir)
{
	int ID = this->clientIDWithConnNum[clientID];
	this->model.changeDirection(ID, dirToVect(newDir));
}

void ConnectionManager::addSnake(int clientID, int x, int y, Tuple direction)
{
	int connNum = this->clientIDWithConnNum[clientID];
	this->model.addSnake(connNum, new Snake(x, y, direction, connNum));
}
		
void ConnectionManager::removeSnake(int clientID)
{
	int connNum = this->clientIDWithConnNum[clientID];
	this->model.removeSnake(connNum);
}



int vectToDir(Tuple vect)
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

void ConnectionManager::moveModel(Compressed* c)
{
	

    // Grow both snakes
    this->model.growSnake(0);
    this->model.growSnake(1);

    Snake* snake1 = this->model.getSnake(0);
    Snake* snake2 = this->model.getSnake(1);

    Tuple head1 = snake1->getHead();
    Tuple head2 = snake2->getHead();

    vector<Tuple> body1 = snake1->getBody();
    vector<Tuple> body2 = snake2->getBody();

    // Check collision for snakes
    bool lose1 = false;
    bool lose2 = false;

    // Heads colliding
    /*if(head1.equals(head2))
    {
        lose1 = true;
        lose2 = true;
    }*/

    // Out of the board
    if(!(head1.getX() >= 0 && head1.getX() < this->model.getBoardWidth() && head1.getY() >= 0 && head1.getY() < this->model.getBoardHeight()))
        lose1 = true;
    if(!(head2.getX() >= 0 && head2.getX() < this->model.getBoardWidth() && head2.getY() >= 0 && head2.getY() < this->model.getBoardHeight()))
        lose2 = true;

    // Colliding with other snake
    for(int i = 0; i < body1.size(); i++)
    {
        if(head2 == body1[i])
            lose2=true;
    }
    for(int i = 0; i < body2.size(); i++)
    {
        if(head1 == body2[i])
            lose1=true;
    }

    // Colliding with yourself
    for(int i = 1; i < body1.size(); i++)
    {
        if(head1 == body1[i])
            lose1=true;
    }
    for(int i = 1; i < body2.size(); i++)
    {
        if(head2 == body2[i])
            lose2=true;
    }
	
	c->s1Direction = vectToDir(snake1->getDirection());
	c->s2Direction = vectToDir(snake2->getDirection());
	c->s1Loss = lose1;
	c->s2Loss = lose2;
	c->s1BonusEaten = false;
	c->s2BonusEaten = false;
	
	
    // Check bonus (head at bonus position)
	vector<Tuple> bonuses = this->model.getBonuses();
    for(int i = 0; i < bonuses.size(); i++)
	{
		if(head1 == bonuses[i])
		{
			snake1->eatBonus();
			Tuple newB = this->model.makeBonus(i);
			c->s1BonusEaten = true;
			c->s1BonusPositionX = newB.getX();
			c->s1BonusPositionY = newB.getY();
		}
		if(head2 == bonuses[i])
		{
			snake2->eatBonus();
			Tuple newB = this->model.makeBonus(i);
			c->s2BonusEaten = true;
			c->s2BonusPositionX = newB.getX();
			c->s2BonusPositionY = newB.getY();
		}
	}
	
	//c->s1Dir = 
	
	/*
	map<int, int>::iterator it;
	for(it = this->IDs.begin(); it != this->IDs.end(); ++it)
	{
		cout << "connNum: " << this->clientIDWithConnNum[it->first] << endl;
		this->model.growSnake(this->clientIDWithConnNum[it->first]);
	}*/
}

void ConnectionManager::newGame()
{
	this->model.newGame();
	//this->model.isRunning = 1;
}

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

unsigned char* ConnectionManager::serialize(Compressed* c)
{
    unsigned char* s = static_cast<unsigned char*>(malloc(sizeof(unsigned char)*4));
    int i = 1;
    
    s[0] = 0;
    
    // s1Dir s1Dir s1Bonus s1Loss s2Dir s2Dir s2Bonus s2Loss
    if(c->s1Direction == 0)    // Right
        s[0] += 64;// 01 000000
    else if(c->s1Direction == 1) // Up
        s[0] += 128+64;// 11 000000
    else if(c->s1Direction == 2) // Left
        s[0] += 0; // 00 000000
    else // Right
        s[0] += 128; // 10 000000
    
    if(c->s1Loss)
        s[0] += 32; // 00 1 00000
    
    if(c->s1BonusEaten)
    {
        s[0] += 16; // 000 1 0000
        s[i] = (c->s1BonusPositionX % 16)*16 + (c->s1BonusPositionY % 16);
        i++;
    }
    
    
    if(c->s2Direction == 0)    // Right
        s[0] += 4;// 0000 01 00
    else if(c->s2Direction == 1) // Up
        s[0] += 8+4;// 0000 11 00
    else if(c->s2Direction == 2) // Left
        s[0] += 0; // 0000 00 00
    else // Down
        s[0] += 8; // 0000 10 00
    
    if(c->s1Loss)
        s[0] += 2; // 000000 1 0
    
    if(c->s2BonusEaten)
    {
        s[0] += 1; // 0000000 1
        s[i] = (c->s2BonusPositionX % 16)*16 + (c->s2BonusPositionY % 16);
        i++;
    }
        
    s[i] = '\0';
    
    return s;
}

int ConnectionManager::deserialize(unsigned char* s)
{
	unsigned char c = s[0];
	
	if(c > 127)
	{
		c-=128;
		if(c>63)
			return 1; // ----- UP
		return 3; // --------- DOWN
	}
	else
	{
		if(c>63)
			return 0; // ----- RIGHT
		return 4; // --------- LEFT
	}
}
