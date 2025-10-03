#pragma once

#include <vector>

#include "Block.h"
#include "Relation.h"

using namespace std;

/*this file contains a global variables that are necessary for the execution of the program like location and action enums for the arms and blocks, 
graphical user interface drawing positions, and vectors used for obtaining program input and maintaining the graphical user interface*/

typedef enum Location { L1, L2 } Location;
typedef enum Action {PICK_UP, PUT_DOWN, STACK, UNSTACK, MOVE, NOOP} Action;

//Execution
extern float delay;

//General Draw
extern float frameSize;

//Table
extern float tableLength;
extern float tableWidth;
extern float tableXPos;
extern float tableYStartPos;

extern int locationTextSize;
extern float L1TextXPos;
extern float L2TextXPos;
extern float LTextYStartPos;

//Block
extern float blockSize;
extern float blockOutlineThickness;
extern float LBlockYStartPos;
extern float L1BlockXPos;
extern float L2BlockXPos;
extern int blockTextSize;
extern float L1BlockTextXPos;
extern float L2BlockTextXPos;
extern float LBlockTextYStartPos;

//Robot Arm
extern float armSize;
extern float armOutlineThickness;
extern float armL1XPos;
extern float armL2XPos;
extern float arm1YStartPos;
extern float arm2YStartPos;
extern float arm1XStartPos;
extern float arm2XStartPos;

//Block in robot arm
extern float arm1BlockXPos;
extern float arm2BlockXPos;
extern float arm1BlockYStartPos;
extern float arm2BlockYStartPos;
extern int armBlockTextSize;
extern float arm1BlockTextYStartPos;
extern float arm2BlockTextYStartPos;
extern float arm1BlockTextXPos;
extern float arm2BlockTextXPos;

//Robot arm actions
extern int armActionTextSize;
extern float arm1ActionTextYStartPos;
extern float arm2ActionTextYStartPos;
extern float armActionTextXPos;

//Lists
extern vector<Block> L1Vec;
extern vector<Block> L2Vec;

extern vector<Block> L1VecEnd;
extern vector<Block> L2VecEnd;