#pragma once

#include "StateMoves.h"

using namespace std;
/*this is an object that contains a state movement along with a ranking that tells the search 
algorithm how good or bad a possible set of arm movements was to achieve a sorted blocks world.*/

class StateMovesRank
{
public:
	StateMovesRank();
	StateMovesRank(StateMoves& stateMoves, State endState, int numPlaced, int numOutOfPlace, int numAbove, int prev1NumAbove, int prev1NumOutOfPlace, Location l);
	StateMovesRank(StateMoves stateMoves, int rank);
	~StateMovesRank();

	int getRank() const { return rank; }

	StateMoves getStateMoves() const { return stateMoves; }

	StateMoves& getStateMovesRef() { return stateMoves; }

	void determineL1Rank(int numPlaced, int numOutOfPlace, int numAbove, int prev1NumAbove, int prev1NumOutOfPlace);

	void determineL2Rank(int numPlaced, int numOutOfPlace, int numAbove, int prev1NumAbove, int prev1NumOutOfPlace);

	bool operator==(const StateMovesRank & rhs) const;

	bool operator<(const StateMovesRank& rhs) const;

	bool operator()(const StateMovesRank & lhs, const StateMovesRank & rhs) const;
private:
	bool moreL1BlocksPlaced(int prev);

	bool nextL1BlockCanBePlaced();

	bool IsArmHoldingNexL1Block();

	bool moreL2BlocksPlaced(int prev);

	bool nextL2BlockCanBePlaced();

	bool IsArmHoldingNexL2Block();

	StateMoves stateMoves;
	int rank;
};
