#pragma once

#include <string>

using namespace std;
/*this class represents a block object. A block simply contains its designated letter.*/

class Block
{
public:
	Block();
	Block(string letter);
	~Block();
	string getLetter() const { return letter; }
	bool operator==(const Block& rhs) const;

private:
	string letter; //The letter of the block
};