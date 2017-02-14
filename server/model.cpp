#include "model.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>  

using namespace std;
Model::Model()
{;}

Model::Model(int boardWidth, int boardHeight)
{
	this->snakes = map<int, Snake>();	
	this->bonuses = vector<Tuple>();
	this->score = vector<int>();
	this->boardWidth  = boardWidth;
	this->boardHeight = boardHeight;
	this->bonuses.push_back(Tuple(6,5));
	this->bonuses.push_back(Tuple(5,6));
	this->score.push_back(0);
	this->score.push_back(0);
	//setting snakes requires the server to 
	//decide which client gets put at which place
	this->isRunning = false;
}

void Model::setScore(int ID, int score)
{
	this->scores[ID] = score;
}

/*vector<int> Model::getIDs()
{
	return this->IDs;
}*/

/*void Model::setIDs(vector<int> IDs)
{
	this->IDs = IDs;
}*/

void Model::addSnake(int ID, Snake snake)
{
	this->snakes[ID] = snake;
}

void Model::removeSnake(int ID)
{
	map<int, Snake> sMap = map<int, Snake>();
	map<int, Snake>::iterator it;
    for (it = this->snakes.begin(); it != this->snakes.end(); it++)
	{
		if(ID != it->first)
		{
			sMap[it->first] = it->second;
		}
	}
	this->snakes = sMap;
}

vector<Snake> Model::getSnakes()
{
	vector<Snake> sVect = vector<Snake>();
	map<int, Snake>::iterator it;
    for (it = this->snakes.begin(); it != this->snakes.end(); it++)
	{
		sVect.push_back(it->second);
	}
	return sVect;
}

Snake Model::getSnake(int ID)
{
	return this->snakes[ID];
}

int Model::getNumberSnakes()
{
	return this->snakes.size();
}

void Model::growSnake(int ID)
{
	Snake s = this->getSnake(ID);
	Tuple v = s.getHead().add(s.getDirection());
	s.addBody(v);
}

void Model::changeDirection(int ID, Tuple dir)
{
	Snake s = this->getSnake(ID);
	s.changeDirection(dir); 
}

vector<Tuple> Model::getBonuses()
{
	return this->bonuses;
}

bool objectInTheWay(Model m, int x, int y)
{
	vector<Snake> sVect =  m.getSnakes();
	for(int i = 0; i < sVect.size(); ++i)
	{
		vector<Tuple> body = sVect.at(i).getBody();
		for(int j = 0; j < body.size(); ++j)
		{
			if(body.at(j) == Tuple(x, y))
			{
				return true;
			}
		}
	}
	
	vector<Tuple> bVect = m.getBonuses();
	for(int i = 0; i < bVect.size(); ++i)
	{
		if(bVect.at(i) == Tuple(x,y))
		{
			return true;
		}
	}
	return false;
}
	
void Model::makeBonus(int bpos)
{
	srand(time(NULL));
	int x = floor(rand() * this->boardWidth);
	int y = floor(rand() * this->boardHeight);
	while(objectInTheWay(*this,x,y))
	{
		x = floor(rand() * this->boardWidth);
		y = floor(rand() * this->boardHeight);
	}
	this->bonuses.at(bpos) = Tuple(x,y);
}

int Model::getBoardWidth()
{
	return this->boardWidth;
}

int Model::getBoardHeight()
{
	return this->boardHeight;
}

void Model::setIsRunning(bool isRunning)
{
	this->isRunning = isRunning;
}
		
bool Model::getIsRunning()
{
	return this->isRunning;
}
	
void Model::mode()
{
	map<int, Snake>::iterator it;
	for(it = this->snakes.begin(); it!=this->snakes.end();++i)
	{
		it->second.move();
	}
}