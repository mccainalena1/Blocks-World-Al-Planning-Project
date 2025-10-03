#include "pch.h"

#include <sstream>

#include "Draw.h"

/*this class represents an arm object. An arm has a number designating which arm is being referred to, a block that it’s holding, 
and the current location of the arm. All of the action methods in this class are solely for purposes of drawing the graphical user interface. 
All of the work of determining and executing the moves that the arm will make is in search.h*/

Arm::Arm()
{

}

Arm::Arm(int num, Location loc)
{
	this->holding = Block();
	this->num = num;
	this->location = loc;
}

Arm::~Arm()
{
}

/*sets the block object that arm is currently holding. If an arm is not holding a block, the block’s letter is represented by an empty string.*/
void Arm::setHolding(Block b)
{
	this->holding = b;
}

/*sets the current location of the arm (L1, L2)*/
void Arm::setLocation(Location l)
{
	this->location = l;
}

/*all of the following methods are the analogous graphical user interface methods 
to draw the blocks relative to the arms movements that are described in the search header file.*/
string Arm::pickUp(Location location)
{
	Block b;
	string l;
	if (location == Location::L1)
	{
		b = L1Vec.back();
		L1Vec.pop_back();
		this->holding = b;
		l = "L1";
	}
	else
	{
		b = L2Vec.back();
		L2Vec.pop_back();
		this->holding = b;
		l = "L2";
	}

	string str;
	std::ostringstream oss;
	oss << "PICK_UP(" << b.getLetter() << ", " << l << ")";
	str = oss.str();
	return str;
}

string Arm::putDown(Location location)
{
	Block b = this->getBlock();
	string l;
	if (location == Location::L1)
	{
		L1Vec.push_back(this->holding);
		this->holding = Block();
		l = "L1";
	}
	else
	{
		L2Vec.push_back(this->holding);
		this->holding = Block();
		l = "L2";
	} 

	string str;
	std::ostringstream oss;
	oss << "PUT_DOWN(" << b.getLetter() << ", " << l << ")";
	str = oss.str();
	return str;
}

string Arm::stack(Location location)
{
	Block b = this->getBlock();
	string l;
	if (location == Location::L1)
	{
		L1Vec.push_back(this->holding);
		this->holding = Block();
		l = "L1";
	}
	else
	{
		L2Vec.push_back(this->holding);
		this->holding = Block();
		l = "L2";
	}

	string str;
	std::ostringstream oss;
	oss << "STACK(" << b.getLetter() << ", " << l << ")";
	str = oss.str();
	return str;
}

string Arm::unstack(Location location)
{
	Block b;
	string l;
	if (location == Location::L1)
	{
		b = L1Vec.back();
		L1Vec.pop_back();
		this->holding = b;
		l = "L1";
	}
	else
	{
		b = L2Vec.back();
		L2Vec.pop_back();
		this->holding = b;
		l = "L2";
	}

	string str;
	std::ostringstream oss;
	oss << "UNSTACK(" << b.getLetter() << ", " << l << ")";
	str = oss.str();
	return str;
}

string Arm::move(Location locationEnd)
{
	string str;
	std::ostringstream oss;
	if (this->getNum() == 1)
	{
		if (locationEnd == L1)
		{
			arm1XStartPos = armL1XPos;
			arm1BlockXPos = arm1XStartPos + 45;
			arm1BlockTextXPos = arm1XStartPos + 60;
			oss << "MOVE(Arm " << this->getNum() << ", L2 , L1)";
		}
		else
		{
			arm1XStartPos = armL2XPos;
			arm1BlockXPos = arm1XStartPos + 45;
			arm1BlockTextXPos = arm1XStartPos + 60;
			oss << "MOVE(Arm " << this->getNum() << ", L1 , L2)";
		}
	}
	else
	{
		if (locationEnd == L1)
		{
			arm2XStartPos = armL1XPos;
			arm2BlockXPos = arm2XStartPos + 45;
			arm2BlockTextXPos = arm2XStartPos + 60;
			oss << "MOVE(Arm " << this->getNum() << ", L2 , L1)";
		}
		else
		{
			arm2XStartPos = armL2XPos;
			arm2BlockXPos = arm2XStartPos + 45;
			arm2BlockTextXPos = arm2XStartPos + 60;
			oss << "MOVE(Arm " << this->getNum() << ", L1 , L2)";
		}
	}
	str = oss.str();
	return str;
}

string Arm::noop()
{
	string str = "NOOP()";
	return str;
}