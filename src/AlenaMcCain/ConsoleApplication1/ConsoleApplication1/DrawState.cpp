#include "pch.h"

#include "DrawState.h"

/*this class represents a state court graphical user interface. 
For the graphical user interface state merely contains lists of shapes that need to be drawn, and the methods to add new shapes to the lists*/

DrawState::DrawState()
{
}

DrawState::~DrawState()
{
}

void DrawState::addRectangle(sf::RectangleShape rectangle)
{
	rectangles.push_back(rectangle);
}

void DrawState::addText(sf::Text text)
{
	texts.push_back(text);
}

void DrawState::addTriangle(sf::CircleShape triangle)
{
	triangles.push_back(triangle);
}