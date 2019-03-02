#pragma once

#include "Maze.h"
#include "Player.h"

class MazeGameSFML
{
public:
	MazeGameSFML();
	~MazeGameSFML();

private:
	int nLengthCorridor;
	int nMazeWidth;
	int nMazeHeight;
	Maze maze;
	Player player;
	float fAngle;
	bool bPlayerInteraction;
	bool bDraw2D;
	float fDistanceToEnd;
	int mouseX;
	int mouseY;
	int mouseOldX;
	int mouseOldY;

public:
	bool GameInit();
	bool GameLoop();

};

