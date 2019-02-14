#define OLC_PGE_APPLICATION
#include"olcPixelGameEngine.h"

using namespace std;

class cMaze
{
public:
	cMaze(int xSize, int ySize)
	{
		this->xSize = xSize;
		this->ySize = ySize;
		vecMaze.resize(xSize * ySize);

		initMaze();
	}

private:
	int xSize;
	int ySize;
	vector<int> vecMaze;
	int nBoxSize = 20;
	enum BlockType { FREE, BORDER, WALL, START, END, LOOP };
	enum Direction { NORTH, WEST, SOUTH, EAST };
	int nlengthCorridor = 0;
	int nTries = 0;

private:
	int getIndex(int x, int y)
	{
		return (ySize * x + y);
	}

private:
	void initMaze()
	{
		for (int x = 0; x < xSize; x++)
		{
			for (int y = 0; y < ySize; y++)
			{
				if (x == 0 || x == xSize - 1 || y == 0 || y == ySize - 1)
					vecMaze.at(getIndex(x, y)) = BORDER;
				else
					vecMaze.at(getIndex(x, y)) = WALL;
			}
		}
	}

public:
	void createRandomMaze(int nlengthCorridor, int nTries)
	{
		this->nlengthCorridor = nlengthCorridor;
		this->nTries = nTries;

		bool isFirst = true;
		srand(time(0));
		
		initMaze();

		while (nTries > 0)
		{
			nTries--;
			int x = rand() % (xSize - 2) + 1;
			int y = rand() % (ySize - 2) + 1;
			int direction = rand() % 4;
			int dx = 0;
			int dy = 0;

			// in the first iteration force to use a wall as starting point
			if (isFirst)
			{
				vecMaze.at(getIndex(x, y)) = FREE;
				isFirst = false;
			}
			else
			{
				if (vecMaze.at(getIndex(x, y)) != FREE)
					continue;
			}

			// continue, if the selected block is FREE. this will result in concatenated corridors 
			if (direction == NORTH)
			{
				dx = 0;
				dy = -1;
			}
			else if (direction == WEST)
			{
				dx = 1;
				dy = 0;
			}
			else if (direction == SOUTH)
			{
				dx = 0;
				dy = 1;
			}
			else if (direction == EAST)
			{
				dx = -1;
				dy = 0;
			}

			// try to create a corridor with a certain random length. start with the FREE selected block
			// in a alredy existing corridor.
			for (int c = 0; c < nlengthCorridor/2 +  rand() % (nlengthCorridor/2); c++)
			{
				// navigate to the next block
				x += dx;
				y += dy;

				// hitting the border?
				if (vecMaze.at(getIndex(x, y)) == BORDER)
					break;

				// do not get in contact with already FREE blocks (other corridors)
				if (direction == NORTH || direction == SOUTH)
				{
					if (vecMaze.at(getIndex(x, y + dy)) != FREE
						&& vecMaze.at(getIndex(x - 1, y + dy)) != FREE
						&& vecMaze.at(getIndex(x + 1, y + dy)) != FREE
						&& vecMaze.at(getIndex(x - 1, y)) != FREE 
						&& vecMaze.at(getIndex(x + 1, y)) != FREE)
						vecMaze.at(getIndex(x, y)) = FREE;
					else
						break;
				}
				else if (direction == WEST || direction == EAST)
				{
					if (vecMaze.at(getIndex(x + dx, y)) != FREE 
						&& vecMaze.at(getIndex(x + dx, y - 1)) != FREE
						&& vecMaze.at(getIndex(x + dx, y + 1)) != FREE
						&& vecMaze.at(getIndex(x, y - 1)) != FREE 
						&& vecMaze.at(getIndex(x, y + 1)) != FREE)
						vecMaze.at(getIndex(x, y)) = FREE;
					else
						break;
				}
			}
		}

		// create rooms
		for (int i = 0; i < 10; i++)
		{
			int xp = rand() % (xSize - 2) + 1;
			int yp = rand() % (ySize - 2) + 1;
			int xWidth = 2 + rand() % 8;
			int yHeight = 2 + rand() % 8;

			if (xp + xWidth > xSize - 1 || yp + yHeight > ySize - 1)
				continue;

			for (int x = xp; x < xp + xWidth; x++)
			{
				for (int y = yp; y < yp + yHeight; y++)
				{
					if (vecMaze.at(getIndex(x, y)) != BORDER)
						vecMaze.at(getIndex(x, y)) = FREE;
				}
			}
		}

		// create loops in the maze. therefore more than one solution exists.
		int i = 50;
		while (i > 0)
		{
			int x = rand() % (xSize - 2) + 1;
			int y = rand() % (ySize - 2) + 1;

			if (vecMaze.at(getIndex(x, y)) == WALL
				&& vecMaze.at(getIndex(x + 1, y)) == vecMaze.at(getIndex(x - 1, y)) 
				&& vecMaze.at(getIndex(x, y + 1)) == vecMaze.at(getIndex(x, y - 1)))
			{
				if (vecMaze.at(getIndex(x + 1, y)) != vecMaze.at(getIndex(x, y + 1)))
				{
					vecMaze.at(getIndex(x, y)) = LOOP;
					i--;
				}
			}
		}

		// set start point
		while (true)
		{
			int x = rand() % (xSize - 2) + 1;
			int y = rand() % (ySize - 2) + 1;

			if (vecMaze.at(getIndex(x, y)) == FREE)
			{
				vecMaze.at(getIndex(x, y)) = START;
				break;
			}
		}

		// set end point
		while (true)
		{
			int x = rand() % (xSize - 2) + 1;
			int y = rand() % (ySize - 2) + 1;

			if (vecMaze.at(getIndex(x, y)) == FREE)
			{
				vecMaze.at(getIndex(x, y)) = END;
				break;
			}
		}
	}

public:
	void Draw(olc::PixelGameEngine *engine)
	{
		for (int x = 0; x < xSize; x++)
		{
			for (int y = 0; y < ySize; y++)
			{
				if (vecMaze.at(getIndex(x, y)) == BORDER)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::DARK_GREY);
				else if (vecMaze.at(getIndex(x, y)) == WALL)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::BLACK);
				else if (vecMaze.at(getIndex(x, y)) == FREE)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::WHITE);
				else if (vecMaze.at(getIndex(x, y)) == START)
				{
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::WHITE);
					engine->FillCircle(x * nBoxSize + nBoxSize / 2, y * nBoxSize + nBoxSize / 2, nBoxSize / 2, olc::GREEN);
				}
				else if (vecMaze.at(getIndex(x, y)) == END)
				{
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::WHITE);
					engine->FillCircle(x * nBoxSize + nBoxSize / 2, y * nBoxSize + nBoxSize / 2, nBoxSize / 2, olc::RED);
				}
				else if (vecMaze.at(getIndex(x, y)) == LOOP)
				{
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::WHITE);
					//engine->FillCircle(x * nBoxSize + nBoxSize / 2, y * nBoxSize + nBoxSize / 2, nBoxSize / 2, olc::YELLOW);
				}
			}
		}
		engine->FillRect(1510, 90, 200, 150, olc::WHITE);
		engine->DrawString(1520, 100, to_string(nlengthCorridor), olc::BLACK, 2);
		engine->DrawString(1520, 150, to_string(nTries), olc::BLACK, 2);
		engine->DrawString(1520, 200, to_string(time(0)), olc::BLACK, 2);

	}
};

class SimpleMaze : public olc::PixelGameEngine
{
public:
	SimpleMaze()
	{
		sAppName = "Simple Maze";
	}

private:
	int nlengthCorridor = 50;
	int nTries = 50000;
	cMaze maze = cMaze(75, 45);

public:
	bool OnUserCreate() override
	{
		FillRect(0, 0, 1699, 899, olc::YELLOW);
		//maze.createRandomMaze(50, 1749);
		maze.createRandomMaze(nlengthCorridor, nTries);
		maze.Draw(this);

		return true;
	}

public:
	bool OnUserUpdate(float fElapsedTime) override
	{
		// set type of game
		if (GetKey(olc::Key::N).bReleased)
		{
			maze.createRandomMaze(nlengthCorridor, nTries);
			maze.Draw(this);
		}
		if (GetKey(olc::Key::A).bReleased)
		{
			nlengthCorridor -= 5;
			if (nlengthCorridor < 5)
				nlengthCorridor = 5;
			maze.createRandomMaze(nlengthCorridor, nTries);
			maze.Draw(this);
		}

		if (GetKey(olc::Key::S).bReleased)
		{
			nlengthCorridor += 5;
			if (nlengthCorridor > 200)
				nlengthCorridor = 200;
			maze.createRandomMaze(nlengthCorridor, nTries);
			maze.Draw(this);
		}

		if (GetKey(olc::Key::Z).bHeld)
		{
			nTries -= 200;
			if (nTries < 200)
				nTries = 200;
			maze.createRandomMaze(nlengthCorridor, nTries);
			maze.Draw(this);
		}

		if (GetKey(olc::Key::X).bHeld)
		{
			nTries += 200;
			if (nTries > 100000)
				nTries = 100000;
			maze.createRandomMaze(nlengthCorridor, nTries);
			maze.Draw(this);
		}

		return true;
	}
};

int main()
{
	SimpleMaze sMaze;
	if (sMaze.Construct(1700, 900, 1, 1))
		sMaze.Start();
	return 0;
}