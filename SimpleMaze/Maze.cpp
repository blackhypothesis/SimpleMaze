#include "Maze.h"


Maze::Maze() : xSize(10), ySize(12)
{
	xDraw = 800;
	yDraw = 900;
	floorColor = olc::DARK_GREEN;

	vecMaze.resize(xSize * ySize);

	for (auto& v : vecMaze)
		v = std::unique_ptr<Block>(new Block(Block::BORDER));

	initMaze();
	createRandomMaze(30);

	if (xDraw / xSize < yDraw / ySize)
	{
		nBlockSize = xDraw / xSize;
	}
	else
	{
		nBlockSize = yDraw / ySize;
	}
}


Maze::Maze(int x, int y) : xSize(x), ySize(y)
{
	xDraw = 800;
	yDraw = 900;
	floorColor = olc::DARK_GREEN;

	vecMaze.resize(xSize * ySize);

	for (auto& v : vecMaze)
		v = std::unique_ptr<Block>(new Block(Block::BORDER));

	initMaze();
	createRandomMaze(30);

	if (xDraw / xSize < yDraw / ySize)
	{
		nBlockSize = xDraw / xSize;
	}
	else
	{
		nBlockSize = yDraw / ySize;
	}
}


int Maze::getXSize() const { return xSize; }
int Maze::getYSize() const { return ySize; }


const std::vector<std::unique_ptr<Block>>& Maze::getMaze() const

{
	return vecMaze;
}

const std::unique_ptr<Block>& Maze::getBlock(int x, int y) const
{
	return vecMaze[getIndex(x, y)];
}


int Maze::getIndex(int x, int y) const { return (xSize * y + x); }

int Maze::getBlockSize() const { return nBlockSize; }

Block::BlockType Maze::getBlockType(int x, int y) const { return vecMaze[getIndex(x, y)]->getBlockType(); }



float Maze::getRatio() const
{
	float nFree = 0;
	for (auto& v : vecMaze)
		if (v->getBlockType() == Block::FREE)
			nFree++;
	// ratio free blocks versus occupied blocks without border
	return nFree / ((xSize - 2) * (ySize - 2));
}


olc::Pixel Maze::getFloorColor() const
{
	return floorColor;
}


Location Maze::getEnd() const
{
	for (int i = 0; i < (int)vecMaze.size(); i++) {
		if (vecMaze[i]->getBlockType() == Block::END)
		{
			return Location { i % xSize, int(i / xSize) };
		}
	}
	return Location { 0, 0 };
}


// ------- methods to create maze
// ------------------------------
void Maze::initMaze()
{
	getVec2DSFML();
	bFirstCorridor = true;

	for (int x = 0; x < xSize; x++)
	{
		for (int y = 0; y < ySize; y++)
		{
			vecMaze[getIndex(x, y)]->setVisited(false);
			if (x == 0 || x == xSize - 1 || y == 0 || y == ySize - 1)
				vecMaze[getIndex(x, y)]->setBlockType(Block::BORDER);
			else
				vecMaze[getIndex(x, y)]->setBlockType(Block::WALL1);
		}
	}
}


void Maze::createCorridor(int nLengthCorridor)
{
	{
		int x = 0;
		int y = 0;
		int dx = 0;
		int dy = 0;
		int direction = rand() % 4;
		int nCurrentLengthCorridor = 0;
		int nTries = 0;

		// algorythm don't work with shorter corridors
		if (nLengthCorridor < 4) nLengthCorridor = 4;

		// try nTries times to crate a corridor which is at least 1 block long. 
		while (nCurrentLengthCorridor < 1 && nTries < xSize * ySize / 10)
		{
			if (bFirstCorridor)
			{
				x = rand() % (xSize - 2) + 1;
				y = rand() % (ySize - 2) + 1;
				vecMaze[getIndex(x, y)]->setBlockType(Block::FREE);
				bFirstCorridor = false;
			}
			else
			{
				// select random block till a FREE one is choosen.
				while (vecMaze[getIndex(x, y)]->getBlockType() != Block::FREE)
				{
					x = rand() % (xSize - 2) + 1;
					y = rand() % (ySize - 2) + 1;
				}
			}

			// from the FREE selected block calculate the next step according to the direction. 
			if (direction == NORTH) { dx = 0; dy = -1; }
			else if (direction == WEST) { dx = 1; dy = 0; }
			else if (direction == SOUTH) { dx = 0; dy = 1; }
			else if (direction == EAST) { dx = -1; dy = 0; }

			// save starting point - used to check if an existing corridor will be enlarged
			int xStart = x;
			int yStart = y;

			// try to create a corridor with a certain random length. start with the FREE selected block
			// in a alredy existing corridor. this will lead to concatenated corridors
			for (int c = 0; c < nLengthCorridor / 2 + (rand() % (nLengthCorridor / 2)); c++)
			{
				// navigate to the next block
				x += dx;
				y += dy;

				// hitting the border?
				if (vecMaze[getIndex(x, y)]->getBlockType() == Block::BORDER)
				{
					nTries++;
					break;
				}

				// do not get in contact with already FREE blocks (other corridors)
				if (direction == NORTH || direction == SOUTH)
				{
					if (vecMaze[getIndex(x, y)]->getBlockType() != Block::FREE
						&& vecMaze[getIndex(x, y + dy)]->getBlockType() != Block::FREE
						&& vecMaze[getIndex(x - 1, y + dy)]->getBlockType() != Block::FREE
						&& vecMaze[getIndex(x + 1, y + dy)]->getBlockType() != Block::FREE
						&& vecMaze[getIndex(x - 1, y)]->getBlockType() != Block::FREE
						&& vecMaze[getIndex(x + 1, y)]->getBlockType() != Block::FREE)
					{
						// do not enlarge an existing corridor every time, instead make a change of direction
						if (vecMaze[getIndex(xStart, yStart - dy)]->getBlockType() != Block::FREE
							|| (vecMaze[getIndex(xStart, yStart - dy)]->getBlockType() == Block::FREE && (rand() % 10000 == 0)))
						{
							vecMaze[getIndex(x, y)]->setBlockType(Block::FREE);
							nCurrentLengthCorridor++;
						}
						else
						{
							nTries++;
							break;
						}
					}
					else
					{
						nTries++;
						break;
					}
				}
				else if (direction == WEST || direction == EAST)
				{
					if (vecMaze[getIndex(x, y)]->getBlockType() != Block::FREE
						&& vecMaze[getIndex(x + dx, y)]->getBlockType() != Block::FREE
						&& vecMaze[getIndex(x + dx, y - 1)]->getBlockType() != Block::FREE
						&& vecMaze[getIndex(x + dx, y + 1)]->getBlockType() != Block::FREE
						&& vecMaze[getIndex(x, y - 1)]->getBlockType() != Block::FREE
						&& vecMaze[getIndex(x, y + 1)]->getBlockType() != Block::FREE)
					{
						vecMaze[getIndex(x, y)]->setBlockType(Block::FREE);
						nCurrentLengthCorridor++;
					}
					else
					{
						nTries++;
						break;
					}

					// do not enlarge an existing corridor every time, instead make a change of direction
					if (vecMaze[getIndex(xStart - dx, yStart)]->getBlockType() != Block::FREE
						|| (vecMaze[getIndex(xStart - dx, yStart)]->getBlockType() == Block::FREE && (rand() % 10000 == 0)))
					{
						vecMaze[getIndex(x, y)]->setBlockType(Block::FREE);
						nCurrentLengthCorridor++;
					}
					else
					{
						nTries++;
						break;
					}
				}
				else
				{
					nTries++;
					break;
				}
			}
		}
	}
}


void Maze::connectCorridors()
{
	for (int i = 0; i < xSize * ySize; i++)
	{
		int x = rand() % (xSize - 2) + 1;
		int y = rand() % (ySize - 2) + 1;

		if (vecMaze[getIndex(x, y)]->getBlockType() == Block::WALL1
			&& vecMaze[getIndex(x + 1, y)]->getBlockType() == vecMaze[getIndex(x - 1, y)]->getBlockType()
			&& vecMaze[getIndex(x, y + 1)]->getBlockType() == vecMaze[getIndex(x, y - 1)]->getBlockType())
		{
			if (vecMaze[getIndex(x + 1, y)]->getBlockType() != vecMaze[getIndex(x, y + 1)]->getBlockType())
			{
				vecMaze[getIndex(x, y)]->setBlockType(Block::FREE);
				break;
			}

		}
	}
}


void Maze::createRoom()
{
	int xp = rand() % (xSize - 2) + 1;
	int yp = rand() % (ySize - 2) + 1;
	int xWidth = 3 + rand() % (xSize / 5);
	int yHeight = 3 + rand() % (ySize / 5);

	if (xp + xWidth <= xSize - 1 && yp + yHeight <= ySize - 1)
	{
		for (int x = xp; x < xp + xWidth; x++)
		{
			for (int y = yp; y < yp + yHeight; y++)
			{
				if (vecMaze[getIndex(x, y)]->getBlockType() != Block::BORDER && vecMaze[getIndex(x, y)]->getBlockType() != Block::END)
					vecMaze[getIndex(x, y)]->setBlockType(Block::FREE);
			}
		}
	}
}


void Maze::createWall2()
{
	int xp = rand() % (xSize - 2) + 1;
	int yp = rand() % (ySize - 2) + 1;
	int xWidth = 3 + rand() % (xSize / 5);
	int yHeight = 3 + rand() % (ySize / 5);

	if (xp + xWidth <= xSize - 1 && yp + yHeight <= ySize - 1)
	{
		for (int x = xp; x < xp + xWidth; x++)
		{
			for (int y = yp; y < yp + yHeight; y++)
			{
				if (vecMaze[getIndex(x, y)]->getBlockType() == Block::WALL1)
					vecMaze[getIndex(x, y)]->setBlockType(Block::WALL2);
			}
		}
	}
}


void Maze::createEnd()
{
	while (true)
	{
		int x = rand() % (xSize - 2) + 1;
		int y = rand() % (ySize - 2) + 1;

		if (vecMaze[getIndex(x, y)]->getBlockType() == Block::FREE)
		{
			vecMaze[getIndex(x, y)]->setBlockType(Block::END);
			break;
		}
	}
}

void Maze::getVec2DSFML()
{
	vecRect.clear();

	for (int x = 0; x < xSize; x++)
	{
		for (int y = 0; y < ySize; y++)
		{
			vecRect.push_back(sf::RectangleShape());
			vecRect.back().setSize(sf::Vector2f{ (float)getBlockSize(), (float)getBlockSize() });
			vecRect.back().setPosition(sf::Vector2f{ (float)(x * getBlockSize()), (float)(y * getBlockSize()) });

			if (vecMaze[getIndex(x, y)]->getBlockType() == Block::BORDER)
				vecRect.back().setFillColor(sf::Color::Black);
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::WALL1)
				vecRect.back().setFillColor(sf::Color::Red);
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::WALL2)
				vecRect.back().setFillColor(sf::Color::Blue);
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::FREE)
				vecRect.back().setFillColor(sf::Color::White);
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::START)
				vecRect.back().setFillColor(sf::Color::Cyan);
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::END)
				vecRect.back().setFillColor(sf::Color::Green);
		}
	}
}

Location Maze::getRandomFreeLocation()
{
	int x;
	int y;

	while (true)
	{
		x = rand() % (xSize - 2) + 1;
		y = rand() % (ySize - 2) + 1;

		if (vecMaze[getIndex(x, y)]->getBlockType() == Block::FREE)
		{
			break;
		}
	}
	return Location{ x, y };
}


void Maze::createRandomMaze(int nLC)
{
	srand((int)time(0));
	initMaze();

	// create corridors
	for (int i = 0; i < xSize * ySize; i++)
	{
		createCorridor(nLC);
		// if ratio > 0.49, most of the possible corridors are crated
		if (getRatio() > 0.49f)
			break;
	}
	// create loops in the maze. therefore more than one solution exists.
	for (int i = 0; i < sqrt(xSize * ySize) / 8; i++)
		connectCorridors();
	// create rooms
	for (int i = 0; i < sqrt(xSize * ySize) / 4; i++)
		createRoom();
	// set randomly some areas with WALL2 blocktype
	for (int i = 0; i < sqrt(xSize * ySize) / 4; i++)
		createWall2();
	// set end point
	createEnd();
}


bool Maze::isFree(int x, int y)
{
	if (vecMaze[getIndex(x, y)]->getBlockType() != Block::BORDER 
		&& vecMaze[getIndex(x, y)]->getBlockType() != Block::WALL1 && vecMaze[getIndex(x, y)]->getBlockType() != Block::WALL2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Maze::drawGrid(olc::PixelGameEngine* engine)
{
	engine->FillRect(0, 0, xDraw, yDraw, olc::YELLOW);
	for (int x = 0; x < xSize; x++)
	{
		for (int y = 0; y < ySize; y++)
		{
			engine->DrawRect(x * getBlockSize(), y * getBlockSize(), getBlockSize(), getBlockSize(), olc::GREY);
			if (vecMaze[getIndex(x, y)]->getBlockType() == Block::BORDER)
				engine->FillRect(x * getBlockSize(), y * getBlockSize(), getBlockSize(), getBlockSize(), olc::DARK_GREY);
		}
	}
}

void Maze::draw2D(olc::PixelGameEngine *engine)
{
	engine->FillRect(0, 0, xDraw, yDraw, olc::YELLOW);
	for (int x = 0; x < xSize; x++)
	{
		for (int y = 0; y < ySize; y++)
		{
			if (vecMaze[getIndex(x, y)]->getBlockType() == Block::BORDER)
				engine->FillRect(x * getBlockSize(), y * getBlockSize(), getBlockSize(), getBlockSize(), olc::DARK_GREY);
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::WALL1)
				engine->FillRect(x * getBlockSize(), y * getBlockSize(), getBlockSize(), getBlockSize(), olc::DARK_RED);
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::WALL2)
				engine->FillRect(x * getBlockSize(), y * getBlockSize(), getBlockSize(), getBlockSize(), olc::DARK_BLUE);
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::FREE)
				engine->FillRect(x * getBlockSize(), y * getBlockSize(), getBlockSize(), getBlockSize(), floorColor);
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::START)
			{
				engine->FillRect(x * getBlockSize(), y * getBlockSize(), getBlockSize(), getBlockSize(), olc::WHITE);
				engine->FillCircle(x * getBlockSize() + getBlockSize() / 2, y * getBlockSize() + getBlockSize() / 2, getBlockSize() / 2, olc::RED);
			}
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::END)
			{
				engine->FillRect(x * getBlockSize(), y * getBlockSize(), getBlockSize(), getBlockSize(), floorColor);
				engine->FillCircle(x * getBlockSize() + getBlockSize() / 2, y * getBlockSize() + getBlockSize() / 2, getBlockSize() / 2, olc::GREEN);
			}
		}
	}
}


void Maze::draw2DVisited(olc::PixelGameEngine* engine)
{
	// drawGrid(engine);
	for (int x = 0; x < xSize; x++)
	{
		for (int y = 0; y < ySize; y++)
		{
			if (vecMaze[getIndex(x, y)]->getBlockType() == Block::BORDER && vecMaze[getIndex(x, y)]->getVisited())
				engine->FillRect(x * getBlockSize(), y * getBlockSize(), getBlockSize(), getBlockSize(), olc::DARK_GREY);
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::WALL1 && vecMaze[getIndex(x, y)]->getVisited())
				engine->FillRect(x * getBlockSize(), y * getBlockSize(), getBlockSize(), getBlockSize(), olc::DARK_RED);
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::WALL2 && vecMaze[getIndex(x, y)]->getVisited())
				engine->FillRect(x * getBlockSize(), y * getBlockSize(), getBlockSize(), getBlockSize(), olc::DARK_BLUE);
			else if (vecMaze[getIndex(x, y)]->getBlockType() == Block::FREE && vecMaze[getIndex(x, y)]->getVisited())
				engine->FillRect(x * getBlockSize(), y * getBlockSize(), getBlockSize(), getBlockSize(), floorColor);
		}
	}
}

void Maze::draw2DSFML(sf::RenderWindow* window)
{
	for (auto& v : vecRect)
		window->draw(v);
}
