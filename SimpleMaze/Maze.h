#pragma once

#include <vector>
#include <memory>

#include "olcPixelGameEngine.h"
#include "SFML/Graphics.hpp"

#include "Block.h"
#include "Location.h"

class Maze
{
public:
	enum Direction { NORTH, WEST, SOUTH, EAST };

public:
	Maze();
	Maze(int, int);

private:
	std::vector<std::unique_ptr<Block>> vecMaze;
	int nBlockSize;
	int xDraw, yDraw;
	int xSize, ySize;
	bool bFirstCorridor;
	olc::Pixel floorColor;
	std::vector<sf::RectangleShape> vecRect;

public:
	int getXSize() const;
	int getYSize() const;

	const std::vector<std::unique_ptr<Block>>& getMaze() const;
	const std::unique_ptr<Block>& getBlock(int, int) const;
	int getIndex(int, int) const;
	int getBlockSize() const;
	// do not use it, use instead getBlock(int, int)->getBlockType()
	Block::BlockType getBlockType(int x, int y) const;

	float getRatio() const;
	olc::Pixel getFloorColor() const;
	Location getEnd() const;

	// ------- methods to create maze
	// ------------------------------
	void initMaze();
	void createCorridor(int);
	void connectCorridors();
	void createRoom();
	void createWall2();
	void createEnd();
	void getVec2DSFML();

	Location getRandomFreeLocation();

	void createRandomMaze(int);
	bool isFree(int x, int y);

	void drawGrid(olc::PixelGameEngine*);	
	void draw2D(olc::PixelGameEngine*);
	void draw2DVisited(olc::PixelGameEngine*);

	void draw2DSFML(sf::RenderWindow* window);

};

