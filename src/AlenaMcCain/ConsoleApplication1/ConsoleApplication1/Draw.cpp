#include "pch.h"

#include "Draw.h"

/*this file contains all the methods necessary for storing all the objects that draw the graphical user interface.
this file contains all of the methods used for drawing the graphical user interface. 
The various methods create shapes and text and since the proper positions of those objects in the window for when they will be drawn. 
It also has the methods that allow all of the current states to be drawn for the entirety of the program.*/

void insertLag()
{
	sf::Time time = sf::seconds(delay);
	sf::Clock clock;
	sf::Time elapsed = clock.restart();
	while (elapsed <= time)
	{
		elapsed = clock.getElapsedTime();
	}
}

void drawTable(sf::Font& font, DrawState& state)
{
	sf::RectangleShape rectangle(sf::Vector2f(tableLength, tableWidth));
	rectangle.setFillColor(sf::Color::Black);
	rectangle.setPosition(tableXPos, tableYStartPos + (frameSize * drawStates.size()));
	state.addRectangle(rectangle);

	sf::Text L1text;
	L1text.setFont(font);
	L1text.setString("L1");
	L1text.setCharacterSize(locationTextSize);
	L1text.setFillColor(sf::Color::White);
	L1text.setStyle(sf::Text::Bold);
	L1text.setPosition(L1TextXPos, LTextYStartPos + (frameSize * drawStates.size()));
	state.addText(L1text);

	sf::Text L2text;
	L2text.setFont(font);
	L2text.setString("L2");
	L2text.setCharacterSize(locationTextSize);
	L2text.setFillColor(sf::Color::White);
	L2text.setStyle(sf::Text::Bold);
	L2text.setPosition(L2TextXPos, LTextYStartPos + (frameSize * drawStates.size()));
	state.addText(L2text);
}

void drawBlockOnTable(sf::Font& font, Block block, Location location, int pos, DrawState& state)
{
	sf::RectangleShape rectangle(sf::Vector2f(blockSize, blockSize));
	rectangle.setFillColor(sf::Color::Magenta);
	rectangle.setOutlineColor(sf::Color::White);
	rectangle.setOutlineThickness(blockOutlineThickness);

	sf::Text blockText;
	blockText.setFont(font);
	blockText.setString(block.getLetter());
	blockText.setCharacterSize(locationTextSize);
	blockText.setFillColor(sf::Color::White);
	blockText.setStyle(sf::Text::Bold);
	if (location == Location::L1)
	{
		rectangle.setPosition(L1BlockXPos, LBlockYStartPos - (pos * blockSize) + (frameSize * drawStates.size()));
		blockText.setPosition(L1BlockTextXPos, LBlockTextYStartPos - (pos * blockSize) + (frameSize * drawStates.size()));
	}
	else
	{
		rectangle.setPosition(L2BlockXPos, LBlockYStartPos - (pos * blockSize) + (frameSize * drawStates.size()));
		blockText.setPosition(L2BlockTextXPos, LBlockYStartPos - (pos * blockSize) + (frameSize * drawStates.size()));
	}
	state.addRectangle(rectangle);
	state.addText(blockText);
}

void drawL1Blocks(sf::Font& font, DrawState& state)
{
	int index = 0;
	for (Block b : L1Vec)
	{
		drawBlockOnTable(font, b, Location::L1, index, state);
		++index;
	}
}

void drawL2Blocks(sf::Font& font, DrawState& state)
{
	int index = 0;
	for (Block b : L2Vec)
	{
		drawBlockOnTable(font, b, Location::L2, index, state);
		++index;
	}
}

void drawArms(sf::Font& font, DrawState& state)
{
	sf::CircleShape arm1(armSize, 3);
	arm1.setOutlineColor(sf::Color::Black);
	arm1.setOutlineThickness(armOutlineThickness);
	arm1.setPosition(arm1XStartPos, arm1YStartPos + (frameSize * drawStates.size()));
	state.addTriangle(arm1);

	sf::CircleShape arm2(armSize, 3);
	arm2.setOutlineColor(sf::Color::Black);
	arm2.setOutlineThickness(armOutlineThickness);
	arm2.setPosition(arm2XStartPos, arm2YStartPos + (frameSize * drawStates.size()));
	state.addTriangle(arm2);
}

void drawArm1Block(sf::Font& font, Arm arm, DrawState& state)
{
	if (arm.getBlock().getLetter() != "")
	{
		sf::RectangleShape rectangle(sf::Vector2f(blockSize, blockSize));
		rectangle.setFillColor(sf::Color::Magenta);
		rectangle.setOutlineColor(sf::Color::White);
		rectangle.setOutlineThickness(blockOutlineThickness);
		rectangle.setPosition(arm1BlockXPos, arm1BlockYStartPos + (frameSize * drawStates.size()));
		state.addRectangle(rectangle);

		sf::Text blockText;
		blockText.setFont(font);
		blockText.setString(arm.getBlock().getLetter());
		blockText.setCharacterSize(locationTextSize);
		blockText.setFillColor(sf::Color::White);
		blockText.setStyle(sf::Text::Bold);
		blockText.setPosition(arm1BlockTextXPos, arm1BlockTextYStartPos + (frameSize * drawStates.size()));
		state.addText(blockText);
	}
}

void drawArm2Block(sf::Font& font, Arm arm, DrawState& state)
{
	if (arm.getBlock().getLetter() != "")
	{
		sf::RectangleShape rectangle(sf::Vector2f(blockSize, blockSize));
		rectangle.setFillColor(sf::Color::Magenta);
		rectangle.setOutlineColor(sf::Color::White);
		rectangle.setOutlineThickness(blockOutlineThickness);
		rectangle.setPosition(arm2BlockXPos, arm2BlockYStartPos + (frameSize * drawStates.size()));
		state.addRectangle(rectangle);

		sf::Text blockText;
		blockText.setFont(font);
		blockText.setString(arm.getBlock().getLetter());
		blockText.setCharacterSize(locationTextSize);
		blockText.setFillColor(sf::Color::White);
		blockText.setStyle(sf::Text::Bold);
		blockText.setPosition(arm2BlockTextXPos, arm2BlockTextYStartPos + (frameSize * drawStates.size()));
		state.addText(blockText);
	}
}

void drawArm1Action(sf::Font& font, DrawState& state, string str)
{
	sf::Text actionText;
	actionText.setFont(font);
	actionText.setString(str);
	actionText.setCharacterSize(armActionTextSize);
	actionText.setFillColor(sf::Color::Magenta);
	actionText.setStyle(sf::Text::Bold);
	actionText.setPosition(armActionTextXPos, arm1ActionTextYStartPos + (frameSize * drawStates.size()));
	state.addText(actionText);
}

void drawArm2Action(sf::Font& font, DrawState& state, string str)
{
	sf::Text actionText;
	actionText.setFont(font);
	actionText.setString(str);
	actionText.setCharacterSize(armActionTextSize);
	actionText.setFillColor(sf::Color::Magenta);
	actionText.setStyle(sf::Text::Bold);
	actionText.setPosition(armActionTextXPos, arm2ActionTextYStartPos + (frameSize * drawStates.size()));
	state.addText(actionText);
}
void drawState(sf::RenderWindow& window, DrawState state)
{
	for (sf::CircleShape i : state.getTriangles())
	{
		window.draw(i);
	}

	for (sf::RectangleShape i : state.getRectangles())
	{
		window.draw(i);
	}

	for (sf::Text i : state.getTexts())
	{
		window.draw(i);
	}
}

void drawAllStates(sf::RenderWindow& window, vector<DrawState> drawStates)
{
	for (DrawState i : drawStates)
	{
		drawState(window, i);
	}
}