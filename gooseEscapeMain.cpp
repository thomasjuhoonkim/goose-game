/*
FILE NAME: gooseEscapeMain.cpp
Edited by: Ethan Dau & Thomas Kim
*/
#include <BearLibTerminal.h>
#include <cmath>
#include <ctime>
#include <iostream> 
#include <fstream>
using namespace std;
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActor.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"

//set up the console.   Don't modify this line!
Console out;

int main()
{    
	// Set up for using rand() to generate random numbers (can be deleted if random numbers not used)
	srand(time(NULL));

	//Set up the window.  Don't edit these two lines
    terminal_open();
  	terminal_set(SETUP_MESSAGE);

	//Declaring the gameboard
	int map[NUM_ROW][NUM_COL] = {EMPTY};
	int youWin[NUM_ROW][NUM_COL] = {EMPTY};    //Array for if the player wins
	int youLose[NUM_ROW][NUM_COL] = {EMPTY};   //Array for if the player loses
	
	//"settings.txt" contains data for # of walls, win position, 
	//barrier existance & qty, player position, monster position, 
	//stupidity factor
	//"you_win.txt" & "you_lose.txt" are for printing the large messages at the
	//end of the game
	ifstream finSettings("settings.txt");
	ifstream finYouWin("you_win.txt");
	ifstream finYouLose("you_lose.txt");
	if (!finSettings)
	{
		cout << "settings.txt did not open correctly.";
		return EXIT_FAILURE;
	}
	if (!finYouWin)
	{
		cout << "you_win.txt did not open correctly.";
		return EXIT_FAILURE;
	}
	if (!finYouLose)
	{
		cout << "you_lose.txt did not open correctly.";
		return EXIT_FAILURE;
	}
	
	//Reading in file data and closing files
	int numWalls = 0;
	int winX = 0, winY = 0;
	bool barrierExists = 0; int numButtons = 0;
	int playerX = 0, playerY = 0, playerHealth;
	int monsterX = 0, monsterY = 0, monsterHealth;
	int stupidityFactor = 0;
	int movesBeforeWall = 0;
	finSettings >> numWalls
				>> winX >> winY
				>> barrierExists >> numButtons
				>> playerX >> playerY >> playerHealth
				>> monsterX >> monsterY >> monsterHealth
				>> stupidityFactor
				>> movesBeforeWall;
	loadText(finYouWin, youWin);
	loadText(finYouLose, youLose);
	finYouWin.close();
	finYouLose.close();
	finSettings.close();

	int buttonX[MAX_BUTTONS] = {0}, buttonY[MAX_BUTTONS] = {0};
	bool allButtonsActivated = false;
	
	//Filling in & printing map
	for (int index = 0; index < numWalls; index++)
		printWall(map);
	map[winY][winX] = WINNER;
	map[RandomPosition(false)][RandomPosition(true)] = WALL_REGEN;
	loadBarrier(barrierExists, map, winX, winY, numButtons, buttonX, buttonY);
    printMap(map);
    
    //Setting up player/goose
	Actor player(PLAYER_CHAR, playerX,playerY, playerHealth, PLAYER_COLOUR);
	Actor monster(MONSTER_CHAR, monsterX, monsterY, monsterHealth, GOOSE_COLOUR);
    
	// Printing the instructions in the console window
    out.writeLine("Escape the Goose! " + monster.get_location_string());
	out.writeLine("Use the arrow keys to move. If the goose catches you, you lose!");
	out.writeLine("You must press all 'B' to unlock the win area. Press 'R' to reset walls");
	out.writeLine("Be careful! Sometimes the goose can jump through walls!");

//Main game loop

    int keyEntered = TK_A, moveCount = 0; 
    
    while(keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE 
                    && !captured(player,monster) && !hasWon(player, winX, winY))
	{
	    // get player key press
	    keyEntered = terminal_read();

        if (keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE)
        {
            // move the player and record the move
    	    movePlayer(keyEntered,player,map);
    	    moveCount++;
    	    
    	    //Add an extra wall if necessary, based on the move count. Reset the
    	    //move count afterwards
    	    if(moveCount == movesBeforeWall)
    	    {
    	    	printWall(map);
    	    	moveCount = 0;
    	    }
    	    
    	    //Restore default walls if player presses the R tile
    	    if(map[player.get_y()][player.get_x()] == WALL_REGEN)
    	    	regenWalls(map, numWalls, player);
    	    
    	    //Check if button has been pressed
			if (allButtonsActivated == false)
			{
    	    checkButton(map, player, winX, winY, numButtons, buttonX, buttonY,
						allButtonsActivated);
			}
			    
    	    // move the goose
			moveMonster(player, monster, map, stupidityFactor);
    	}
  	}

    if (keyEntered != TK_CLOSE)
    {
      	//once we're out of the loop, the game is over
        out.writeLine("Game has ended");
    
        // Output reason
		if (hasWon(player, winX, winY) == true)
		{
			clearMap(map);
			terminal_color(PLAYER_COLOUR);
			printMap(youWin);
			out.writeLine("You Win");
		}
		else
		{
			clearMap(map);
			terminal_color(GOOSE_COLOUR);
			printMap(youLose);
			out.writeLine("You Lose");
		}
	
    	// Wait until user closes the window
        while (terminal_read() != TK_CLOSE);
    }

	//game is done, close it
    terminal_close();
}
