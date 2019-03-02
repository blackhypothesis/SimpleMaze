#include <iostream>
#include <vector>

#include "Maze.h"
#include "MazeGame.h"
#include "MazeGameSFML.h"

#define SFML



int main()
{
#ifdef OLC

	std::cout << "Creating Maze, ..." << std::endl;
	MazeGame maze;
	std::cout << "Maze created" << std::endl;


	if (maze.Construct(1700, 900, 1, 1))
		maze.Start();

#endif

#ifdef SFML

	MazeGameSFML mazeSFML = MazeGameSFML();
	bool retVal = mazeSFML.GameInit();
	retVal = mazeSFML.GameLoop();

#endif	

	return 0;
}