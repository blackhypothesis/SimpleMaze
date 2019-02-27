#include <iostream>
#include <vector>


#include "Maze.h"
#include "MazeGame.h"

void drawBGandString(olc::PixelGameEngine *engine, int x, int y, std::string s, olc::Pixel p)
{
	int deltaX = 910;
	int deltaY = 570;
	int fontSize = 34;

	int xpos = deltaX + x * fontSize;
	int ypos = deltaY + y * fontSize;

	engine->FillRect(xpos, ypos, (s.length() + 2) * fontSize, fontSize, olc::YELLOW);
	engine->DrawString(xpos + 2, ypos + 2, s, p, 4);
}

int main()
{
	std::cout << "Creating Maze, ..." << std::endl;
	MazeGame maze;
	std::cout << "Maze created" << std::endl;


	if (maze.Construct(1700, 900, 1, 1))
		maze.Start();
	return 0;

	return 0;
}