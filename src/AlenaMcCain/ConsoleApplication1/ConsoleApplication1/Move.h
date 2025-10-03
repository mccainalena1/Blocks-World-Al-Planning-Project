#pragma once

#include "Data.h"

/*this class represents a move that an arm can take or will be taking. 
An arms movement consists of a location; where the action is taking place, an action; what 
the arm is doing, and a boolean; to keep track of which arm moved first*/
class Move
{
public:
	Move(Action action, Location location, bool first);
	~Move();

	Action getAction() { return action; }
	Location getLocation() { return location; }
	bool isFirst() { return first; }

private:
	Action action; //The action the arm is taking
	Location location; //The location that the action takes place
	bool first; //Whether this arm moved firsrt or second
};
