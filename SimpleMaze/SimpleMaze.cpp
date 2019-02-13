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

private:
	int xSize;
	int ySize;
	vector<int> vecMaze;
	int nBoxSize = 30;
	enum BlockType { FREE, BORDER, WALL, START, END };
	enum Direction { NORTH, WEST, SOUTH, EAST };

private:
	int getIndex(int x, int y)
	{
		return (ySize * x + y);
	}

public:
	void createRandomMaze(int nlengthCorridor, int tries)
	{
		bool isFirst = true;
		// srand(time(0));
		srand(394);

		while (tries > 0)
		{
			tries--;
			int x = rand() % (xSize - 2) + 1;
			int y = rand() % (ySize - 2) + 1;
			int direction = rand() % 4;
			int dx = 0;
			int dy = 0;

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

			for (int c = 0; c < rand() % nlengthCorridor; c++)
			{
				// vecMaze.at(getIndex(x, y)) = START;
				x += dx;
				y += dy;

				// hitting the border?
				if (vecMaze.at(getIndex(x, y)) == BORDER)
					break;

				if (direction == NORTH || direction == SOUTH)
				{
					if (vecMaze.at(getIndex(x, y + dy)) != FREE && vecMaze.at(getIndex(x - 1, y)) != FREE && vecMaze.at(getIndex(x + 1, y)) != FREE)
						vecMaze.at(getIndex(x, y)) = FREE;
					else
						continue;
				}
				else if (direction == WEST || direction == EAST)
				{
					if (vecMaze.at(getIndex(x + dx, y)) != FREE && vecMaze.at(getIndex(x, y - 1)) != FREE && vecMaze.at(getIndex(x, y + 1)) != FREE)
						vecMaze.at(getIndex(x, y)) = FREE;
					else
						continue;
				}
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
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::BLUE);
				else if (vecMaze.at(getIndex(x, y)) == WALL)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::BLACK);
				else if (vecMaze.at(getIndex(x, y)) == FREE)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::WHITE);
				else if (vecMaze.at(getIndex(x, y)) == START)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::GREEN);
			}
		}
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
	cMaze maze = cMaze(30, 20);

public:
	bool OnUserCreate() override
	{
		FillRect(0, 0, 999, 799, olc::YELLOW);
		maze.createRandomMaze(50, 100000);
		maze.Draw(this);

		return true;
	}

public:
	bool OnUserUpdate(float fElapsedTime) override
	{

			
		return true;
	}
};

int main()
{
	SimpleMaze sMaze;
	if (sMaze.Construct(1000, 800, 1, 1))
		sMaze.Start();
	return 0;
}