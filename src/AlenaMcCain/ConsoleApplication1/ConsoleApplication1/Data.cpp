#include "pch.h"

#include "Data.h"
/*this file contains a global variables that are necessary for the execution of the program like location and action enums for the arms and blocks, 
graphical user interface drawing positions, and vectors used for obtaining program input and maintaining the graphical user interface
this file contains all the initializations of the variables in the data header file*/

float delay = 3;

//General Draw
float frameSize = 1400;

//Table
float tableLength = 1200;
float tableWidth = 50;
float tableXPos = 420;
float tableYStartPos = 840;

int locationTextSize = 30;
float L1TextXPos = 720;
float L2TextXPos = 1320;
float LTextYStartPos = 850;

//Block
float blockSize = 50;
float blockOutlineThickness = 2;
float LBlockYStartPos = 790;
float L1BlockXPos = 700;
float L2BlockXPos = 1300;
int blockTextSize = 30;
float L1BlockTextXPos = 715;
float L2BlockTextXPos = 1315;
float LBlockTextYStartPos = 800;

//Robot Arm
float armSize = 70;
float armOutlineThickness = 10;
float armL1XPos = 700;
float armL2XPos = 1300;
float arm1YStartPos = 15;
float arm2YStartPos = 145;
float arm1XStartPos = armL1XPos;
float arm2XStartPos = armL2XPos;

//Block in robot arm
float arm1BlockXPos = arm1XStartPos + 45;
float arm2BlockXPos = arm2XStartPos + 45;
float arm1BlockYStartPos = arm1YStartPos + 40;
float arm2BlockYStartPos = arm2YStartPos + 40;
int armBlockTextSize = 30;
float arm1BlockTextYStartPos = arm1BlockYStartPos + 10;
float arm2BlockTextYStartPos = arm2BlockYStartPos + 10;
float arm1BlockTextXPos = arm1XStartPos + 60;
float arm2BlockTextXPos = arm2XStartPos + 60;

//Robot arm actions
int armActionTextSize = 35;
float arm1ActionTextYStartPos = 565;
float arm2ActionTextYStartPos = 715;
float armActionTextXPos = 30;