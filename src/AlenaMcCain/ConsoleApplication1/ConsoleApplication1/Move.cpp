#include "pch.h"

#include "Move.h"
/*this class represents a move that an arm can take or will be taking. 
And arms movement consists of a location; where the action is taking place, an action; 
what arm is doing, and a boolean; to keep track of which arm moved first*/

Move::Move(Action action, Location location, bool first)
{
	this->action = action;
	this->location = location;
	this->first = first;
}

Move::~Move()
{
}