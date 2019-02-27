#pragma once

#include "Maze.h"
#include "Player.h"



class MazeGame : public olc::PixelGameEngine {
public:
	MazeGame();
	~MazeGame();

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

	void drawBGandString(olc::PixelGameEngine*, int, int, std::string, olc::Pixel);
	void drawParams();


public:
	bool OnUserCreate() override;
	bool OnUserUpdate(float) override;
};

