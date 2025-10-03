#include "pch.h"

#include "Relation.h"

/*this class represents an individual relation. 
It consists of an adjective describing how two blocks are related to each other, or how a single block is related to the world. 
The empty relation contains no blocks. It was added to be a placeholder for an empty table location since vector comparison operations 
(i.e. comparing states in a vector) if one vector is empty is not allowed.
*/

Relation::Relation()
{

}

Relation::Relation(RelationName name, Block block1, Block block2)
{
	this->name = name;
	this->block1 = block1;
	this->block2 = block2;
}

Relation::~Relation()
{
	
}

/*this method allows for the first block of relation to be changed without having to delete a relation object and create a new one. For example, 
if one block is put on top of another one you would still need the clear relation, but the block would just change to the new top block*/
void Relation::setFirstBlock(Block b)
{
	this->block1 = b;
}

/*a relation is considered equal if it has the same name and blocks associated with it in the proper order.*/
bool Relation::operator==(const Relation& rhs) const
{
	return (this->name == rhs.name) && (this->block1 == rhs.block1) && (this->block2 == rhs.block2);
}

/*this operator is used by the search algorithm when sorting list of relations to determine which blocks are in each stack and how many are in each stack. 
For example, my method orders the relations by their name in the order of TABLE, ON, CLEAR, ABOVE, EMPTY and then orders the relations based on one are not the 
first and second blocks are equal. This is helpful for counting blocks because you only have to count the table and on relations to determine the number blocks 
in the stack. It also allows you to determine ordering. For example if I had the relations (ON, b, a) and (ON, c, b) it is much easier to determine what 
that stack looks like and compare it to other stacks because layering a natural ordering. In the example above b is on a and c is on b.*/
bool Relation::operator<(const Relation& rhs) const
{
	if (this->name == rhs.name)
	{
		if (this->block1.getLetter() == rhs.block2.getLetter())
		{
			return true;
		}
		else
			return false;
	}
	else
		return this->name < rhs.name;
}

bool Relation::operator()(const Relation & lhs, const Relation & rhs)
{
	if (lhs.name == rhs.name)
	{
		if (lhs.block1.getLetter() == rhs.block1.getLetter())
		{
			return lhs.block2.getLetter() < rhs.block2.getLetter();
		}
		else
			return lhs.block1.getLetter() < rhs.block1.getLetter();
	}
	else
		return lhs.name < rhs.name;
}