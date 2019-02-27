#pragma once

#include <vector>
#include "Maze.h"

typedef struct sPath
{
	float x;
	float y;
	float angle;
} path;


class Player
{
public:
	Player(Maze*);
	~Player();

private:
	Maze *maze;
	Location p;
	float fxPos;
	float fyPos;
	float fxPosOld;
	float fyPosOld;
	float fAngle;
	float fxVec;          // x vector if fAngle
	float fyVec;          // y vector of fAngle
	float fx90leftVec;    // sidestep left
	float fx90rightVec;   // sidestep right
	float fUserSize;
	std::vector<path> vecPath; // save the path the user moved around

public:
	float getAngle() const;
	Location getPos() const;
	int getPathLength() const;
	void setRandomPosition();

public:
	void rotate(float);
	void move(float);
	void moveSide(float, bool);

private:
	bool collisionDetection();

public:
	void draw2DPath(olc::PixelGameEngine*);
	void draw2D(olc::PixelGameEngine*);
	void draw3D(olc::PixelGameEngine*, Maze*);
};

