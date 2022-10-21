/*
FILE NAME: gooseEscapeGamePlay.cpp
Edited by: Ethan Dau & Thomas Kim
*/
#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;
#include <BearLibTerminal.h>
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActor.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"

extern Console out;

//Clear the game world
void clearMap(int map[NUM_ROW][NUM_COL])
{
	for (int row = 0; row < NUM_ROW; row++)
	{
		for (int col = 0; col < NUM_COL; col++)
			terminal_put(col, row, ' ');
	}
}

//Print the game world
void printMap(int map[NUM_ROW][NUM_COL])
{
	for (int row = 0; row < NUM_ROW; row++)
	{
		for (int col = 0; col < NUM_COL; col++)
		{
			if (map[row][col] == SHALL_NOT_PASS)
				terminal_put(col, row, WALL_CHAR);
			if (map[row][col] == WINNER)
			{
				terminal_color(WIN_COLOUR);
				terminal_put(col, row, WIN_CHAR);
				terminal_color(DEFAULT_COLOUR);
			}
			if (map[row][col] == BARRIER)
			{
				terminal_color(BARRIER_COLOUR);
				terminal_put(col, row, BARRIER_CHAR);
				terminal_color(DEFAULT_COLOUR);
			}
			if (map[row][col] == BUTTON)
			{
				terminal_color(BARRIER_COLOUR);
				terminal_put(col, row, BUTTON_CHAR);
				terminal_color(DEFAULT_COLOUR);
			}
			if (map[row][col] == BUTTON_USED)
			{
				terminal_color(BARRIER_COLOUR);
				terminal_put(col, row, BUTTON_USED_CHAR);
				terminal_color(DEFAULT_COLOUR);
			}
			if (map[row][col] == WALL_REGEN)
			{
				terminal_color(REGEN_COLOUR);
				terminal_put(col, row, WALL_REGEN_CHAR);
				terminal_color(DEFAULT_COLOUR);
			}
		}
	}
	terminal_refresh();
}

void printWall(int map[NUM_ROW][NUM_COL])
{
	const bool HORIZONTAL = true, VERTICAL = false;
	bool isHorizontal = randomBool(); //true = horizontal
	int wallX = randomNumber(HORIZONTAL), wallY = randomNumber(VERTICAL);
	for (int index = 0; index < WALL_LENGTH; index++)
	{
		if (isHorizontal == true && map[wallY][wallX + index] == EMPTY)
			map[wallY][wallX + index] = SHALL_NOT_PASS;
		else if(isHorizontal == false && map[wallY + index][wallX] == EMPTY)
			map[wallY + index][wallX] = SHALL_NOT_PASS;
	}
}

//Do something when the goose captures the player
bool captured(Actor const & player, Actor const & monster)
{
    return (player.get_x() == monster.get_x() 
         && player.get_y() == monster.get_y());
}

//Move the player to a new location based on the user input
void movePlayer(int key, Actor & player, int map[NUM_ROW][NUM_COL])
{
    int yMove = 0, xMove = 0;
    if (key == TK_UP)
        yMove = -1;
    else if (key == TK_DOWN)
        yMove = 1;
    else if (key == TK_LEFT)
        xMove = -1;
    else if (key == TK_RIGHT)
        xMove = 1;
        
    if (player.can_move(xMove, yMove) 
      && map[player.get_y() + yMove][player.get_x() + xMove] != SHALL_NOT_PASS
	  && map[player.get_y() + yMove][player.get_x() + xMove] != BARRIER)
        player.update_location(xMove, yMove);
}

//Our added functions are below!

//Detect if the player has won
bool hasWon(Actor const & player, int winX, int winY)
{
	if (player.get_x() == winX && player.get_y() == winY)
		return true;
	else
		return false;
}

/*
The following functions are responsible for generating randomized walls.
When the printWall function is called, a random bool is assigned to determine
the wall orientation. Once the orientation is assigned, a random number generator
assigns the location of the wall to begin ranging from 0 to 60 horizontally or 0 to 11 vertically.
Then, 10 SHALL_NOT_PASS integers are assigned to the matrix and is outputted.
*/

bool randomBool()
{
	return rand() % 2;
}

int randomNumber(bool isHorizontal)
{
	int highBound = 0;
	if (isHorizontal == true)
		highBound = NUM_COL - WALL_LENGTH;
	else
		highBound = NUM_ROW - WALL_LENGTH;
	return rand() % highBound;
}

/*
The following function picks a random x or y position somewhere within the bounds of 
the gameboard. The bounds are dependent on boolean "isHorizontal" This function
is used for randomly moving the regen symbol around
*/
int RandomPosition(bool isHorizontal) 
{
	if(isHorizontal)
		return rand() % NUM_COL;
	else
		return rand() % NUM_ROW;
}

/*
The following functions are responsible for moving the goose. The goose moves towards
the player depending on the angle of the arctan of the distance between the entities.

If the angle is > 60 degrees, move in y direction
If the angle is 30 < angle < 60 degrees, move in random y or x that is closer to player
If the angle is < 30 degrees, move in x direction

In addition, there is a "stupidity" factor in the range of 0-100 that determines how stupid
the monster is. For example, if the goose is 70% stupid, the goose will make a stupid move
70% of the time. If the goose is 0% stupid, the goose will never make a stupid move.
A stupid move is when the goose moves in an absolutely random direction with magnitude 1.
*/

//Determines current x & y distance btwn. monster and player
void MonsterPlayerDist(Actor const & player, Actor const & monster, 
					   double & distanceX, double & distanceY)
{
	distanceX = player.get_x() - monster.get_x();
	distanceY = player.get_y() - monster.get_y();
}

/*
Chooses appropriate x or y direction to move the monster
If the angle is > 60 degrees, move in y direction
If the angle is 30 < angle < 60 degrees, move in random y or x that is closer to player
If the angle is < 30 degrees, move in x direction
*/
void moveMonsterDirections(double distanceY, double distanceX, Actor const & player, 
						   Actor const & monster, int & moveY, int & moveX, int stupidityFactor)
{
	if (isStupid(stupidityFactor) == true)
		stupidMove(moveX, moveY);
	else
	{
		if (atan(abs(distanceY / distanceX)) > SIXTY_DEG)
			moveY = copysign(1, distanceY);
		else if (atan(abs(distanceY / distanceX)) <= SIXTY_DEG
				 && atan(abs(distanceY / distanceX)) >= THIRTY_DEG)
		{
			if (randomBool() == true)
				moveY = copysign(1, distanceY);
			else
				moveX = copysign(1, distanceX);
		}
		else
			moveX = copysign(1, distanceX);	
	}
}

//Uses 4 functions above to move the monster to the next spot
void moveMonster(Actor const & player, Actor & monster, int map[NUM_ROW][NUM_COL],
				 int stupidityFactor)
{	
	double distanceX = 0, distanceY = 0;
	int moveX = 0, moveY = 0;
	MonsterPlayerDist(player,monster,distanceX,distanceY);
	moveMonsterDirections(distanceY,distanceX,player,monster,moveY,moveX,stupidityFactor);
	
	monster.update_location(moveX,moveY);
	updateMap(map, player, monster);
}

//Updates the map by printing entire map excluding the pieces where the goose is currently at
void updateMap(int map[NUM_ROW][NUM_COL], Actor const & player, Actor & monster)
{
	printMap(map);		
	if (map[monster.get_y()][monster.get_x()] != EMPTY)
	{
		terminal_color(GOOSE_COLOUR);
		terminal_put(monster.get_x(), monster.get_y(), MONSTER_CHAR);
		terminal_color(DEFAULT_COLOUR);	
	}
	//In case a player spawns in a wall
	if (map[player.get_y()][player.get_x()] != EMPTY)
	{
		terminal_color(PLAYER_COLOUR);
		terminal_put(player.get_x(), player.get_y(), PLAYER_CHAR);
		terminal_color(DEFAULT_COLOUR);	
	}
	terminal_refresh();
}



//Anything below are new feature functions as of week 2

//Loads the text for the winning and losing screen
void loadText(ifstream & fin, int text[NUM_ROW][NUM_COL])
{
	for (int row = 0; row < NUM_ROW; row++)
	{
		for (int col = 0; col < NUM_COL; col++)
			fin >> text[row][col];
	}
}

//A stupidity factor determines the probability of the monster making a stupid move.
//true = stupid move
bool isStupid(int stupidityFactor)
{
	//Generates random number from 1 to 100, if 0, the goose got perfect on PHYS115
	const int MAX_STUPIDITY = 100;
	int randomNum = rand() % MAX_STUPIDITY + 1;
	
	if (randomNum <= stupidityFactor)
		return true;
	else
		return false;
}

//Makes monster do a stupid move.
void stupidMove(int & moveX, int & moveY)
{
	int randomNum3 = rand() % 3;
	if (randomNum3 == 0)
	{
		if (randomBool() == true)
			moveY = 1;
		else
			moveY = -1;
	}
	else if (rand() % 3 == 1)
	{
		if (randomBool() == true)
			moveX = 1;
		else
			moveX = -1;
	}
	//When randomNum3() == 2, default moveX = 0, and moveY = 0 will happen.
}

//Loads in barrier around the win piece and generates random buttons to release the barrier
void loadBarrier(bool barrierExists, int map[NUM_ROW][NUM_COL], int winX, int winY,
				 int numButtons, int buttonX[MAX_BUTTONS], int buttonY[MAX_BUTTONS])
{
	if (barrierExists == true)
	{
		for (double index = 0; index < 2 * M_PI; index += M_PI / 10)
		{
			const int RADIUS = 3;
			int deltaY = round(RADIUS * sin(index)), deltaX = round(RADIUS * cos(index));
			map[winY + deltaY][winX + deltaX] = BARRIER;
		}
		for (int index = 0; index < numButtons; index++)
		{
			bool buttonLocationFound = false;
			while (buttonLocationFound == false)
			{
				buttonY[index] = rand() % NUM_ROW;
				buttonX[index] = rand() % NUM_COL;
				if (map[buttonY[index]][buttonX[index]] == EMPTY)
				{
					const int RADIUS = 3;
					double distanceY = abs(winY - buttonY[index]);
					double distanceX = abs(winX - buttonX[index]);
					double distance = sqrt(pow(distanceY,2) + pow(distanceX,2));
					if (distance > RADIUS)
						buttonLocationFound = true;
				}
			}
			map[buttonY[index]][buttonX[index]] = BUTTON;
		}
	} 
}

//Checks if the player has hit the button after every player move
void checkButton(int map[NUM_ROW][NUM_COL], Actor & player, int winX, int winY,
				 int numButtons, int buttonX[MAX_BUTTONS], int buttonY[MAX_BUTTONS],
				 bool & allButtonsActivated)
{
	int buttonActivations = 0;
	for (int index = 0; index < numButtons; index++)
	{
		if (player.get_x() == buttonX[index] && player.get_y() == buttonY[index])
			map[buttonY[index]][buttonX[index]] = BUTTON_USED;
		if (map[buttonY[index]][buttonX[index]] == BUTTON_USED)
		{
			buttonActivations++;
			if (buttonActivations == numButtons)
				allButtonsActivated = true;
		}
	}
	if (allButtonsActivated == true)
	{
		barrierRelease(map, winX, winY);
		clearMap(map);
		printMap(map);
	}
}

//Releases the barrier if checkButton(); has detected that the player clicked the button.
void barrierRelease(int map[NUM_ROW][NUM_COL], int winX, int winY)
{
	for (double index = 0; index < 2 * M_PI; index += M_PI / 10)
	{
		const int RADIUS = 3;
		int deltaY = round(RADIUS * sin(index)), deltaX = round(RADIUS * cos(index));
		map[winY + deltaY][winX + deltaX] = EMPTY;
	}
}

//Resets walls to original count. Used when player presses "R" tile
void regenWalls(int map[NUM_ROW][NUM_COL], int numWalls, Actor const & Player)
{
	for(int row = 0; row < NUM_ROW; row++)
		for(int col = 0; col < NUM_COL; col++)
			if(map[row][col] == SHALL_NOT_PASS || map[row][col] == WALL_REGEN)
				map[row][col] = EMPTY;
	clearMap(map);
	for(int index = 0; index < numWalls; index++)
		printWall(map);
	map[RandomPosition(false)][RandomPosition(true)] = WALL_REGEN;
	printMap(map);
	Player.put_actor();
}
