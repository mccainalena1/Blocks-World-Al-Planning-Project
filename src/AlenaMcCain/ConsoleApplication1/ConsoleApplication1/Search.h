#pragma once

#include <vector>
#include <set>
#include <tuple>
#include <algorithm>

#include "StateMovesRank.h"


using namespace std;


typedef tuple<Arm, Move> StateAction; //Used to keep track of moves in a state

/*this method is responsible for updating the relevant search space for the algorithm every time an arm completes an action/move. This allows the algorithm to 
determine quickly which blocks are on the top of the stacks and update that information when future actions are completed. See state.cpp for more details*/
void updateIndicies( int l, State s, int& b1ClearIndex, int& b2ClearIndex, int& b1OnIndex, int& b2OnIndex, int& b1TableIndex, int& b2TableIndex, vector<int>& b1AboveIndicies, vector<int>& b2AboveIndicies)
{
	if (l == L1)
	{
		//B1
		b1ClearIndex = s.findL1ClearRelation();
		if (b1ClearIndex != -1)
		{
			Block b1 = s.getL1()[b1ClearIndex].getFirstBlock();
			b1OnIndex = s.findL1OnRelation(b1);
			if (b1OnIndex == -1)
			{
				b1TableIndex = s.findL1TableRelation(b1);
				b1AboveIndicies = { -1 };
			}
			else
			{
				b1TableIndex = -1;
				b1AboveIndicies = s.findL1AboveRelations(b1);
			}
		}
	}
	else
	{
		//B2
		b2ClearIndex = s.findL2ClearRelation();
		if (b2ClearIndex != -1)
		{
			Block b1 = s.getL2()[b2ClearIndex].getFirstBlock();
			b2OnIndex = s.findL2OnRelation(b1);
			if (b2OnIndex == -1)
			{
				b2TableIndex = s.findL2TableRelation(b1);
				b2AboveIndicies = { -1 };
			}
			else
			{
				b2TableIndex = -1;
				b2AboveIndicies = s.findL2AboveRelations(b1);
			}
		}
	}
}

/*this method returns a current state of the world after an arm completes an NOOP operation.*/
tuple<State, Move, Arm> updateStateNOOP(State state, Arm arm, bool first)
{
	return make_tuple(state, Move(NOOP, L1, first), arm);
}

/*this method returns a current state of the world after an arm completes a PICK_UP operation.*/
tuple<State, Move, Arm> updateStatePICK_UP(State state, int locationIndex, int clearRelationIndex, int tableRelationIndex, Arm arm, bool first)
{
	if (locationIndex == 0) //If the arm is over L1
	{
		arm.setHolding(state.getL1()[tableRelationIndex].getFirstBlock());
		if (clearRelationIndex < tableRelationIndex)
		{
			state.getL1().erase(state.getL1().begin() + clearRelationIndex);
			state.getL1().erase(state.getL1().begin() + (tableRelationIndex - 1));
		}
		else
		{
			state.getL1().erase(state.getL1().begin() + tableRelationIndex);
			state.getL1().erase(state.getL1().begin() + (clearRelationIndex - 1));
		}
		state.getL1().push_back(Relation(EMPTY, Block(), Block()));
		state.decrementL1Total();
		return make_tuple(state, Move(PICK_UP, L1, first), arm);
	}
	else //If the arm is over L2
	{
		arm.setHolding(state.getL2()[tableRelationIndex].getFirstBlock());
		if (clearRelationIndex < tableRelationIndex)
		{
			state.getL2().erase(state.getL2().begin() + clearRelationIndex);
			state.getL2().erase(state.getL2().begin() + (tableRelationIndex - 1));
		}
		else
		{
			state.getL2().erase(state.getL2().begin() + tableRelationIndex);
			state.getL2().erase(state.getL2().begin() + (clearRelationIndex - 1));
		}
		state.getL2().push_back(Relation(EMPTY, Block(), Block()));
		state.decrementL2Total();
		return make_tuple(state, Move(PICK_UP, L2, first), arm);
	}
}

/*this method returns a current state of the world after an arm completes a PUT_DOWN operation.*/
tuple<State, Move, Arm> updateStatePUT_DOWN(State state, int locationIndex, Arm arm, bool first)
{
	if (locationIndex == 0) //If the arm is over L1
	{
		state.getL1().clear();
		state.getL1().push_back(Relation(TABLE, arm.getBlock(), Block()));
		state.getL1().push_back(Relation(CLEAR, arm.getBlock(), Block()));
		arm.setHolding(Block(""));
		state.incrementL1Total();
		return make_tuple(state, Move(PUT_DOWN, L1, first), arm);
	}
	else //If the arm is over L2
	{
		state.getL2().clear();
		state.getL2().push_back(Relation(TABLE, arm.getBlock(), Block()));
		state.getL2().push_back(Relation(CLEAR, arm.getBlock(), Block()));
		arm.setHolding(Block(""));
		state.incrementL2Total();
		return make_tuple(state, Move(PUT_DOWN, L2, first), arm);
	}
}

/*this method returns a current state of the world after an arm completes an UNSTACK operation.*/
tuple<State, Move, Arm> updateStateUNSTACK(State state, int locationIndex, int onRelationIndex, vector<int> aboveRelationIndicies, int clearIndex, Arm arm, bool first)
{
	if (locationIndex == 0) //If the arm is over L1
	{
		Relation o = state.getL1()[onRelationIndex];
		Block newClearBlock = state.getL1()[onRelationIndex].getSecondBlock();
		state.getL1()[clearIndex].setFirstBlock(newClearBlock);
		arm.setHolding(state.getL1()[onRelationIndex].getFirstBlock());
		for (int i = 0; i < aboveRelationIndicies.size(); i++)
		{
			state.getL1().erase(state.getL1().begin() + (aboveRelationIndicies[i] - i));
		}
		auto i = std::find(begin(state.getL1()), end(state.getL1()), o);
		state.getL1().erase(i);
		state.decrementL1Total();
		return make_tuple(state, Move(UNSTACK, L1, first), arm);
	}
	else //If the arm is over L2
	{
		Relation o = state.getL2()[onRelationIndex];
		Block newClearBlock = state.getL2()[onRelationIndex].getSecondBlock();
		state.getL2()[clearIndex].setFirstBlock(newClearBlock);
		arm.setHolding(state.getL2()[onRelationIndex].getFirstBlock());
		for (int i = 0; i < aboveRelationIndicies.size(); i++)
		{
			state.getL2().erase(state.getL2().begin() + (aboveRelationIndicies[i] - i));
		}
		auto i = std::find(begin(state.getL2()), end(state.getL2()), o);
		state.getL2().erase(i);
		state.decrementL2Total();
		return make_tuple(state, Move(UNSTACK, L2, first), arm);
	}
}

/*this method returns a current state of the world after an arm completes a STACK operation*/
tuple<State, Move, Arm> updateStateSTACK(State state, int locationIndex, int clearRelationIndex, Arm arm, bool first)
{
	if (locationIndex == 0) //If the arm is over L1
	{
		Block startClear = state.getL1()[clearRelationIndex].getFirstBlock();
		state.getL1()[clearRelationIndex].setFirstBlock(arm.getBlock());
		for (int j = 0; j < state.getL1Size(); ++j)
		{
			if (state.getL1()[j].getName() == TABLE || state.getL1()[j].getName() == ON)
			{
				state.getL1().push_back(Relation(ABOVE, arm.getBlock(), state.getL1()[j].getFirstBlock()));
			}
		}
		state.getL1().push_back(Relation(ON, arm.getBlock(), startClear));
		arm.setHolding(Block());
		state.incrementL1Total();
		return make_tuple(state, Move(STACK, L1, first), arm);
	}
	else //If the arm is over L2
	{
		Block startClear = state.getL2()[clearRelationIndex].getFirstBlock();
		state.getL2()[clearRelationIndex].setFirstBlock(arm.getBlock());
		for (int j = 0; j < state.getL2Size(); ++j)
		{
			if (state.getL2()[j].getName() == TABLE || state.getL2()[j].getName() == ON)
			{
				state.getL2().push_back(Relation(ABOVE, arm.getBlock(), state.getL2()[j].getFirstBlock()));
			}
		}
		state.getL2().push_back(Relation(ON, arm.getBlock(), startClear));
		arm.setHolding(Block());
		state.incrementL2Total();
		return make_tuple(state, Move(STACK, L2, first), arm);
	}
}

/*this method returns a current state of the world after an arm completes a MOVE operation.*/
tuple<State, Move, Arm> updateStateMOVE(State state, Arm arm, bool first)
{
	if (arm.getLocation() == L1)
		arm.setLocation(L2);
	else
		arm.setLocation(L1);
	return make_tuple(state, Move(MOVE, arm.getLocation(), first), arm);
}

/*this method given the current state, and the actions that the arms have taken produces new state using the action methods listed above.*/
StateMoves getNewState(StateMoves sm, int b1ClearIndex, int b2ClearIndex, int b1OnIndex, int b2OnIndex, int b1TableIndex, int b2TableIndex, vector<int> b1AboveIndicies, vector<int> b2AboveIndicies, vector<StateAction> stateActions)
{
	vector<State> s;
	s.push_back(sm.getState());
	Moves m;
	vector<Arm> arms;
	int i = 0;
	//Placeholder
	tuple<State, Move, Arm> sma = make_tuple(sm.getState(), Move(NOOP, L1, false), Arm(0, L1));
	for (StateAction sa : stateActions)
	{
		//Get statistics about an arm's location
		Arm a = get<0>(sa);
		int loc = get<1>(sa).getLocation();
		int clear;
		int on;
		int table;
		vector<int> above;
		bool first = get<1>(sa).isFirst();
		if (loc == 0)
		{
			clear = b1ClearIndex;
			on = b1OnIndex;
			table = b1TableIndex;
			above = b1AboveIndicies;
		}
		else
		{
			clear = b2ClearIndex;
			on = b2OnIndex;
			table = b2TableIndex;
			above = b2AboveIndicies;
		}
		//Execute arms action
		switch (get<1>(sa).getAction()) //Action
		{
		case NOOP:
			sma = updateStateNOOP(s.back(), a, first);
			break;
		case PICK_UP:
			sma = updateStatePICK_UP(s.back(), loc, clear, table, a, first);
			break;
		case PUT_DOWN:
			sma = updateStatePUT_DOWN(s.back(), loc, a, first);
			break;
		case STACK:
			sma = updateStateSTACK(s.back(), loc, clear, a, first);
			break;
		case UNSTACK:
			sma = updateStateUNSTACK(s.back(), loc, on, above, clear, a, first);
			break;
		case MOVE:
			sma = updateStateMOVE(s.back(), a, first);
			break;
		}
		s.push_back(get<0>(sma));
		m.push_back(get<1>(sma));
		arms.push_back(get<2>(sma));
		//Update the state after one arm's action
		if ((i == 0) && (get<1>(sa).getAction() != NOOP) && (get<1>(sa).getAction() != MOVE))
		{
			updateIndicies(loc, s.back(), b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies);
		}
		++i;
	}
	//Return a new state with updated moves
	Arm a1 = Arm(-1, L1);
	Arm a2 = Arm(-1, L1);
	if (arms.front().getNum() == 1)
	{
		sm.addMoves2(m.back());
		m.pop_back();
		sm.addMoves1(m.back());
		a2 = arms.back();
		arms.pop_back();
		a1 = arms.back();
	}
	else
	{
		sm.addMoves1(m.back());
		m.pop_back();
		sm.addMoves2(m.back());
		a1 = arms.back();
		arms.pop_back();
		a2 = arms.back();
	}
	return StateMoves(s.back(), sm.getMoves1Value(), sm.getMoves2Value(), a1, a2);
}

/*this method adds all the Move action child states to the list of possible states. This is done in a separate method from 
find children because the MOVE action could replace either arm’s actions 
in the list of determined child states with the exception of an UNSTACK followed by PICK_UP or PUT_DOWN followed by STACK.*/
vector<StateMoves> addMoveChildren(StateMoves state, vector<StateMoves> children, int b1ClearIndex, int b2ClearIndex, int b1OnIndex, int b2OnIndex, int b1TableIndex, int b2TableIndex, vector<int> b1AboveIndicies, vector<int> b2AboveIndicies, Arm arm1, Arm arm2)
{
	vector<StateMoves> allChildren = children;
	for (StateMoves i : children)
	{
		if (!(i.getMoves2().back().getAction() == UNSTACK && i.getMoves1().back().getAction() == PICK_UP && i.getMoves2().back().getLocation() == i.getMoves1().back().getLocation()) 
			&& !(i.getMoves2().back().getAction() == PUT_DOWN && i.getMoves1().back().getAction() == STACK && i.getMoves2().back().getLocation() == i.getMoves1().back().getLocation()))
		{
			StateAction sa1 = make_tuple(arm1, i.getMoves1().back());
			StateAction sa2 = make_tuple(arm2, Move(MOVE, arm2.getLocation(), !i.getMoves1().back().isFirst()));
			allChildren.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}

		if (!(i.getMoves1().back().getAction() == UNSTACK && i.getMoves2().back().getAction() == PICK_UP && i.getMoves2().back().getLocation() == i.getMoves1().back().getLocation())
			&& !(i.getMoves1().back().getAction() == PUT_DOWN && i.getMoves2().back().getAction() == STACK && i.getMoves2().back().getLocation() == i.getMoves1().back().getLocation()))
		{
			StateAction sa1 = make_tuple(arm2, i.getMoves2().back());
			StateAction sa2 = make_tuple(arm1, Move(MOVE, arm1.getLocation(), !i.getMoves2().back().isFirst()));
			allChildren.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
	}

	StateAction sa1 = make_tuple(arm1, Move(MOVE, arm1.getLocation(), true));
	StateAction sa2 = make_tuple(arm2, Move(MOVE, arm2.getLocation(), true));
	allChildren.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
	return allChildren;
}

/*this method given the current state and the number blocks currently sorted in each stack, determines 
all the possible child states that could come from arm actions on the current state.*/
vector<StateMoves> findChildren(StateMoves state, int numL1Blocks, int numL1Placed, int numL2Blocks, int numL2Placed)
{
	vector<StateMoves> children;

	State s = state.getState();
	Arm arm1 = state.getArm1();
	Arm arm2 = state.getArm2();
	//Determine search space
	int b1ClearIndex;
	int b2ClearIndex;
	int b1OnIndex;
	int b2OnIndex;
	int b1TableIndex;
	int b2TableIndex;
	vector<int> b1AboveIndicies;
	vector<int> b2AboveIndicies;

	//Find the number of unsorted blocks that the arms can pick up
	int L1Available = numL1Blocks - numL1Placed;
	int L2Available = numL2Blocks - numL2Placed;

	//Get statistics about the current state
	updateIndicies(0, s, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies);
	updateIndicies(1, s, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies);

	//Placeholders
	StateAction sa1 =  make_tuple(arm1, Move(NOOP, L1, true));
	StateAction sa2 = make_tuple(arm1, Move(NOOP, L1, true));

	if (arm1.getBlock().getLetter() == "" && arm2.getBlock().getLetter() == "") // Both empty
	{
		if (b1ClearIndex == -1) // L1 Empty L2 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(NOOP, L1, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{

			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				if (L2Available > 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L2 Unstack L2: Order Irrelevent
					sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				else if (L2Available == 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
		}
		else if (b2ClearIndex == -1) //L2 Empty L1 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					StateAction sa1 = make_tuple(arm2, Move(NOOP, L1, true));
					StateAction sa2 = make_tuple(arm1, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					StateAction sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					StateAction sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				if (L1Available > 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L1 Unstack L1: Order Irrelevent
					sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				else if (L2Available == 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
			}
		}
		else if (b1OnIndex == -1) //L1 Has One L2 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				if (L1Available == 1)
				{
					//Pickup L1 NOOP: Order Irrelevent
					StateAction sa1 = make_tuple(arm2, Move(NOOP, L1, true));
					StateAction sa2 = make_tuple(arm1, Move(PICK_UP, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available == 1 && L2Available >= 1)
				{
					//Pickup L1 Unstack L2: Order Irrelevent
					sa1 = make_tuple(arm1, Move(PICK_UP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				if (L1Available == 1)
				{
					//Pickup L1 NOOP: Order Irrelevent
					StateAction sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					StateAction sa2 = make_tuple(arm2, Move(PICK_UP, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available == 1 && L2Available >= 1)
				{
					//Pickup L1 Unstack L2: Order Irrelevent
					sa1 = make_tuple(arm2, Move(PICK_UP, L1, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(NOOP, L1, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				if (L1Available == 1)
				{
					//Pickup L1 NOOP: Order Irrelevent
					StateAction sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					StateAction sa2 = make_tuple(arm2, Move(PICK_UP, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available > 1)
				{
					//Unstack L2 Unstack L2: Order Irrelevent
					sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
		}
		else if (b2OnIndex == -1) //L2 Has One L1 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				if (L2Available == 1)
				{
					//Pickup L2 NOOP: Order Irrelevent
					StateAction sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					StateAction sa2 = make_tuple(arm2, Move(PICK_UP, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available == 1 && L1Available >= 1)
				{
					//Pickup L2 Unstack L1: Order Irrelevent
					sa1 = make_tuple(arm2, Move(PICK_UP, L2, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(NOOP, L1, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				if (L2Available == 1)
				{
					//Pickup L2 NOOP: Order Irrelevent
					StateAction sa1 = make_tuple(arm2, Move(NOOP, L1, true));
					StateAction sa2 = make_tuple(arm1, Move(PICK_UP, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available == 1 && L1Available >= 1)
				{
					//Pickup L2 Unstack L1: Order Irrelevent
					sa1 = make_tuple(arm1, Move(PICK_UP, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available > 1)
				{
					//Unstack L1 Unstack L1: Order Irrelevent
					sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				if (L2Available == 1)
				{
					//Pickup L2 NOOP: Order Irrelevent
					StateAction sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					StateAction sa2 = make_tuple(arm2, Move(PICK_UP, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
		}
		else if (b1AboveIndicies.size() == 1) //L1 Has TWO L2 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(NOOP, L1, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available >= 1 && L1Available >= 1)
				{
					//Unstack L2 Unstack L1: Order Irrelevent
					sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(NOOP, L1, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available >= 1 && L1Available >= 1)
				{
					//Unstack L2 Unstack L1: Order Irrelevent
					sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				if (L1Available > 1)
				{
					//Unstack L1 Pickup L1: Order Irrelevent
					StateAction sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
					StateAction sa2 = make_tuple(arm2, Move(PICK_UP, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(NOOP, L1, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				if (L2Available > 1)
				{
					//Unstack L2 Unstack L2: Order Irrelevent
					sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(NOOP, L1, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
		}
		else if (b2AboveIndicies.size() == 1) //L2 Has TWO L1 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(NOOP, L2, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available >= 1 && L2Available >= 1)
				{
					//Unstack L1 Unstack L2: Order Irrelevent
					sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(NOOP, L2, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available >= 1 && L2Available >= 1)
				{
					//Unstack L1 Unstack L2: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available > 1)
				{
					//Unstack L1 Unstack L1: Order Irrelevent
					sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				if (L2Available > 1)
				{
					//Unstack L2 Pickup L2: Order Irrelevent
					StateAction sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
					StateAction sa2 = make_tuple(arm2, Move(PICK_UP, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
		}
		else // L1 Stacked L2 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				if (L2Available >= 1 && L1Available >= 1)
				{
					//Unstack L2 Unstack L1: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}

				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(NOOP, L2, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				if (L2Available >= 1 && L1Available >= 1)
				{
					//Unstack L2 Unstack L1: Order Irrelevent
					sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}

				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(NOOP, L2, true));
					sa2 = make_tuple(arm1, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L1Available > 1)
				{
					//Unstack L1 Unstack L1: Order Irrelevent
					sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				if (L2Available > 1)
				{
					//Unstack L2 Unstack L2: Order Irrelevent
					sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
		}
	}
	else if (arm1.getBlock().getLetter() != "" && arm2.getBlock().getLetter() != "") // Both full
	{
		if (b1ClearIndex == -1) // L1 Empty L2 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{

				//Stack L2 NOOP: Order Relevent
				sa1 = make_tuple(arm2, Move(STACK, L2, true));
				sa2 = make_tuple(arm1, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Stack L2 Put Down L1: Order Relevent
				sa1 = make_tuple(arm1, Move(PUT_DOWN, L1, true));
				sa2 = make_tuple(arm2, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Put Down L1 NOOP : Order Relevent
				sa1 = make_tuple(arm1, Move(PUT_DOWN, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{

				//Stack L2 NOOP: Order Relevent
				sa1 = make_tuple(arm1, Move(STACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Stack L2 Put Down L1: Order Relevent
				sa1 = make_tuple(arm2, Move(PUT_DOWN, L1, true));
				sa2 = make_tuple(arm1, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Put Down L1 NOOP : Order Relevent
				sa1 = make_tuple(arm2, Move(PUT_DOWN, L1, true));
				sa2 = make_tuple(arm1, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				//Put Down L1 Stack L1: Order Relevent
				sa1 = make_tuple(arm1, Move(PUT_DOWN, L1, true));
				sa2 = make_tuple(arm2, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				sa1 = make_tuple(arm2, Move(PUT_DOWN, L1, true));
				sa2 = make_tuple(arm1, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Put Down L1 NOOP : Order Relevent
				sa1 = make_tuple(arm1, Move(PUT_DOWN, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				sa1 = make_tuple(arm2, Move(PUT_DOWN, L1, true));
				sa2 = make_tuple(arm1, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				//Stack L2 Stack L2: Order Relevent
				StateAction sa1 = make_tuple(arm1, Move(STACK, L2, true));
				StateAction sa2 = make_tuple(arm2, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				sa1 = make_tuple(arm2, Move(STACK, L2, true));
				sa2 = make_tuple(arm1, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Stack L2 NOOP: Order Relevent
				sa1 = make_tuple(arm1, Move(STACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				sa1 = make_tuple(arm2, Move(STACK, L2, true));
				sa2 = make_tuple(arm1, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
		}
		else if (b2ClearIndex == -1) //L2 Empty L1 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				//Stack L1 NOOP: Order Relevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Stack L1 Put Down L2: Order Relevent
				sa1 = make_tuple(arm2, Move(PUT_DOWN, L2, true));
				sa2 = make_tuple(arm1, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Put Down L2 NOOP : Order Relevent
				sa1 = make_tuple(arm2, Move(PUT_DOWN, L2, true));
				sa2 = make_tuple(arm1, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				//Stack L1 NOOP: Order Relevent
				sa1 = make_tuple(arm2, Move(STACK, L1, true));
				sa2 = make_tuple(arm1, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Stack L1 Put Down L2: Order Relevent
				sa1 = make_tuple(arm1, Move(PUT_DOWN, L2, true));
				sa2 = make_tuple(arm2, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Put Down L2 NOOP : Order Relevent
				sa1 = make_tuple(arm1, Move(PUT_DOWN, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				//Stack L1 Stack L1: Order Relevent
				StateAction sa1 = make_tuple(arm1, Move(STACK, L1, true));
				StateAction sa2 = make_tuple(arm2, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				sa1 = make_tuple(arm2, Move(STACK, L1, true));
				sa2 = make_tuple(arm1, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Stack L1 NOOP: Order Relevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				sa1 = make_tuple(arm2, Move(STACK, L1, true));
				sa2 = make_tuple(arm1, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				//Put Down L2 Stack L2: Order Relevent
				sa1 = make_tuple(arm1, Move(PUT_DOWN, L2, true));
				sa2 = make_tuple(arm2, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				sa1 = make_tuple(arm2, Move(PUT_DOWN, L2, true));
				sa2 = make_tuple(arm1, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Put Down L2 NOOP : Order Relevent
				sa1 = make_tuple(arm1, Move(PUT_DOWN, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				sa1 = make_tuple(arm2, Move(PUT_DOWN, L2, true));
				sa2 = make_tuple(arm1, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
		}
		else // L1 Stacked L2 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				//Stack L2 NOOP: Order Relevent
				sa1 = make_tuple(arm2, Move(STACK, L2, true));
				sa2 = make_tuple(arm1, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Stack L1 NOOP: Order Relevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Stack L1 Stack L2: Order Relevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				//Stack L2 NOOP: Order Relevent
				sa1 = make_tuple(arm1, Move(STACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Stack L1 NOOP: Order Relevent
				sa1 = make_tuple(arm2, Move(STACK, L1, true));
				sa2 = make_tuple(arm1, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Stack L1 Stack L2: Order Relevent
				sa1 = make_tuple(arm2, Move(STACK, L1, true));
				sa2 = make_tuple(arm1, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				//Stack L1 Stack L1: Order Relevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				sa1 = make_tuple(arm2, Move(STACK, L1, true));
				sa2 = make_tuple(arm1, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Stack L1 NOOP: Order Relevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				sa1 = make_tuple(arm2, Move(STACK, L1, true));
				sa2 = make_tuple(arm1, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				//Stack L2 Stack L2: Order Relevent
				StateAction sa1 = make_tuple(arm1, Move(STACK, L2, true));
				StateAction sa2 = make_tuple(arm2, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				sa1 = make_tuple(arm2, Move(STACK, L2, true));
				sa2 = make_tuple(arm1, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Stack L2 NOOP: Order Relevent
				sa1 = make_tuple(arm1, Move(STACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				sa1 = make_tuple(arm2, Move(STACK, L2, true));
				sa2 = make_tuple(arm1, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
		}
	}
	else if (arm1.getBlock().getLetter() != "" && arm2.getBlock().getLetter() == "") //arm2 Empty arm1 Full
	{
		if (b1ClearIndex == -1) // L1 Empty L2 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				if (L2Available >= 1)
				{
					//Put Down L1 Unstack L2: Order Irrelevent
					StateAction sa1 = make_tuple(arm1, Move(PUT_DOWN, L1, true));
					StateAction sa2 = make_tuple(arm2, Move(UNSTACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm1, Move(NOOP, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				//Put Down L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(PUT_DOWN, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				//Stack L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				//Put Down L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(PUT_DOWN, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm1, Move(NOOP, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L2 Stack L2: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm1, Move(STACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				//Stack L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
		}
		else if (b2ClearIndex == -1) //L2 Empty L1 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				//Stack L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				if (L1Available >= 1)
				{
					//Put Down L2 Unstack L1: Order Irrelevent
					StateAction sa1 = make_tuple(arm1, Move(PUT_DOWN, L2, true));
					StateAction sa2 = make_tuple(arm2, Move(UNSTACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm1, Move(NOOP, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				//Put Down L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(PUT_DOWN, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm1, Move(NOOP, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L1 Stack L1: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm1, Move(STACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				//Stack L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				//Put Down L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(PUT_DOWN, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
		}
		else if (b1OnIndex == -1) //L1 Has One L2 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm1, Move(NOOP, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L2 Stack L1: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm1, Move(STACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				//Stack L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				//Stack L2 NOOP: Order Irrelevent
				StateAction sa1 = make_tuple(arm1, Move(STACK, L2, true));
				StateAction sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				if (L1Available >= 1)
				{
					//Pickup L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(PICK_UP, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Pickup L1 Stack L2: Order Irrelevent
					sa1 = make_tuple(arm2, Move(PICK_UP, L1, true));
					sa2 = make_tuple(arm1, Move(STACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				//Stack L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				if (L1Available >= 1)
				{
					//Pickup L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L1, true));
					sa2 = make_tuple(arm2, Move(PICK_UP, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Pickup L1 Put Down L1: Order Irrelevent
					sa1 = make_tuple(arm2, Move(PICK_UP, L1, true));
					sa2 = make_tuple(arm1, Move(PUT_DOWN, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
				//Stack L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				//Stack L2 NOOP: Order Irrelevent
				StateAction sa1 = make_tuple(arm1, Move(STACK, L2, true));
				StateAction sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm1, Move(NOOP, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L2 Stack L2: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm1, Move(STACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
		}
		else if (b2OnIndex == -1) //L2 Has One L1 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				//Stack L1 NOOP: Order Irrelevent
				StateAction sa1 = make_tuple(arm1, Move(STACK, L1, true));
				StateAction sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				if (L2Available >= 1)
				{
					//Pickup L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L2, true));
					sa2 = make_tuple(arm2, Move(PICK_UP, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Pickup L2 Stack L1: Order Irrelevent
					sa1 = make_tuple(arm2, Move(PICK_UP, L2, true));
					sa2 = make_tuple(arm1, Move(STACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm1, Move(NOOP, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L1 Stack L2: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm1, Move(STACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}

				//Stack L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				//Stack L1 NOOP: Order Irrelevent
				StateAction sa1 = make_tuple(arm1, Move(STACK, L1, true));
				StateAction sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm1, Move(NOOP, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L1 Stack L1: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm1, Move(STACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				if (L2Available >= 1)
				{
					//Pickup L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm1, Move(NOOP, L2, true));
					sa2 = make_tuple(arm2, Move(PICK_UP, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Pickup L2 Put Down L2: Order Irrelevent
					sa1 = make_tuple(arm2, Move(PICK_UP, L2, true));
					sa2 = make_tuple(arm1, Move(PUT_DOWN, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}

				//Stack L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
		}
		else // L1 Stacked L2 Stacked
		{
			if (arm1.getLocation() == L1 && arm2.getLocation() == L2)
			{
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm1, Move(NOOP, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L2 Stack L1: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm1, Move(STACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				}

				//Stack L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L1)
			{
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					StateAction sa1 = make_tuple(arm2, Move(UNSTACK, L1, true));
					StateAction sa2 = make_tuple(arm1, Move(NOOP, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L1 Stack L2: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm1, Move(STACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				}

				//Stack L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

			}
			else if (arm1.getLocation() == L1 && arm2.getLocation() == L1)
			{
				if (L1Available >= 1)
				{
					//Unstack L1 NOOP: Order Irrelevent
					StateAction sa1 = make_tuple(arm2, Move(UNSTACK, L1, true));
					StateAction sa2 = make_tuple(arm1, Move(NOOP, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L1 Stack L1: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L1, true));
					sa2 = make_tuple(arm1, Move(STACK, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}

				//Stack L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			else if (arm1.getLocation() == L2 && arm2.getLocation() == L2)
			{
				if (L2Available >= 1)
				{
					//Unstack L2 NOOP: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm1, Move(NOOP, L1, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

					//Unstack L2 Stack L2: Order Irrelevent
					sa1 = make_tuple(arm2, Move(UNSTACK, L2, true));
					sa2 = make_tuple(arm1, Move(STACK, L2, false));
					children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
				}

				//Stack L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(STACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
		}
	}
	else if (arm1.getBlock().getLetter() == "" && arm2.getBlock().getLetter() != "") //Arm 1 Empty arm2 Full
	{
	if (b1ClearIndex == -1) // L1 Empty L2 Stacked
	{
		if (arm2.getLocation() == L1 && arm1.getLocation() == L2)
		{
			if (L2Available >= 1)
			{
				//Put Down L1 Unstack L2: Order Irrelevent
				StateAction sa1 = make_tuple(arm2, Move(PUT_DOWN, L1, true));
				StateAction sa2 = make_tuple(arm1, Move(UNSTACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Unstack L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			//Put Down L1 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(PUT_DOWN, L1, true));
			sa2 = make_tuple(arm1, Move(NOOP, L1, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
		else if (arm2.getLocation() == L2 && arm1.getLocation() == L1)
		{
			//Stack L2 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L2, true));
			sa2 = make_tuple(arm1, Move(NOOP, L1, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
		else if (arm2.getLocation() == L1 && arm1.getLocation() == L1)
		{
			//Put Down L1 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(PUT_DOWN, L1, true));
			sa2 = make_tuple(arm1, Move(NOOP, L1, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
		else if (arm2.getLocation() == L2 && arm1.getLocation() == L2)
		{
			if (L2Available >= 1)
			{
				//Unstack L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Unstack L2 Stack L2: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
				sa2 = make_tuple(arm2, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			//Stack L2 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L2, true));
			sa2 = make_tuple(arm1, Move(NOOP, L1, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
	}
	else if (b2ClearIndex == -1) //L2 Empty L1 Stacked
	{
		if (arm2.getLocation() == L1 && arm1.getLocation() == L2)
		{
			//Stack L1 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L1, true));
			sa2 = make_tuple(arm1, Move(NOOP, L2, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
		else if (arm2.getLocation() == L2 && arm1.getLocation() == L1)
		{
			if (L1Available >= 1)
			{
				//Put Down L2 Unstack L1: Order Irrelevent
				StateAction sa1 = make_tuple(arm2, Move(PUT_DOWN, L2, true));
				StateAction sa2 = make_tuple(arm1, Move(UNSTACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Unstack L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			//Put Down L2 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(PUT_DOWN, L2, true));
			sa2 = make_tuple(arm1, Move(NOOP, L2, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
		else if (arm2.getLocation() == L1 && arm1.getLocation() == L1)
		{
			if (L1Available >= 1)
			{
				//Unstack L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Unstack L1 Stack L1: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
				sa2 = make_tuple(arm2, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			//Stack L1 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L1, true));
			sa2 = make_tuple(arm1, Move(NOOP, L2, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
		else if (arm2.getLocation() == L2 && arm1.getLocation() == L2)
		{
			//Put Down L2 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(PUT_DOWN, L2, true));
			sa2 = make_tuple(arm1, Move(NOOP, L2, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
	}
	else if (b1OnIndex == -1) //L1 Has One L2 Stacked
	{
		if (arm2.getLocation() == L1 && arm1.getLocation() == L2)
		{
			if (L2Available >= 1)
			{
				//Unstack L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Unstack L2 Stack L1: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
				sa2 = make_tuple(arm2, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			//Stack L1 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L1, true));
			sa2 = make_tuple(arm1, Move(NOOP, L1, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
		else if (arm2.getLocation() == L2 && arm1.getLocation() == L1)
		{
			//Stack L2 NOOP: Order Irrelevent
			StateAction sa1 = make_tuple(arm2, Move(STACK, L2, true));
			StateAction sa2 = make_tuple(arm1, Move(NOOP, L1, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

			if (L1Available >= 1)
			{
				//Pickup L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm2, Move(NOOP, L1, true));
				sa2 = make_tuple(arm1, Move(PICK_UP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Pickup L1 Stack L2: Order Irrelevent
				sa1 = make_tuple(arm1, Move(PICK_UP, L1, true));
				sa2 = make_tuple(arm2, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			//Stack L1 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L1, true));
			sa2 = make_tuple(arm1, Move(NOOP, L1, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
		else if (arm2.getLocation() == L1 && arm1.getLocation() == L1)
		{
			if (L1Available >= 1)
			{
				//Pickup L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm2, Move(NOOP, L1, true));
				sa2 = make_tuple(arm1, Move(PICK_UP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Pickup L1 Put Down L1: Order Irrelevent
				sa1 = make_tuple(arm1, Move(PICK_UP, L1, true));
				sa2 = make_tuple(arm2, Move(PUT_DOWN, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
			//Stack L1 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L1, true));
			sa2 = make_tuple(arm1, Move(NOOP, L1, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
		else if (arm2.getLocation() == L2 && arm1.getLocation() == L2)
		{
			//Stack L2 NOOP: Order Irrelevent
			StateAction sa1 = make_tuple(arm2, Move(STACK, L2, true));
			StateAction sa2 = make_tuple(arm1, Move(NOOP, L1, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

			if (L2Available >= 1)
			{
				//Unstack L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Unstack L2 Stack L2: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
				sa2 = make_tuple(arm2, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
		}
	}
	else if (b2OnIndex == -1) //L2 Has One L1 Stacked
	{
		if (arm2.getLocation() == L1 && arm1.getLocation() == L2)
		{
			//Stack L1 NOOP: Order Irrelevent
			StateAction sa1 = make_tuple(arm2, Move(STACK, L1, true));
			StateAction sa2 = make_tuple(arm1, Move(NOOP, L2, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

			if (L2Available >= 1)
			{
				//Pickup L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm2, Move(NOOP, L2, true));
				sa2 = make_tuple(arm1, Move(PICK_UP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Pickup L2 Stack L1: Order Irrelevent
				sa1 = make_tuple(arm1, Move(PICK_UP, L2, true));
				sa2 = make_tuple(arm2, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
		}
		else if (arm2.getLocation() == L2 && arm1.getLocation() == L1)
		{
			if (L1Available >= 1)
			{
				//Unstack L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Unstack L1 Stack L2: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
				sa2 = make_tuple(arm2, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}

			//Stack L2 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L2, true));
			sa2 = make_tuple(arm1, Move(NOOP, L2, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
		else if (arm2.getLocation() == L1 && arm1.getLocation() == L1)
		{
			//Stack L1 NOOP: Order Irrelevent
			StateAction sa1 = make_tuple(arm2, Move(STACK, L1, true));
			StateAction sa2 = make_tuple(arm1, Move(NOOP, L2, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

			if (L1Available >= 1)
			{
				//Unstack L1 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
				sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Unstack L1 Stack L1: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
				sa2 = make_tuple(arm2, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}
		}
		else if (arm2.getLocation() == L2 && arm1.getLocation() == L2)
		{
			if (L2Available >= 1)
			{
				//Pickup L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm2, Move(NOOP, L2, true));
				sa2 = make_tuple(arm1, Move(PICK_UP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Pickup L2 Put Down L2: Order Irrelevent
				sa1 = make_tuple(arm1, Move(PICK_UP, L2, true));
				sa2 = make_tuple(arm2, Move(PUT_DOWN, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}

			//Stack L2 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L2, true));
			sa2 = make_tuple(arm1, Move(NOOP, L2, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
	}
	else // L1 Stacked L2 Stacked
	{
		if (arm2.getLocation() == L1 && arm1.getLocation() == L2)
		{
			if (L2Available >= 1)
			{
				//Unstack L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Unstack L2 Stack L1: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
				sa2 = make_tuple(arm2, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

			}

			//Stack L1 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L1, true));
			sa2 = make_tuple(arm1, Move(NOOP, L2, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

		}
		else if (arm2.getLocation() == L2 && arm1.getLocation() == L1)
		{
			if (L1Available >= 1)
			{
				//Unstack L1 NOOP: Order Irrelevent
				StateAction sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
				StateAction sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Unstack L1 Stack L2: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
				sa2 = make_tuple(arm2, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

			}

			//Stack L2 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L2, true));
			sa2 = make_tuple(arm1, Move(NOOP, L2, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

		}
		else if (arm2.getLocation() == L1 && arm1.getLocation() == L1)
		{
			if (L1Available >= 1)
			{
				//Unstack L1 NOOP: Order Irrelevent
				StateAction sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
				StateAction sa2 = make_tuple(arm2, Move(NOOP, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Unstack L1 Stack L1: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L1, true));
				sa2 = make_tuple(arm2, Move(STACK, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}

			//Stack L1 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L1, true));
			sa2 = make_tuple(arm1, Move(NOOP, L2, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
		else if (arm2.getLocation() == L2 && arm1.getLocation() == L2)
		{
			if (L2Available >= 1)
			{
				//Unstack L2 NOOP: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
				sa2 = make_tuple(arm2, Move(NOOP, L1, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));

				//Unstack L2 Stack L2: Order Irrelevent
				sa1 = make_tuple(arm1, Move(UNSTACK, L2, true));
				sa2 = make_tuple(arm2, Move(STACK, L2, false));
				children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
			}

			//Stack L2 NOOP: Order Irrelevent
			sa1 = make_tuple(arm2, Move(STACK, L2, true));
			sa2 = make_tuple(arm1, Move(NOOP, L2, false));
			children.push_back(getNewState(state, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, { sa1, sa2 }));
		}
	}
	}
	return addMoveChildren(state, children, b1ClearIndex, b2ClearIndex, b1OnIndex, b2OnIndex, b1TableIndex, b2TableIndex, b1AboveIndicies, b2AboveIndicies, arm1, arm2);
}

/*this method sorts the states based on their ranks and determines the best next possible child state.*/
StateMovesRank findBest(vector<StateMovesRank> states)
{
	sort(states.begin(), states.end());
	return states.front();
}

/*this method sorts the L1 stack.*/
StateMoves searchL1(State currentState, State endState, Arm arm1, Arm arm2)
{
	//Set up the state
	sort(currentState.getL1().begin(), currentState.getL1().end());

	sort(endState.getL1().begin(), endState.getL1().end());

	vector<StateMovesRank> states;
	set<State> foundStates;
	Moves m1 = {};
	Moves m2 = {};
	currentState.setL1Total();
	currentState.setL2Total();
	currentState.setL1NumPlaced(endState);
	currentState.setL1NextBlock(endState);
	currentState.setL1NextBlockLoc();
	endState.setL1Total();
	StateMoves sm = StateMoves(currentState, m1, m2, arm1, arm2);
	StateMoves prev = sm;
	states.push_back(StateMovesRank(sm, 1));
	while (states.size() >= 1)
	{
		StateMovesRank spRank = findBest(states);
		StateMoves sp = spRank.getStateMoves();
		//Remove here
		states.erase(std::remove(states.begin(), states.end(), spRank), states.end());
		if (prev.getState().getL1Placed() < sp.getState().getL1Placed()) //If a block was placed find the next block
		{
			sp.getState().setL1NextBlock(endState);
			sp.getState().setL1NextBlockLoc();
			states.clear();
		}
		if ((sp.getState().getL1Placed() == endState.getL1Total() && sp.getState().getL1()[0] == endState.getL1()[0]) || 
			(endState.getL1()[0].getName() == EMPTY && sp.getState().getL1()[0].getName() == EMPTY)) //If L1 is sorted
		{
			return sp;
		}
		else  
		{
			states.clear(); //Only rank the current state's children
			//Find and rank the state's children
			vector<StateMoves> children = findChildren(sp, sp.getState().getL1Total(), sp.getState().getL1Placed(), sp.getState().getL2Total(), sp.getState().getL2Placed());
			for (StateMoves i : children)
			{
				states.push_back(StateMovesRank(i, endState, sp.getState().getL1Placed(), sp.getState().getL1BlocksOutPlace(), sp.nextL1BlockAbove(), prev.nextL1BlockAbove(), prev.getState().getL1BlocksOutPlace(), L1));
			}
		}
		prev = sp;
	}
}

/*this method sorts the L2 stack.*/
StateMoves searchL2(State currentState, State endState, Moves l1Moves1, Moves l1Moves2, Arm arm1, Arm arm2)
{
	//Set up the state
	sort(currentState.getL2().begin(), currentState.getL2().end());
	sort(endState.getL2().begin(), endState.getL2().end());
	vector<StateMovesRank> states;
	Moves m1 = l1Moves1;
	Moves m2 = l1Moves2;
	currentState.setL1Total();
	currentState.setL1NumPlaced(endState);
	currentState.setL2Total();
	currentState.setL2NumPlaced(endState);
	currentState.setL2NextBlock(endState);
	currentState.setL2NextBlockLoc();
	endState.setL2Total();
	StateMoves sm = StateMoves(currentState, m1, m2, arm1, arm2);
	StateMoves prev = StateMoves();
	states.push_back(StateMovesRank(sm, 1));

	while (states.size() >= 1)
	{
		StateMovesRank spRank = findBest(states);
		StateMoves sp = spRank.getStateMoves();
		//Remove here
		states.erase(std::remove(states.begin(), states.end(), spRank), states.end());
		if (prev.getState().getL2Placed() < sp.getState().getL2Placed()) //If a block was placed find the next block
		{
			sp.getState().setL2NextBlock(endState);
			sp.getState().setL2NextBlockLoc();
			states.clear();
		}
		if ((sp.getState().getL2Placed() == endState.getL2Total() && sp.getState().getL2()[0] == endState.getL2()[0]) ||
			(endState.getL2()[0].getName() == EMPTY && sp.getState().getL2()[0].getName() == EMPTY))
		{
			return sp;
		}
		else 
		{
			states.clear();//Only rank the current state's children
			//Find and rank the state's children
			vector<StateMoves> children = findChildren(sp, sp.getState().getL1Total(), sp.getState().getL1Placed(), sp.getState().getL2Total(), sp.getState().getL2Placed());
			for (StateMoves i : children)
			{
				states.push_back(StateMovesRank(i, endState, sp.getState().getL2Placed(), sp.getState().getL2BlocksOutPlace(), sp.nextL2BlockAbove(), prev.nextL2BlockAbove(), prev.getState().getL2BlocksOutPlace(), L2));
			}
			prev = sp;
		}
	}
}

/*this method calls the methods to sort the L1 stack and then the L2 stack*/
StateMoves search(State currentState, State endState, Arm arm1, Arm arm2)
{
	StateMoves L1sort = searchL1(currentState, endState, arm1, arm2);
	StateMoves L2sort = searchL2(L1sort.getState(), endState, L1sort.getMoves1(), L1sort.getMoves2(), L1sort.getArm1(), L1sort.getArm2());
 	return L2sort;
}