#pragma once

#include <SFML/Graphics.hpp>

#include "Data.h"
#include "Block.h"
#include "Arm.h"
#include "DrawState.h"

/*this file contains all the methods necessary for storing all the objects that draw the graphical user interface.*/

extern vector<DrawState> drawStates;

extern void insertLag();

extern void drawTable(sf::Font& font, DrawState& state);

extern void drawBlockOnTable(sf::Font& font, Block block, Location location, int pos, DrawState& state);

extern void drawL1Blocks(sf::Font& font, DrawState& state);

extern void drawL2Blocks(sf::Font& font, DrawState& state);

extern void drawArms(sf::Font& font, DrawState& state);

extern void drawArm1Action(sf::Font& font, DrawState& state, string str);

extern void drawArm2Action(sf::Font& font, DrawState& state, string str);

extern void drawArm1Block(sf::Font& font, Arm arm, DrawState& state);

extern void drawArm2Block(sf::Font& font, Arm arm, DrawState& state);

extern void drawState(sf::RenderWindow& window, DrawState state);

extern void drawAllStates(sf::RenderWindow& window, vector<DrawState> drawStates);