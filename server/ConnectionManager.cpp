ConnectionManager::ConnectionManager(Server* server)
{
	this.server = server;
}

bool ConnectionManager::connReady()
{
	return this.IDs.size() == 2;//means it won't work for more than 2 either
}

void ConnectionManager::addConn(int ID, int snakeID)
{
	this.IDs.put(ID, snakeID);
}

void ConnectionManager::removeConn(int ID)
{
	for(auto i = this.IDs.begin(); i!= this.IDs.end();)
	{
		if(i->first == ID)
		{
			i = this.IDs.erase(i);
		}
		else
		{
			++i;
		}
	}
}

void ConnectionManager::send(string message, int clientID)
{
	this.server.wsSend(message, clientID);
}

void ConnectionManager::sendAll(string message)
{
	for(auto i : this.IDs)
	{
		this.server.wsSend(message, i->first);
	}
}

void ConnectionManager::sendIDs()
{
	for(auto a : this.IDs)
	{
		for(auto b : this.IDs)
		{
			if(b->first != a->first)
			{
				ostringstream os;
				os << b->second;
				this.server.wsSend(os.str(), a->first);
				os.str("");
			}
		}
	}
}