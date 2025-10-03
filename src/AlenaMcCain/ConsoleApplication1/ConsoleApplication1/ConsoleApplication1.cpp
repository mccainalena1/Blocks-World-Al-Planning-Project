#include "pch.h"

#include <iostream>
#include <sstream>

#include "Draw.h"
#include "Search.h"

vector<DrawState> drawStates;

State startState = State();
State endState = State();

vector<Block> L1Vec;
vector<Block> L2Vec;
vector<Block> L1VecEnd;
vector<Block> L2VecEnd;

Arm arm1 = Arm(1, L1);
Arm arm2 = Arm(2, L2);

StateMoves path = StateMoves();

Moves  m1Vec;
Moves m2Vec;

/*this method loads the inputs from the console for the states of the block world and puts them into vectors*/
void loadInputs()
{
	string letter;

	string L1Input;
	cout << "Enter L1 start blocks from bottom to top seperated by spaces ";
	getline(cin, L1Input);
	istringstream ss(L1Input);
	while (ss >> letter)
	{
		L1Vec.push_back(letter);
	}
	string L2Input;
	cout << "Enter L2 start blocks from bottom to top seperated by spaces ";
	getline(cin, L2Input);
	istringstream ss2(L2Input);
	while (ss2 >> letter)
	{
		L2Vec.push_back(letter);
	}

	string L1Output;
	cout << "Enter L1 end blocks from bottom to top seperated by spaces ";
	getline(cin, L1Output);
	istringstream ss3(L1Output);
	while (ss3 >> letter)
	{
		L1VecEnd.push_back(letter);
	}
	string L2Output;
	cout << "Enter L2 end blocks from bottom to top seperated by spaces ";
	getline(cin, L2Output);
	istringstream ss4(L2Output);
	while (ss4 >> letter)
	{
		L2VecEnd.push_back(letter);
	}
}

/*this method takes the two input vectors of the starting blocks and converts them into relations that the searching algorithm can use.*/
void loadStartRelations()
{
	vector<Relation> L1Relations = {};
	vector<Relation> L2Relations = {};
	
	if (L1Vec.size() > 0)
	{
		//Table
		L1Relations.push_back(Relation(TABLE, Block(L1Vec.front()), Block()));
		//Clear
		L1Relations.push_back(Relation(CLEAR, Block(L1Vec.back()), Block()));
	}
	else
	{
		L1Relations.push_back(Relation(EMPTY, Block(), Block()));
	}
	if (L1Vec.size() > 1)
	{
		for (int index = 0; index < L1Vec.size() - 1; ++index)
		{
			L1Relations.push_back(Relation(ON, Block(L1Vec[index +1]), Block(L1Vec[index])));
			for (int j = index + 1; j < L1Vec.size(); ++j)
			{
				L1Relations.push_back(Relation(ABOVE, Block(L1Vec[j]), Block(L1Vec[index])));
			}
		}
	}

	if (L2Vec.size() > 0)
	{
		//Table
		L2Relations.push_back(Relation(TABLE, Block(L2Vec.front()), Block()));
		//Clear
		L2Relations.push_back(Relation(CLEAR, Block(L2Vec.back()), Block()));
	}
	else
	{
		L2Relations.push_back(Relation(EMPTY, Block(), Block()));
	}
	if (L2Vec.size() > 1)
	{
		for (int index = 0; index < L2Vec.size() - 1; ++index)
		{
			L2Relations.push_back(Relation(ON, Block(L2Vec[index + 1]), Block(L2Vec[index])));
			for (int j = index + 1; j < L2Vec.size(); ++j)
			{
				L2Relations.push_back(Relation(ABOVE, Block(L2Vec[j]), Block(L2Vec[index])));
			}
		}
	}
	startState.setL1(L1Relations);
	startState.setL2(L2Relations);
}

/*this method takes the two input vectors of the ending blocks and converts them into relations that the searching algorithm can use.*/
void loadEndRelations()
{
	vector<Relation> L1Relations = {};
	vector<Relation> L2Relations = {};

	if (L1VecEnd.size() > 0)
	{
		//Table
		L1Relations.push_back(Relation(TABLE, Block(L1VecEnd.front()), Block()));
		//Clear
		L1Relations.push_back(Relation(CLEAR, Block(L1VecEnd.back()), Block()));
	}
	else
	{
		L1Relations.push_back(Relation(EMPTY, Block(), Block()));
	}
	if (L1VecEnd.size() > 1)
	{
		for (int index = 0; index < L1VecEnd.size() - 1; ++index)
		{
			L1Relations.push_back(Relation(ON, Block(L1VecEnd[index + 1]), Block(L1VecEnd[index])));
			for (int j = index + 1; j < L1VecEnd.size(); ++j)
			{
				L1Relations.push_back(Relation(ABOVE, Block(L1VecEnd[j]), Block(L1VecEnd[index])));
			}
		}
	}

	if (L2VecEnd.size() > 0)
	{
		//Table
		L2Relations.push_back(Relation(TABLE, Block(L2VecEnd.front()), Block()));
		//Clear
		L2Relations.push_back(Relation(CLEAR, Block(L2VecEnd.back()), Block()));
	}
	else
	{
		L2Relations.push_back(Relation(EMPTY, Block(), Block()));
	}
	if (L2VecEnd.size() > 1)
	{
		for (int index = 0; index < L2VecEnd.size() - 1; ++index)
		{
			L2Relations.push_back(Relation(ON, Block(L2VecEnd[index + 1]), Block(L2VecEnd[index])));
			for (int j = index + 1; j < L2VecEnd.size(); ++j)
			{
				L2Relations.push_back(Relation(ABOVE, Block(L2VecEnd[j]), Block(L2VecEnd[index])));
			}
		}
	}
	endState.setL1(L1Relations);
	endState.setL2(L2Relations);
}

/*is the program is just started, the update method takes all the input and uses that input to determine all of the moves for the arms. Once that is completed, 
the update method is used to draw the new moves and states in the graphical user interface in the correct order.*/
vector<string> update(bool& start)
{
	if (start)
	{
		loadInputs();
		loadStartRelations();
		loadEndRelations();
		path = search(startState, endState, arm1, arm2);
		m1Vec = path.getMoves1();
		m2Vec = path.getMoves2();
		reverse(m1Vec.begin(), m1Vec.end());
		reverse(m2Vec.begin(), m2Vec.end());
		start = false;
	}
	else if (m1Vec.size() > 0)
	{
		Move m1 = m1Vec.back();
		Move m2 = m2Vec.back();
		string s1;
		string s2;
		std::ostringstream oss1;
		std::ostringstream oss2;
		if (m1.isFirst()) //Arm 1 First
		{
			switch (m1.getAction()) //Action
			{
			case NOOP:
				s1 = arm1.noop();
				break;
			case PICK_UP:
				s1 = arm1.pickUp(m1.getLocation());
				break;
			case PUT_DOWN:
				s1 = arm1.putDown(m1.getLocation());
				break;
			case STACK:
				s1 = arm1.stack(m1.getLocation());
				break;
			case UNSTACK:
				s1 = arm1.unstack(m1.getLocation());
				break;
			case MOVE:
				s1 = arm1.move(m1.getLocation());
			}

			switch (m2.getAction()) //Action
			{
			case NOOP:
				s2 = arm2.noop();
				break;
			case PICK_UP:
				s2 = arm2.pickUp(m2.getLocation());
				break;
			case PUT_DOWN:
				s2 = arm2.putDown(m2.getLocation());
				break;
			case STACK:
				s2 = arm2.stack(m2.getLocation());
				break;
			case UNSTACK:
				s2 = arm2.unstack(m2.getLocation());
				break;
			case MOVE:
				s2 = arm2.move(m2.getLocation());
			}
			oss1 << "1. " << s1;
			oss2 << "2. " << s2;
		}
		else //Arm 2 first
		{
			switch (m2.getAction()) //Action
			{
			case NOOP:
				s2 = arm2.noop();
				break;
			case PICK_UP:
				s2 = arm2.pickUp(m2.getLocation());
				break;
			case PUT_DOWN:
				s2 = arm2.putDown(m2.getLocation());
				break;
			case STACK:
				s2 = arm2.stack(m2.getLocation());
				break;
			case UNSTACK:
				s2 = arm2.unstack(m2.getLocation());
				break;
			case MOVE:
				s2 = arm2.move(m2.getLocation());
			}

			switch (m1.getAction()) //Action
			{
			case NOOP:
				s1 = arm1.noop();
				break;
			case PICK_UP:
				s1 = arm1.pickUp(m1.getLocation());
				break;
			case PUT_DOWN:
				s1 = arm1.putDown(m1.getLocation());
				break;
			case STACK:
				s1 = arm1.stack(m1.getLocation());
				break;
			case UNSTACK:
				s1 = arm1.unstack(m1.getLocation());
				break;
			}
			
			oss1 << "2. " << s1;
			oss2 << "1. " << s2;
		}
		m1Vec.pop_back();
		m2Vec.pop_back();
		return { oss1.str(), oss2.str() };
	}
	return { "", "" };
}

/*the main function is responsible for obtaining user input, drawing the graphical user interface, and updating the graphical user interface.*/
int main()
{
	bool start = true;
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "My window");
	sf::Font font;
	font.loadFromFile("arial.ttf");
	
	sf::Clock clock;

	sf::View view;
	view.reset(sf::FloatRect(0, 0, 1920, 1080));
	view.setViewport(sf::FloatRect(0, 0, 1, 1));

	while (window.isOpen())
	{
		clock.restart();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear(sf::Color::White);
		view.reset(sf::FloatRect(0, frameSize * (drawStates.size()), 1920, 1080));
		window.setView(view);
		DrawState state;
		vector<string> moves = update(start);
		drawTable(font, state);
		drawL1Blocks(font, state);
		drawL2Blocks(font, state);
		drawArms(font, state);
		drawArm1Block(font, arm1, state);
		drawArm2Block(font, arm2, state);
		drawArm1Action(font, state, moves.at(0));
		drawArm2Action(font, state, moves.at(1));
		drawStates.push_back(state);
		drawAllStates(window, drawStates);
		insertLag();
		window.display();
	}
	return 0;
}
