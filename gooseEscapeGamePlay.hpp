/*
FILE NAME: gooseEscapeGamePlay.hpp
Edited by: Ethan Dau & Thomas Kim
*/
#ifndef GOOSE_ESCAPE_GAMEPLAY
#define GOOSE_ESCAPE_GAMEPLAY
#include <cmath>
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActor.hpp"
#include "gooseEscapeConsole.hpp"

/*
    This file is all about the game world.  You will modify this to add
    constants and function prototypes.  Modify gooseGamePlay.cpp to
	actually add functionality.
*/

/*
    Declare constants to indicate various game world features in the board
    array.  Modify them to fit what you would like to do in the game.
*/

// Going further:  Learn how to use an enum for these values
const int EMPTY = 0;
const int SHALL_NOT_PASS = 1;
const int WINNER = 2;
const int NUM_ROW = 21;
const int NUM_COL = 80;
const int WALL_LENGTH = 10;
const double THIRTY_DEG = M_PI / 6;
const double SIXTY_DEG = M_PI / 3;
const int BARRIER = 3;
const int BUTTON = 4;
const int BUTTON_USED = 5;
const int WALL_REGEN = 6;
const int MAX_BUTTONS = 10;

/*
    A few examples of characters both for actors and for the game board itself
    are shown.
*/
//display characters
const int PLAYER_CHAR = int('@');
const int MONSTER_CHAR = int('G');
const int WALL_CHAR = int('O');
const int WIN_CHAR = int('%'); //% sign, a special character used in the ancient game "Rogue"
const int BARRIER_CHAR = int('Q');
const int BUTTON_CHAR = int('B');
const int BUTTON_USED_CHAR = int ('_');
const int WALL_REGEN_CHAR = int ('R');

/*
Colours have the format "brightness hue" or "hue", e.g. "white" "light green"
Values for brightness are: lightest, lighter, light, dark, darker, darkest
Hue may be specified in many ways, see http://foo.wyrd.name/en:bearlibterminal:reference
  hue names are: grey, gray, red, flame, orange, amber, yellow, lime, chartreuse
                 green, sea, turquoise, cyan, sky, azure, blue, han, violet,
                 purple, fushsia, magenta, pink, crimson, transparent
*/
/*
  Declare a character array with the colour name, the size is set based on the
  number of character used to initialize the array.
  
  DRAW_COLOUR is already defined as white.
*/

const char PLAYER_COLOUR[] = "light sky";
const char GOOSE_COLOUR[] = "yellow";
const char DEFAULT_COLOUR[] = "white";
const char WIN_COLOUR[] = "purple";
const char BARRIER_COLOUR[] = "red";
const char REGEN_COLOUR[] = "chartreuse";

//Given & fixed functions:

//Print the game world
void clearMap(int map[NUM_ROW][NUM_COL]);

void printMap(int map[NUM_ROW][NUM_COL]);

void printWall(int map[NUM_ROW][NUM_COL]);


//Do something when the goose captures the player
bool captured(Actor const & player, Actor const & monster);

//Move the player to a new location based on the user input
void movePlayer(int key, Actor & player, int map[NUM_ROW][NUM_COL]);


//Our added functions:

//Detect if the player has won
bool hasWon(Actor const & player, int winX, int winY);

// random wall functions

bool randomBool();

int randomNumber(bool isHorizontal);

//Random position generator
int RandomPosition(bool isHorizontal);

//Moving the goose functions: See .cpp version for explanation
bool isStupid(int stupidityFactor);

void stupidMove(int & moveX, int & moveY);

void MonsterPlayerDist(Actor const & player, Actor const & monster, 
					   double & distanceX, double & distanceY);
void moveMonsterDirections(double distanceY, double distanceX, Actor const & player, 
						   Actor const & monster, int & moveY, int & moveX, int stupidityFactor);
			   
void moveMonster(Actor const & player, Actor & monster, int map[NUM_ROW][NUM_COL],
				 int stupidityFactor);

void updateMap(int map[NUM_ROW][NUM_COL], Actor const & player, Actor & monster);

//Anything below are new feature functions
void loadText(ifstream & fin, int text[NUM_ROW][NUM_COL]);

void loadBarrier(bool barrierExists, int map[NUM_ROW][NUM_COL], int winX, int winY,
				 int numButtons, int buttonX[MAX_BUTTONS], int buttonY[MAX_BUTTONS]);

void checkButton(int map[NUM_ROW][NUM_COL], Actor & player, int winX, int winY,
				 int numButtons, int buttonX[MAX_BUTTONS], int buttonY[MAX_BUTTONS],
				 bool & allButtonsActivated);

void barrierRelease(int map[NUM_ROW][NUM_COL], int winX, int winY);

void regenWalls(int map[NUM_ROW][NUM_COL], int numWalls, Actor const & Player);

#endif
