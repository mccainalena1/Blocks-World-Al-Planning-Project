#include "pch.h"

#include "StateMovesRank.h"

/*this is an object that contains a state movement along with a ranking that tells the search
algorithm how good or bad a possible set of arm movements was to achieve a sorted blocks world.*/

StateMovesRank::StateMovesRank()
{

}

/*these objects and methods are used to distinguish which set of actions the arm should take to get closer to the end state. 
The constructor takes the current state after the arms of just completed their actions, the end state, the blocks from the previous state that are in the stack unsorted, the number blocks from the previous state that are above the block that is currently trying to be found, and those two parameters from the second to last state. The constructor sorts the relations in the current state and determines how many more blocks were placed in the state. It then determines the current rank of the world state relative to how many moves away the program is to the end state. 
Rankings are ordered from least to greatest, so that is why negative numbers are used.*/
StateMovesRank::StateMovesRank(StateMoves& stateMoves, State endState, int numPlaced, int numOutOfPlace, int numAbove, int prev1NumAbove, int prev1NumOutOfPlace, Location l)
{
	this->stateMoves = stateMoves;
	rank = 1;
	if (l == L1)
	{
		sort(this->getStateMovesRef().getState().getL1().begin(), this->getStateMovesRef().getState().getL1().end());
		this->getStateMovesRef().getState().setL1NumPlaced(endState);
		determineL1Rank(numPlaced, numOutOfPlace, numAbove, prev1NumAbove, prev1NumOutOfPlace);
	}
	else
	{
		sort(this->getStateMovesRef().getState().getL2().begin(), this->getStateMovesRef().getState().getL2().end());
		this->getStateMovesRef().getState().setL2NumPlaced(endState);
		determineL2Rank(numPlaced, numOutOfPlace, numAbove, prev1NumAbove, prev1NumOutOfPlace);
	}
}

StateMovesRank::StateMovesRank(StateMoves stateMoves, int rank)
{
	this->stateMoves = stateMoves;
	this->rank = rank;
}

StateMovesRank::~StateMovesRank()
{
	
}

void StateMovesRank::determineL1Rank(int numPlaced, int numOutOfPlace, int numAbove, int prev1NumAbove, int prev1NumOutOfPlace)
{
	if (moreL1BlocksPlaced(numPlaced)) //If more blocks are in place than the previous state
	{
		rank = -100 - this->getStateMoves().getState().getL1Placed();
	}
	else if (nextL1BlockCanBePlaced() && this->getStateMoves().getState().getL1BlocksOutPlace() == 0) //If an arm can put a block in place
	{
		rank = -99;
	}
	else if (nextL1BlockCanBePlaced())
	{
		Arm freeArm = Arm();
		if (this->getStateMoves().getArm1().getBlock() == this->getStateMoves().getState().getL1NextBlock()) //Determine which arm doesn't have the next block
			freeArm = this->getStateMoves().getArm2();
		else
			freeArm = this->getStateMoves().getArm1();
		int currentNumOutOfPlace = this->getStateMoves().getState().getL1BlocksOutPlace();

		if (currentNumOutOfPlace < numOutOfPlace)
		{
			if (!(freeArm.getBlock() == Block()) && freeArm.getLocation() == L1) //An arm has a block over L1
			{
				rank = -98 + currentNumOutOfPlace;
			}
		}
		else if (currentNumOutOfPlace < prev1NumOutOfPlace)
		{
			if (!(freeArm.getBlock() == Block()) && freeArm.getLocation() == L2) //An arm can put the extra block down on L2
			{
				rank = -98 + currentNumOutOfPlace;
			}
		}
		else
		{
			if (freeArm.getBlock() == Block() && freeArm.getLocation() == L1) //An arm can pick up an out of place block on L1
			{
				rank = -97 + currentNumOutOfPlace;
			}
			else if (freeArm.getBlock() == Block() && freeArm.getLocation() == L2) //Arm is empy and over L2
			{
				rank = -96 + currentNumOutOfPlace;
			}
		}
	}
	else if (IsArmHoldingNexL1Block()) // If an arm has the next block but is not over L1
	{
		rank = -80;
	}
	else //How close to the top the next block to be placed is
	{
		int blocksAbove = this->getStateMoves().nextL1BlockAbove();
		if (blocksAbove < numAbove)
		{
			if ((this->getStateMoves().getArm1().getLocation() == this->getStateMoves().getState().getL1NextBlockLoc() && !this->getStateMoves().getArm1().isEmpty()) &&
				(this->getStateMoves().getArm2().getLocation() == this->getStateMoves().getState().getL1NextBlockLoc() && !this->getStateMoves().getArm2().isEmpty()))
			{
				rank = -77 + blocksAbove;
			}
		}
		else if (blocksAbove < prev1NumAbove)
		{
			if ((this->getStateMoves().getArm1().getLocation() != this->getStateMoves().getState().getL1NextBlockLoc() && !this->getStateMoves().getArm1().isEmpty()) &&
				(this->getStateMoves().getArm2().getLocation() != this->getStateMoves().getState().getL1NextBlockLoc() && !this->getStateMoves().getArm2().isEmpty()))
			{
				rank = -77 + blocksAbove;
			}
		}
		else
		{
			if ((this->getStateMoves().getArm1().getLocation() == this->getStateMoves().getState().getL1NextBlockLoc() && this->getStateMoves().getArm1().isEmpty()) &&
				(this->getStateMoves().getArm2().getLocation() == this->getStateMoves().getState().getL1NextBlockLoc() && this->getStateMoves().getArm2().isEmpty()))
			{
				rank = -76 + blocksAbove;
			}
			else if ((this->getStateMoves().getArm1().getLocation() != this->getStateMoves().getState().getL1NextBlockLoc() && this->getStateMoves().getArm1().isEmpty()) &&
				(this->getStateMoves().getArm2().getLocation() != this->getStateMoves().getState().getL1NextBlockLoc() && this->getStateMoves().getArm2().isEmpty()))
			{
				rank = -75 + blocksAbove;
			}
		}
	}
}

void StateMovesRank::determineL2Rank(int numPlaced, int numOutOfPlace, int numAbove, int prev1NumAbove, int prev1NumOutOfPlace)
{
	if (moreL2BlocksPlaced(numPlaced)) //If more blocks are in place than the previous state
	{
		rank = -100 - this->getStateMoves().getState().getL2Placed();
	}
	else if (nextL2BlockCanBePlaced() && this->getStateMoves().getState().getL2BlocksOutPlace() == 0) //If an arm can put a block in place
	{
		rank = -99;
	}
	else if (nextL2BlockCanBePlaced())
	{
		Arm freeArm = Arm(); //Determine which arm doesn't have the next block
		if (this->getStateMoves().getArm1().getBlock() == this->getStateMoves().getState().getL2NextBlock())
			freeArm = this->getStateMoves().getArm2();
		else
			freeArm = this->getStateMoves().getArm1();
		int currentNumOutOfPlace = this->getStateMoves().getState().getL2BlocksOutPlace();

		if (currentNumOutOfPlace < numOutOfPlace)
		{
			if (!(freeArm.getBlock() == Block()) && freeArm.getLocation() == L2) //An arm has a block over L2
			{
				rank = -98 + currentNumOutOfPlace;
			}
		}
		else if (currentNumOutOfPlace < prev1NumOutOfPlace)
		{
			if (!(freeArm.getBlock() == Block()) && freeArm.getLocation() == L1) //An arm can put the extra block down on L1
			{
				rank = -98 + currentNumOutOfPlace;
			}
		}
		else
		{
			if (freeArm.getBlock() == Block() && freeArm.getLocation() == L2) //An arm can pick up an out of place block on L2
			{
				rank = -97 + currentNumOutOfPlace;
			}
			else if (freeArm.getBlock() == Block() && freeArm.getLocation() == L1) //Arm is empy and over L1
			{
				rank = -96 + currentNumOutOfPlace;
			}
		}
	}
	else if (IsArmHoldingNexL2Block()) // If an arm has the next block but is not over L2
	{
		rank = -80;
	}
	else //How close to the top the next block to be placed is
	{
		int blocksAbove = this->getStateMoves().nextL2BlockAbove();
		if (blocksAbove < numAbove)
		{
			if ((this->getStateMoves().getArm1().getLocation() == this->getStateMoves().getState().getL2NextBlockLoc() && !this->getStateMoves().getArm1().isEmpty()) &&
				(this->getStateMoves().getArm2().getLocation() == this->getStateMoves().getState().getL2NextBlockLoc() && !this->getStateMoves().getArm2().isEmpty()))
			{
				rank = -77 + blocksAbove;
			}
		}
		else if (blocksAbove < prev1NumAbove)
		{
			if ((this->getStateMoves().getArm1().getLocation() != this->getStateMoves().getState().getL2NextBlockLoc() && !this->getStateMoves().getArm1().isEmpty()) &&
				(this->getStateMoves().getArm2().getLocation() != this->getStateMoves().getState().getL2NextBlockLoc() && !this->getStateMoves().getArm2().isEmpty()))
			{
				rank = -77 + blocksAbove;
			}
		}
		else
		{
			if ((this->getStateMoves().getArm1().getLocation() == this->getStateMoves().getState().getL2NextBlockLoc() && this->getStateMoves().getArm1().isEmpty()) &&
				(this->getStateMoves().getArm2().getLocation() == this->getStateMoves().getState().getL2NextBlockLoc() && this->getStateMoves().getArm2().isEmpty()))
			{
				rank = -76 + blocksAbove;
			}
			else if ((this->getStateMoves().getArm1().getLocation() != this->getStateMoves().getState().getL2NextBlockLoc() && this->getStateMoves().getArm1().isEmpty()) &&
				(this->getStateMoves().getArm2().getLocation() != this->getStateMoves().getState().getL2NextBlockLoc() && this->getStateMoves().getArm2().isEmpty()))
			{
				rank = -75 + blocksAbove;
			}
		}
	}

}

/*this method returns true if there are more blocks placed in order in the current state than in the previous state*/
bool StateMovesRank::moreL1BlocksPlaced(int prev) 
{
	return this->getStateMovesRef().getState().getL1Placed() > prev;
}

/*this method returns true if there are no extra blocks on top of the current blocks that are sorted and the arm is 
holding the next block to be sorted over the proper location.*/
bool StateMovesRank::nextL1BlockCanBePlaced()
{
	if (this->getStateMovesRef().getState().getL1NextBlock() == Block())
		return false;
	return (this->getStateMovesRef().getArm1().getBlock() == this->getStateMovesRef().getState().getL1NextBlock() && this->getStateMovesRef().getArm1().getLocation() == L1) ||
		(this->getStateMovesRef().getArm2().getBlock() == this->getStateMovesRef().getState().getL1NextBlock() && this->getStateMovesRef().getArm2().getLocation() == L1);
}

/*this method returns true if the block is holding the next block to be sorted.*/
bool StateMovesRank::IsArmHoldingNexL1Block()
{
	if (this->getStateMovesRef().getState().getL1NextBlock() == Block())
		return false;
	return this->getStateMovesRef().getArm1().getBlock() == this->getStateMovesRef().getState().getL1NextBlock() || 
		this->getStateMovesRef().getArm2().getBlock() == this->getStateMovesRef().getState().getL1NextBlock();
}

/*this method returns true if there are more blocks placed in order in the current state than in the previous state*/
bool StateMovesRank::moreL2BlocksPlaced(int prev)
{
	return this->getStateMovesRef().getState().getL2Placed() > prev;
}

/*this method returns true if there are no extra blocks on top of the current blocks that are sorted and the arm is
holding the next block to be sorted over the proper location.*/
bool StateMovesRank::nextL2BlockCanBePlaced()
{
	if (this->getStateMovesRef().getState().getL2NextBlock() == Block())
		return false;
	return (this->getStateMovesRef().getArm1().getBlock() == this->getStateMovesRef().getState().getL2NextBlock() && this->getStateMovesRef().getArm1().getLocation() == L2) ||
		(this->getStateMovesRef().getArm2().getBlock() == this->getStateMovesRef().getState().getL2NextBlock() && this->getStateMovesRef().getArm2().getLocation() == L2);
}

/*this method returns true if the block is holding the next block to be sorted.*/
bool StateMovesRank::IsArmHoldingNexL2Block()
{
	if (this->getStateMovesRef().getState().getL2NextBlock() == Block())
		return false;
	return this->getStateMovesRef().getArm1().getBlock() == this->getStateMovesRef().getState().getL2NextBlock() ||
		this->getStateMovesRef().getArm2().getBlock() == this->getStateMovesRef().getState().getL2NextBlock();
}

/*two state move rank objects are considered equal if there states are equal. */
bool StateMovesRank::operator==(const StateMovesRank& rhs) const
{
	return this->stateMoves.getStateConst() == rhs.getStateMoves().getState();
}

/*this is used when sorting the states ranks in order to determine which of the next possible states is best. States are ordered by their rank.*/
bool StateMovesRank::operator<(const StateMovesRank& rhs) const
{
	return this->rank < rhs.getRank();
}

bool StateMovesRank::operator()(const StateMovesRank &lhs, const StateMovesRank &rhs) const
{
	return lhs.getRank() < rhs.getRank();
}