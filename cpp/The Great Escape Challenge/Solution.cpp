#include "Solution.h"
#include "WallInstaller.h"
#include "Player.h"
#include <iostream>

//========================== Solution ===============================
Solution::Solution(int w, int h, int id, int pc):
	width(w), height(h), myId(id), playerCount(pc),
	wallBuilder( new WallInstaller(w, h) )
{}
Solution::~Solution()
{
	delete this->wallBuilder;
}

void Solution::updatePlayer(int id, int x, int y, int wallsLeft)
{
	if (this->players.size() == id){
		this->players.push_back(new Player(id, this->width, this->height, 
			this->wallBuilder->getWalls()));
		cerr << "Player with id = " << id << " was created" << endl;
	}
	auto* player = this->players.at(id);
	player->update(x, y, wallsLeft);
}

void Solution::updateWall(int x, int y, string orientation)
{
	this->wallBuilder->addWall(x, y, orientation);
	cerr << orientation << "Wall( x = " << x << ", y = " << y << ") was updated" << endl;
}

string Solution::playerAction()
{
	this->calculatePath();
	this->getEmeny();

	auto* player = this->players.at(this->myId);	
	string action = "NONE";
	if (player->getId() != this->closeToFinish 
		&& player->hasWallsToSet())
	{
		cerr << "Need to build a wall for enemy "
			<< this->closeToFinish << " !!!" << endl;
		//copy all players positions to prevent changing
		auto players = new vector<Point>();
		for (auto i = 0; i < this->players.size(); ++i)
			players->push_back( Point( *this->players[i]->getPosition() ) );
		//get path
		auto* installer = static_cast<WallInstaller*>(this->wallBuilder);
		action = installer->buildFor(closeToFinish, &this->players);
	}
	if (action == "NONE")
		action = player->getNextStep();
	return action;
}

void Solution::calculatePath()
{
	for (auto i = 0; i < this->playerCount; ++i)
	{
		auto player = this->players.at(i);
		if (!player->isPlaying())continue;
		player->calculatePath();
//		cerr << endl;
	}
}

void Solution::getEmeny()
{
	this->closeToFinish = this->myId;
	auto* player = this->players.at(this->myId);
	if (player->pathLength() > 1 + this->myId)
	{
		//check only if path has angles	
		if (player->pathHasAngle())
			this->getCloser();
	} else
	{
		//need check last time befor last step
		this->getCloser();
	}
}

void Solution::getCloser()
{
	int mpl = INT_MAX;
	for (auto i = 0; i < this->players.size(); ++i)
	{
		auto* p = this->players.at(i);
		if (!p->isPlaying())continue;
		if (mpl > p->pathLength()){
			mpl = p->pathLength();
			this->closeToFinish = i;
		}
	}
}