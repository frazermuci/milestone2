ConnectionManager::ConnectionManager(Server* server)
{
	this.server = server;
}

bool ConnectionManager::connReady()
{
	return this.IDs.size() == 2;
}

void ConnectionManager::addConn(int ID)
{
	this.IDs.push_back(ID);
}

void ConnectionManager::removeConn(int ID)
{
	for(auto i = this.IDs.begin(); i!= this.IDs.end();)
	{
		if((*i) == ID)
		{
			i = this.IDs.erase(i);
		}
		else
		{
			++i;
		}
	}
}

void ConnectionManager::sendState()
{
	string stringObj = this.serialize();
	for(auto i : this.IDs)
	{
		this.server.wsSend(stringObj, (*i));
	}
}