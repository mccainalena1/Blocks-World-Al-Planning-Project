#include "pch.h"

#include "StateMoves.h"

/*this is an object that contains information about the current state,
the moves of the arms took to get to the current state, and the current state of the arms (i.e. holding block, location)*/

StateMoves::StateMoves()
{

}

StateMoves::StateMoves(State state, Moves moves1, Moves moves2, Arm arm1, Arm arm2)
{
	this->state = state;
	this->moves1 = moves1;
	this->moves2 = moves2;
	this->arm1 = arm1;
	this->arm2 = arm2;
}

StateMoves::~StateMoves()
{
}

/*this method allows you to add the current move that arm one or two made to the current state. 
This is useful for tracking the moves that the arms and made over time.*/
void StateMoves::addMoves1(Move m)
{
	moves1.push_back(m);
}

/*this method allows you to add the current move that arm one or two made to the current state.
This is useful for tracking the moves that the arms and made over time.*/
void StateMoves::addMoves2(Move m)
{
	moves2.push_back(m);
}

/*two state moves are equal if there states are equal.*/
bool StateMoves::operator==(const StateMoves& rhs) const
{
	return this->state == rhs.getStateConst();
}

/*this method determines how many blocks are above the current block that the arms are trying to find to sort. 
This is useful work when ranking states because the less blocks that are above the block that is trying to be found, 
the closer the program is to the end state.*/
int StateMoves::nextL1BlockAbove()
{
	Block b = getState().getL1NextBlock();
	if (b == Block())
		return this->getState().getL1Total();
	else if (getState().getL1NextBlockLoc() == L1)
	{
		int mycount = count_if(getState().getL1().begin(), getState().getL1().end(),
			[b](Relation const& r) {return b == r.getSecondBlock() && r.getName() == ABOVE; });
		return mycount;
	}
	else
	{
		int mycount = count_if(getState().getL2().begin(), getState().getL2().end(),
			[b](Relation const& r) {return b == r.getSecondBlock() && r.getName() == ABOVE; });
		return mycount;
	}
}

/*this method determines how many blocks are above the current block that the arms are trying to find to sort.
This is useful work when ranking states because the less blocks that are above the block that is trying to be found,
the closer the program is to the end state.*/
int StateMoves::nextL2BlockAbove()
{
	Block b = getState().getL2NextBlock();
	if (b == Block())
		return this->getState().getL2Total();
	if (getState().getL2NextBlockLoc() == L1)
	{
		int mycount = count_if(getState().getL1().begin(), getState().getL1().end(),
			[b](Relation const& r) {return b == r.getSecondBlock() && r.getName() == ABOVE; });
		return mycount;
	}
	else
	{
		int mycount = count_if(getState().getL2().begin(), this->getState().getL2().end(),
			[b](Relation const& r) {return b == r.getSecondBlock() && r.getName() == ABOVE; });
		return mycount;
	}
}