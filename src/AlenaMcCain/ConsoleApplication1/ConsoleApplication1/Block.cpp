#include "pch.h"

#include "Block.h"

/*this class represents a block object. A block simply contains its designated letter.*/

Block::Block()
{

}

Block::Block(string letter)
{
	this->letter = letter;
}

Block::~Block()
{
}

/*a block is considered equal to another block is that both have the same letter.*/
bool Block::operator==(const Block& rhs) const
{
	return this->letter == rhs.letter;
}