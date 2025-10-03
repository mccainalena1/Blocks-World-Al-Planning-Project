#pragma once

#include <vector>
#include <algorithm>

#include "Data.h"
#include "Relation.h"

/*this file contains an object that represents the state as the world at any given point. 
This state class is the one with the search file uses to create, maintain, and update the world. 
A state is just to vectors of blocks representing each table position, and methods to maintain and update information about these two vectors.*/

class State
{
public:
	State();
	~State();

	vector<Relation>& getL1() { return l1; }
	vector<Relation>& getL2() { return l2; }

	vector<Relation> getL1Value() { return l1; }
	vector<Relation> getL2Value() { return l2; }

	vector<Relation> getL1Const() const { return l1; }
	vector<Relation> getL2Const() const { return l2; }

	int getL1Size() { return l1.size(); }
	int getL2Size() { return l2.size(); }

	int getL1Total() { return l1Total; }
	int getL2Total() { return l2Total; }

	int getL1Placed() { return l1Placed; }
	int getL2Placed() { return l2Placed; }

	Block getL1NextBlock() { return l1NextBlock; }
	Block getL2NextBlock() { return l2NextBlock; }

	int getL1BlocksOutPlace() { return l1Total - l1Placed; }
	int getL2BlocksOutPlace() { return l2Total - l2Placed; }

	Location getL1NextBlockLoc() { return l1NextBlockLoc; }
	Location getL2NextBlockLoc() { return l2NextBlockLoc; }

	void setL1(vector<Relation> nl1);
	void setL2(vector<Relation> nl2);

	int findL1ClearRelation();

	int findL2ClearRelation();

	int findL1TableRelation(Block b1);

	int findL1OnRelation(Block b1);

	vector<int> findL1AboveRelations(Block b1);

	int findL2TableRelation(Block b1);

	int findL2OnRelation(Block b1);

	vector<int> findL2AboveRelations(Block b1);

	bool incrementL1NumPlaced(State endState);

	void setL1NumPlaced(State endState);

	void setL1Total();

	void setL1NextBlock(State endState);

	void setL1NextBlockLoc();

	void incrementL1Total();

	void decrementL1Total();

	bool incrementL2NumPlaced(State endState);

	void setL2NumPlaced(State endState);

	void setL2Total();

	void setL2NextBlock(State endState);

	void setL2NextBlockLoc();

	void incrementL2Total();

	void decrementL2Total();

	bool operator==(const State& rhs) const;

	bool operator<(const State& rhs) const;

	bool operator()(const State & lhs, const State & rhs);


private:
	vector<Relation> l1; //A vector of relations describing L1
	vector<Relation> l2; //A vector of relations describing L2
	int l1Total; //The total number of blocks in the L1 position
	int l1Placed = 0; //The number of sorted blocks in the L1 position
	Block l1NextBlock; //The next block that needs to be found and sorted in L1
	Location l1NextBlockLoc; //The starting location of the next block that needs to be found and sorted in L1
	int l2Total; //The total number of blocks in the L2 position
	int l2Placed = 0; //The number of sorted blocks in the L2 position
	Block l2NextBlock; //The next block that needs to be found and sorted in L2
	Location l2NextBlockLoc; //The starting location of the next block that needs to be found and sorted in L2
};
