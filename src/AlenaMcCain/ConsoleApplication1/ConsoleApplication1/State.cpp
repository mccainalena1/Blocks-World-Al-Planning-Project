#include "pch.h"

#include "State.h"

using namespace std;

/*this file contains an object that represents the state as the world at any given point. 
This state class is the one with the search file uses to create, maintain, and update the world. 
A state is just to vectors of blocks representing each table position, and methods to maintain and update information about these two vectors.*/

State::State()
{
	l1 = {};
	l2 = {};
}

State::~State()
{
}

void State::setL1(vector<Relation> nl1)
{
	l1 = nl1;
}

void State::setL2(vector<Relation> nl2)
{
	l2 = nl2;
}

/*this method returns the index of the relation clear in L1 or L2. This is useful to determine if the vectors are empty 
because of there is no clear relation found the methods return -1. 
This is also used when updating the list of relations when an arm action is being completed.*/
int State::findL1ClearRelation()
{
	for (int i = 0; i < l1.size(); i++)
	{
		if (l1[i].getName() == CLEAR)
		{
			return i;
		}
	}
	return -1;
}

/*this method takes the block that was found by using the index returned in the clear relation method to determine if that block is on the table.
This is also used when updating the list of relations when an arm action is being completed.*/
int State::findL1TableRelation(Block b1)
{
	for (int i = 0; i < l1.size(); i++)
	{
		if (l1[i].getName() == TABLE && l1[i].getFirstBlock().getLetter() == b1.getLetter())
		{
			return i;
		}
	}
	return -1;
}

/*this method takes the block that was found by using the index returned in the clear relation method to determine if that block is on top of another block. 
If that is not the case the methods return -1 and you now know that only one block is in the list. 
This is also used when updating the list of relations when an arm action is being completed.*/
int State::findL1OnRelation(Block b1)
{
	for (int i = 0; i < l1.size(); i++)
	{
		if (l1[i].getName() == ON && l1[i].getFirstBlock().getLetter() == b1.getLetter())
		{
			return i;
		}
	}
	return -1;
}

/*this method takes the block that was found by using the index returned in the clear relation method to determine if that block is above any other blocks. 
This will return the list of indices instead of a single index because a block could be above multiple blocks. 
This is also used when updating the list of relations when an arm action is being completed.*/
vector<int> State::findL1AboveRelations(Block b1)
{
	vector<int> v;
	for (int i = 0; i < l1.size(); i++)
	{
		if (l1[i].getName() == ABOVE && l1[i].getFirstBlock().getLetter() == b1.getLetter())
		{
			v.push_back(i);
		}
	}
	if (v.size() == 0)
	{
		return { -1 };
	}
	return v;
}

/*this method returns the index of the relation clear in L1 or L2. This is useful to determine if the vectors are empty
because of there is no clear relation found the methods return -1.
This is also used when updating the list of relations when an arm action is being completed.*/
int State::findL2ClearRelation()
{
	for (int i = 0; i < l2.size(); i++)
	{
		if (l2[i].getName() == CLEAR)
		{
			return i;
		}
	}
	return -1;
}

/*this method takes the block that was found by using the index returned in the clear relation method to determine if that block is on the table.
This is also used when updating the list of relations when an arm action is being completed.*/
int State::findL2TableRelation(Block b1)
{
	for (int i = 0; i < l2.size(); i++)
	{
		if (l2[i].getName() == TABLE && l2[i].getFirstBlock().getLetter() == b1.getLetter())
		{
			return i;
		}
	}
	return -1;
}

/*this method takes the block that was found by using the index returned in the clear relation method to determine if that block is on top of another block.
If that is not the case the methods return -1 and you now know that only one block is in the list.
This is also used when updating the list of relations when an arm action is being completed.*/
int State::findL2OnRelation(Block b1)
{
	for (int i = 0; i < l2.size(); i++)
	{
		if (l2[i].getName() == ON && l2[i].getFirstBlock().getLetter() == b1.getLetter())
		{
			return i;
		}
	}
	return -1;
}

/*this method takes the block that was found by using the index returned in the clear relation method to determine if that block is above any other blocks.
This will return the list of indices instead of a single index because a block could be above multiple blocks.
This is also used when updating the list of relations when an arm action is being completed.*/
vector<int> State::findL2AboveRelations(Block b1)
{
	vector<int> v;
	for (int i = 0; i < l2.size(); i++)
	{
		if (l2[i].getName() == ABOVE && l2[i].getFirstBlock().getLetter() == b1.getLetter())
		{
			v.push_back(i);
		}
	}
	if (v.size() == 0)
	{
		return { -1 };
	}
	return v;
}

/*this method takes the end state and determines how many blocks are currently in the correct order from bottom to top on the table location. 
Given the current number of blocks that are already in place, the method determines if the next block in the sequence is in place and ordered correctly.*/
bool State::incrementL1NumPlaced(State endState)
{
	if (l1Placed < endState.getL1Size() && l1Placed < l1.size() && (l1[l1Placed] == endState.getL1()[l1Placed]) && l1[l1Placed].getName() != ABOVE && l1[l1Placed].getName() != CLEAR)
	{
		++l1Placed;
		return true;
	}
	return false;
}

/*using increment num placed. This will determine the total number blocks that are currently in the correct order in a stack.*/
void State::setL1NumPlaced(State endState)
{
	sort(this->getL1().begin(), this->getL1().end());
	bool incremented = incrementL1NumPlaced(endState);
	while (incremented)
	{
		incremented = incrementL1NumPlaced(endState);
	}
}

/*this method determines how many blocks are in a given stack based on the number of table and on relations in the list.*/
void State::setL1Total()
{
	int total = 0;
	for (int i = 0; i < l1.size(); i++)
	{
		if (l1[i].getName() == TABLE || l1[i].getName() == ON)
			++total;
	}
	l1Total = total;
}

/*this method determines the next block but the algorithm needs to search for to be placed in a given stack based on how many 
blocks are already ordered in the stack.*/
void State::setL1NextBlock(State endState)
{
	if (l1Placed < endState.getL1Size())
		l1NextBlock = endState.getL1()[l1Placed].getFirstBlock();
}

/*given the next block, this algorithm determines which stack the next block to be ordered is currently found in.*/
void State::setL1NextBlockLoc()
{
	Block b = Block();
	b = this->getL1NextBlock();
	vector<Relation>::iterator it = find_if(this->getL1().begin(), this->getL1().end(),
		[b](Relation const& r) {return (b == r.getFirstBlock() && r.getName() == ON) || (b == r.getFirstBlock() && r.getName() == TABLE); });
	if (b == Block())
		this->l1NextBlockLoc = L1;
	else if (it != this->getL1().end())
		this->l1NextBlockLoc = L1;
	else
		this->l1NextBlockLoc = L2;
}

/*this method simply increments the total number blocks in the stack. This is very useful for when an arm has completed a put down or stack action.*/
void State::incrementL1Total()
{
	++l1Total;
}

/*this method simply decrements the number blocks in the stack. This is very useful when an arm has completed a pick up or unstack action.*/
void State::decrementL1Total()
{
	--l1Total;
}

/*this method takes the end state and determines how many blocks are currently in the correct order from bottom to top on the table location.
Given the current number of blocks that are already in place, the method determines if the next block in the sequence is in place and ordered correctly.*/
bool State::incrementL2NumPlaced(State endState)
{
	if (l2Placed < endState.getL2Size() && l2Placed < l2.size() && (l2[l2Placed] == endState.getL2()[l2Placed]) && l2[l2Placed].getName() != ABOVE && l2[l2Placed].getName() != CLEAR)
	{
		++l2Placed;
		return true;
	}
	return false;
}

/*using increment num placed. This will determine the total number blocks that are currently in the correct order in a stack.*/
void State::setL2NumPlaced(State endState)
{
	sort(this->getL2().begin(), this->getL2().end());
	bool incremented = incrementL2NumPlaced(endState);
	while (incremented)
	{
		incremented = incrementL2NumPlaced(endState);
	}
}

/*this method determines how many blocks are in a given stack based on the number of table and on relations in the list.*/
void State::setL2Total()
{
	int total = 0;
	for (int i = 0; i < l2.size(); i++)
	{
		if (l2[i].getName() == TABLE || l2[i].getName() == ON)
			++total;
	}
	l2Total = total;
}

/*this method determines the next block but the algorithm needs to search for to be placed in a given stack based on how many
blocks are already ordered in the stack.*/
void State::setL2NextBlock(State endState)
{
	if (l2Placed < endState.getL2Size())
		l2NextBlock = endState.getL2()[l2Placed].getFirstBlock();
}

/*given the next block, this algorithm determines which stack the next block to be ordered is currently found in.*/
void State::setL2NextBlockLoc()
{
	Block b = Block();
	b = this->getL2NextBlock();
	vector<Relation>::iterator it = find_if(this->getL1().begin(), this->getL1().end(),
		[b](Relation const& r) {return (b == r.getFirstBlock() && r.getName() == ON) || (b == r.getFirstBlock() && r.getName() == TABLE); });
	if (it != this->getL1().end())
		this->l1NextBlockLoc = L1;
	else
		this->l1NextBlockLoc = L2;
}

/*this method simply increments the total number blocks in the stack. This is very useful for when an arm has completed a put down or stack action.*/
void State::incrementL2Total()
{
	++l2Total;
}

/*this method simply decrements the number blocks in the stack. This is very useful when an arm has completed a pick up or unstack action.*/
void State::decrementL2Total()
{
	--l2Total;
}

/*this method determines if states are equal if their relations are equal. The relations in the states must be ordered.*/
bool State::operator==(const State& rhs) const
{
	return (this->l1 == rhs.l1) && (this->l2 == rhs.l2);
}

/*these methods ordered the states based on their relations.*/
bool State::operator<(const State& rhs) const
{
	if (this->l1 == rhs.l1)
		return this->l2 < rhs.l2;
	else
		return this->l1 < rhs.l1;
}

bool State::operator()(const State & lhs, const State & rhs)
{
	if (lhs.l1 == rhs.l1)
		return lhs.l2 < rhs.l2;
	else
		return lhs.l1 < rhs.l1;
}