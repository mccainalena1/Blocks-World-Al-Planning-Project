#pragma once

/*
this class represents an arm object. 
An arm has a number designating which arm is being referred to, a block that it’s holding, and the current location of the arm. 
All of the action methods in this class are solely for purposes of drawing the graphical user interface. 
All of the work of determining and executing the moves that the arm will make is in search.h*/



#include <string>

#include "DrawState.h"

using namespace std;

class Arm
{
public:
	Arm();
	Arm(int num, Location loc);
	~Arm();

	int getNum() { return num; }
	Block getBlock() { return holding; }
	Location getLocation() { return location; }
	bool isEmpty() { return holding == Block(); }

	void setHolding(Block b);
	void setLocation(Location l);

	string pickUp(Location location);
	string putDown(Location location);
	string stack(Location location);
	string unstack(Location location);
	string move(Location locationEnd);
	string noop();

private:
	int num; //The number of the arm (1 or 2)
	Location location; //The location of an arm
	Block holding; //The block that an arm is currently holding
};
