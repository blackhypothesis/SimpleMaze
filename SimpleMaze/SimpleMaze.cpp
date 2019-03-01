#include <iostream>
#include <vector>

#include "Maze.h"
#include "MazeGame.h"



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