#include "MazeGameSFML.h"



MazeGameSFML::MazeGameSFML() : player(&maze)
{
	nLengthCorridor = 50;
	nMazeWidth = 40;
	nMazeHeight = 50;
	maze = Maze(nMazeWidth, nMazeHeight);
	fAngle = 0;
	bPlayerInteraction = false;
	bDraw2D = false;
	fDistanceToEnd = 0.0f;
	mouseX = 0;
	mouseY = 0;
	mouseOldX = mouseX;
	mouseOldY = mouseY;
}


MazeGameSFML::~MazeGameSFML()
{
}

bool MazeGameSFML::GameInit()
{
	maze.createRandomMaze(nLengthCorridor);
	player.setRandomPosition();

	return true;
}

bool MazeGameSFML::GameLoop()
{
	sf::RenderWindow window(sf::VideoMode(1700, 900), "My window");
	sf::Clock clock;
	sf::Time elapsed;
	sf::Event event;

	while (window.isOpen())
	{	
		elapsed = clock.restart();
		// float fps = 1000000 / elapsed.asMicroseconds();
		std::cout << "Time: " << elapsed.asMicroseconds() << std::endl;
		std::cout << "hello";

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::N)
				{
					std::cout << "create random maze" << std::endl;
					maze.createRandomMaze(nLengthCorridor);
					player.setRandomPosition();
				}

				// User Navigation
				// Rotate
				if (event.key.code == sf::Keyboard::A)
				{
					player.rotate(-0.1f);
					bPlayerInteraction = true;
				}
			}
		}

		window.clear(sf::Color::Yellow);

		player.draw3DSFML(&window, &maze);
		//maze.draw2DSFML(&window);
		
		window.display();
	}

	return true;
}
