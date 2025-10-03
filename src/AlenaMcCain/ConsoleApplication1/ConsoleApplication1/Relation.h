#pragma once

#include <string>
#include <vector>

#include "Block.h" 

/*this class represents an individual relation. It consists of an adjective describing how two blocks are related to each other, 
or how a single block is related to the world. The empty relation contains no blocks. It was added
to be a placeholder for an empty table location since vector comparison operations (i.e. comparing states in a vector) 
if one vector is empty is not allowed.*/

using namespace std;

typedef enum RelationName { TABLE, ON, CLEAR, ABOVE, EMPTY } RelationName;

class Relation
{
public:
	Relation();
	Relation(RelationName name, Block block1, Block block2);
	~Relation();

	RelationName getName() const { return name; }
	Block getFirstBlock() const { return block1; }
	Block getSecondBlock() const { return block2; }

	void setFirstBlock(Block b);

	bool operator==(const Relation& rhs) const;

	bool operator<(const Relation& rhs) const;

	bool operator()(const Relation & lhs, const Relation & rhs);

private:
	RelationName name; //Type of relation
	Block block1; //First block of the relation
	Block block2; //Second block of the relation 
};