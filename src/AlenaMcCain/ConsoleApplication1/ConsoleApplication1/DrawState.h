#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;
/*this class represents a state court graphical user interface. 
For the graphical user interface state merely contains lists of shapes that need to be drawn, and the methods to add new shapes to the lists*/
class DrawState
{
public:
	DrawState();
	~DrawState();
	vector<sf::RectangleShape> getRectangles() { return rectangles; }
	vector<sf::Text> getTexts() { return texts; }
	vector<sf::CircleShape> getTriangles() { return triangles; }

	void addRectangle(sf::RectangleShape rectangle);

	void addText(sf::Text text);

	void addTriangle(sf::CircleShape triangle);

private:
	vector<sf::RectangleShape> rectangles;
	vector<sf::Text> texts;
	vector<sf::CircleShape> triangles;
};
