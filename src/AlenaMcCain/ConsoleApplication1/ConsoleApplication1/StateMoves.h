#pragma once

#include "Move.h"
#include "Data.h"
#include "Arm.h"
#include "State.h"
/*this is an object that contains information about the current state, 
the moves of the arms took to get to the current state, and the current state of the arms (i.e. holding block, location)*/

typedef vector<Move> Moves;

class StateMoves
{
public:
	StateMoves();
	StateMoves(State state, Moves moves1, Moves moves2, Arm arm1, Arm arm2);
	~StateMoves();

	State getStateConst() const { return state; }

	State& getState(){ return state; }

	Moves getMoves1() const { return moves1; }

	Moves getMoves2() const { return moves2; }

	Moves& getMoves1Ref() { return moves1; }

	Moves& getMoves2Ref() { return moves2; }

	Moves getMoves1Value() { return moves1; }

	Moves getMoves2Value() { return moves2; }

	Arm getArm1() const { return arm1; }

	Arm getArm2() const { return arm2; }

	void addMoves1(Move m);

	void addMoves2(Move m);

	int nextL1BlockAbove();

	int nextL2BlockAbove();

	bool operator==(const StateMoves& rhs) const;

private:
	State state;
	Moves moves1;
	Moves moves2;
	Arm arm1;
	Arm arm2;

};