using namespace std;

class model //change direction to a two vector
{
	map<int, Pair<int, Pair<int,int>>> snakes;
	map<int, vector<Pair<x,y>>> snakeBodies;//don't think this is necessary
	
	public:
		void generateSnakes()//idk if this is necessary
		{
			for(auto entry : snakes)
			{
				giveOppo(entry.second().first());
			}
		}
}