#define OLC_PGE_APPLICATION
#include"olcPixelGameEngine.h"

using namespace std;

enum BlockType { FREE, BORDER, WALL, START, END, LOOP };
enum Direction { NORTH, WEST, SOUTH, EAST };

typedef struct sPoint
{
	int x;
	int y;
} point;

class cUser;

class cMaze
{
public:
	cMaze(int xSize, int ySize)
	{
		this->xSize = xSize;
		this->ySize = ySize;
		vecMaze.resize(xSize * ySize);
		initMaze();
		createRandomMaze(20, 50000);
		nBoxSize = 800 / xSize;
	}

public:
	int nBoxSize;
	vector<int> vecMaze;

private:
	int xSize;
	int ySize;
	int nlengthCorridor = 0;
	int nTries = 0;
	int nEndX;
	int nEndY;

public:
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
					vecMaze[getIndex(x, y)] = BORDER;
				else
					vecMaze[getIndex(x, y)] = WALL;
			}
		}
	}

public:
	void createRandomMaze(int nlengthCorridor, int nTries)
	{
		this->nlengthCorridor = nlengthCorridor;
		this->nTries = nTries;

		bool isFirst = true;
		srand((int)time(0));

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
				vecMaze[getIndex(x, y)] = FREE;
				isFirst = false;
			}
			else
			{
				if (vecMaze[getIndex(x, y)] != FREE)
					continue;
			}

			// if the selected block is FREE, calculate the next step according to the direction. 
			if (direction == NORTH)      { dx = 0; dy = -1; }
			else if (direction == WEST)  { dx = 1; dy = 0; }
			else if (direction == SOUTH) { dx = 0; dy = 1; }
			else if (direction == EAST)  { dx = -1; dy = 0; }

			// try to create a corridor with a certain random length. start with the FREE selected block
			// in a alredy existing corridor. this will lead to concatenated corridors
			for (int c = 0; c < nlengthCorridor / 2 + rand() % (nlengthCorridor / 2); c++)
			{
				// navigate to the next block
				x += dx;
				y += dy;

				// hitting the border?
				if (vecMaze[getIndex(x, y)] == BORDER)
					break;

				// do not get in contact with already FREE blocks (other corridors)
				if (direction == NORTH || direction == SOUTH)
				{
					if (vecMaze[getIndex(x, y + dy)] != FREE
						&& vecMaze[getIndex(x - 1, y + dy)] != FREE
						&& vecMaze[getIndex(x + 1, y + dy)] != FREE
						&& vecMaze[getIndex(x - 1, y)] != FREE
						&& vecMaze[getIndex(x + 1, y)] != FREE)
						vecMaze[getIndex(x, y)] = FREE;
					else
						break;
				}
				else if (direction == WEST || direction == EAST)
				{
					if (vecMaze[getIndex(x + dx, y)] != FREE
						&& vecMaze[getIndex(x + dx, y - 1)] != FREE
						&& vecMaze[getIndex(x + dx, y + 1)] != FREE
						&& vecMaze[getIndex(x, y - 1)] != FREE
						&& vecMaze[getIndex(x, y + 1)] != FREE)
						vecMaze[getIndex(x, y)] = FREE;
					else
						break;
				}
			}
		}

		// create loops in the maze. therefore more than one solution exists.
		int i = xSize * ySize / 100;
		while (i > 0)
		{
			int x = rand() % (xSize - 2) + 1;
			int y = rand() % (ySize - 2) + 1;

			if (vecMaze[getIndex(x, y)] == WALL
				&& vecMaze[getIndex(x + 1, y)] == vecMaze[getIndex(x - 1, y)]
				&& vecMaze[getIndex(x, y + 1)] == vecMaze[getIndex(x, y - 1)])
			{
				if (vecMaze[getIndex(x + 1, y)] != vecMaze[getIndex(x, y + 1)])
					i--;
			}
		}
		
		// create rooms
		for (int i = 0; i < xSize * ySize / 100; i++)
		{
			int xp = rand() % (xSize - 2) + 1;
			int yp = rand() % (ySize - 2) + 1;
			int xWidth = 3 + rand() % 8;
			int yHeight = 3 + rand() % 8;

			if (xp + xWidth > xSize - 1 || yp + yHeight > ySize - 1)
				continue;

			for (int x = xp; x < xp + xWidth; x++)
			{
				for (int y = yp; y < yp + yHeight; y++)
				{
					if (vecMaze[getIndex(x, y)] != BORDER)
						vecMaze[getIndex(x, y)] = FREE;
				}
			}
		}
		
		// set end point
		while (true)
		{
			int x = rand() % (xSize - 2) + 1;
			int y = rand() % (ySize - 2) + 1;

			if (vecMaze[getIndex(x, y)] == FREE)
			{
				vecMaze[getIndex(x, y)] = END;
				nEndX = x;
				nEndY = y;
				break;
			}
		}
	}

public:
	void updateMaze(point p, int bt)
	{
		vecMaze[getIndex(p.x, p.y)] = bt;
	}

public:
	point getRandomFreePoint()
	{
		int x;
		int y;

		while (true)
		{
			x = rand() % (xSize - 2) + 1;
			y = rand() % (ySize - 2) + 1;

			if (vecMaze[getIndex(x, y)] == FREE)
			{
				break;
			}
		}
		return point{ x, y };
	}

public:
	bool isFree(int x, int y)
	{
		if (vecMaze[getIndex(x, y)] != BORDER && vecMaze[getIndex(x, y)] != WALL)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

public:
	point getEnd()
	{
		return point{ nEndX, nEndY };
	}


public:
	void Draw(olc::PixelGameEngine *engine)
	{
		engine->FillRect(0, 0, 800, 800, olc::YELLOW);
		for (int x = 0; x < xSize; x++)
		{
			for (int y = 0; y < ySize; y++)
			{ 
				if (vecMaze[getIndex(x, y)] == BORDER)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::DARK_GREY);
				else if (vecMaze[getIndex(x, y)] == WALL)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::BLACK);
				else if (vecMaze[getIndex(x, y)] == FREE)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::Pixel(200, 200, 255));
				else if (vecMaze[getIndex(x, y)] == START)
				{
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::WHITE);
					engine->FillCircle(x * nBoxSize + nBoxSize / 2, y * nBoxSize + nBoxSize / 2, nBoxSize / 2, olc::RED);
				}
				else if (vecMaze[getIndex(x, y)] == END)
				{
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::WHITE);
					engine->FillCircle(x * nBoxSize + nBoxSize / 2, y * nBoxSize + nBoxSize / 2, nBoxSize / 2, olc::DARK_GREEN);
				}
				else if (vecMaze[getIndex(x, y)] == LOOP)
				{
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::WHITE);
					engine->FillCircle(x * nBoxSize + nBoxSize / 2, y * nBoxSize + nBoxSize / 2, nBoxSize / 4, olc::YELLOW);
				}
			}
		}
	}

public:
	void getGridPath()
	{

	}

};


class cUser
{
public:
	cUser(cMaze *maze)
	{
		this->maze = maze;
		p = maze->getRandomFreePoint();
		// center the user's position into the middle of the square
		fxPos = (float)p.x + 0.5f;
		fyPos = (float)p.y + 0.5f;
		fxPosOld = fxPos;
		fyPosOld = fyPos;
		fxVec = cosf(fAngle);
		fyVec = sinf(fAngle);
	}

private:
	point p;
	float fxPos;
	float fyPos;
	float fxPosOld;
	float fyPosOld;
	float fAngle = 0;  
	float fxVec;          // x vector if fAngle
	float fyVec;          // y vector of fAngle
	float fx90leftVec;    // sidestep left
	float fx90rightVec;   // sidestep right
	float fUserSize = 0.2f;
	cMaze *maze;
	
public:
	float getAngle() { return fAngle; }
	point getPos() { return point{ (int)fxPos, (int)fyPos }; }

public:
	void setRandomPosition()
	{
		p = maze->getRandomFreePoint();
		fxPos = (float)p.x + 0.5f;
		fyPos = (float)p.y + 0.5f;
		fxPosOld = fxPos;
		fyPosOld = fyPos;
	}

public:
	void rotate(float fDelta)
	{
		fAngle += fDelta;
		fxVec = cosf(fAngle);
		fyVec = sinf(fAngle);
	}

	void move(float fDistance)
	{
		fxPosOld = fxPos;
		fyPosOld = fyPos;
		fxPos += fxVec * fDistance;
		fyPos += fyVec * fDistance;

		if (collisionDetection())
		{
			// try to slide along the y axis
			fxPos = fxPosOld;
			fyPos += fyVec * fDistance;

			if(collisionDetection())
			{
				// try to slide along the x axis
				fxPos += fxVec * fDistance;
				fyPos = fyPosOld;

				if (collisionDetection())
				{
					// stuck in a corner, no sliding possible. user cannot move.
					fxPos = fxPosOld;
					fyPos = fyPosOld;
				}

			}
		}
	}

public:
	void moveSide(float fDistance, bool bDirection)
	{
		if (bDirection)
		// right shift
		{
			fxPosOld = fxPos;
			fyPosOld = fyPos;
			fxPos += fyVec * fDistance;
			fyPos += -fxVec * fDistance;

			if (collisionDetection())
			{
				fxPos = fxPosOld;
				fyPos = fyPosOld;
			}
		}
		else
		// left shift
		{
			fxPosOld = fxPos;
			fyPosOld = fyPos;
			fxPos += -fyVec * fDistance;
			fyPos += fxVec * fDistance;

			if (collisionDetection())
			{
				fxPos = fxPosOld;
				fyPos = fyPosOld;
			}
		}
	}

private:
	bool collisionDetection()
	{			
		float xv = 0;
		float yv = 0;

		for (float theta = 0; theta < 2 * 3.14159f; theta += 0.1f)
		{

			xv = cosf(theta) * fUserSize;
			yv = sinf(theta) * fUserSize;
			if (!maze->isFree((int)(fxPos + xv), (int)(fyPos + yv)))
			{
				return true;
			}
		}
		return false;
	}

public:
	void draw2D(olc::PixelGameEngine *engine)
	{
		int xp = (int)(fxPos * (float)maze->nBoxSize);
		int yp = (int)(fyPos * (float)maze->nBoxSize);
		//int xpOld = (int)((fxPosOld - fUserSize) * (float)maze->nBoxSize);
		//int ypOld = (int)((fyPosOld - fUserSize) * (float)maze->nBoxSize);
		int xpOld = (int)(fxPosOld * (float)maze->nBoxSize);
		int ypOld = (int)(fyPosOld * (float)maze->nBoxSize);
		int nBoxSize = int(fUserSize * maze->nBoxSize * 2 + 1);

		// engine->FillRect(xpOld, ypOld, nBoxSize, nBoxSize, olc::WHITE);

		engine->FillCircle(xpOld, ypOld, int(fUserSize * maze->nBoxSize), olc::WHITE);
		engine->FillCircle(xp, yp, int(fUserSize * maze->nBoxSize), olc::DARK_MAGENTA);
		engine->DrawLine(xp, yp, int(xp + fxVec * (maze->nBoxSize * fUserSize)),
			(int)(yp + fyVec * (maze->nBoxSize * fUserSize)), olc::WHITE);
	}

public:
	void draw3D(olc::PixelGameEngine *engine, cMaze *maze)
	{
		int nScreenWidth = 860;
		int nScreenHeight = 500;
		int nXoffset = 820;
		int nYoffset = 50;
		point pEnd = maze->getEnd();

		// field of view pi/4 == 90 Degree
		float fFOV = 3.14159f / 4.0f;

		// maximum distance to do ray tracing
		float fDepth = 30.0f;

		for (int x = 0; x < nScreenWidth; x++)
		{
			// calculate the projected ray angle 
			float fRayAngle = (fAngle - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;
			// find distance to wall
			float fStepSize = 0.1f;
			float fDistanceToWall = 0.0f;

			bool bHitWall = false;
			bool bBoundary = false;

			float nTestX;
			float nTestY;

			float fEyeX = cosf(fRayAngle);
			float fEyeY = sinf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;
				nTestX = fxPos + fEyeX * fDistanceToWall;
				nTestY = fyPos + fEyeY * fDistanceToWall;

				if (maze->vecMaze[maze->getIndex((int)nTestX, (int)nTestY)] != 0)
				{
					if (fStepSize < 0.01f)
						bHitWall = true;
					else
					{
						fDistanceToWall -= fStepSize;
						fStepSize *= 0.5f;
					}
				}
			}

			int nShadow = 255 - fDistanceToWall * 15;
			if (nShadow < 0)
				nShadow = 0;
			olc::Pixel pWall = olc::Pixel(nShadow, 0, 0);

			// draw END in different color
			if ((int)(nTestX) == pEnd.x && (int)nTestY == pEnd.y)
				pWall = olc::Pixel(0, nShadow, 0);

			float nStripe = 0.02;
			// check if nTestX and nTestY are near an integer number
			float nFractX = nTestX - (int)nTestX;
			float nFractY = nTestY - (int)nTestY;

			// if this is the case, draw a stripe
			if (nFractX < nStripe && nFractY < nStripe || nFractX < nStripe && 1 - nFractY < nStripe
				|| 1 - nFractX < nStripe && nFractY < nStripe || 1 - nFractX < nStripe && 1 - nFractY < nStripe)
				pWall = olc::Pixel(nShadow, nShadow, nShadow);

			int nCeiling = (float)(nScreenHeight / 2.0f) - nScreenHeight / fDistanceToWall;
			if (nCeiling < 0)
				nCeiling = 0;
			int nFloor = nScreenHeight - nCeiling;

			// ceiling
			engine->DrawLine(x + nXoffset, nYoffset, x + nXoffset, nYoffset + nCeiling, olc::Pixel(0, 100, 200));
			// walls
			engine->DrawLine(x + nXoffset, nYoffset + nCeiling, x + nXoffset, nYoffset + nFloor, pWall);
			// floor
			engine->DrawLine(x + nXoffset, nYoffset + nFloor, x + nXoffset, nYoffset + nScreenHeight, olc::Pixel(0, 128, 64));

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
	int nlengthCorridor = 100;
	int nTries = 50000;
	int nMazeWidth = 25;
	int nMazeHeight = 25;
	cMaze maze = cMaze(nMazeWidth, nMazeHeight);
	cUser user = cUser(&maze);
	float fAngle = 0;
	bool bUserInteraction = false;
	float fDistanceToEnd = 0.0f;

public:
	bool OnUserCreate() override
	{
		FillRect(0, 0, 1699, 899, olc::YELLOW);
		//maze.createRandomMaze(50, 1749);
		maze.createRandomMaze(nlengthCorridor, nTries);
		maze.Draw(this);
		user.draw2D(this);
		user.draw3D(this, &maze);

		return true;
	}

public:
	bool OnUserUpdate(float fElapsedTime) override
	{

		// new game
		if (GetKey(olc::Key::N).bReleased)
		{
			maze.createRandomMaze(nlengthCorridor, nTries);
			user.setRandomPosition();
			maze.Draw(this);
			user.draw2D(this);
			user.draw3D(this, &maze);
		}

		// grow maze
		if (GetKey(olc::Key::V).bReleased)
		{
			nMazeHeight++;
			nMazeWidth++;
			maze = cMaze(nMazeWidth, nMazeHeight);
			maze.createRandomMaze(nlengthCorridor, nTries);
			user.setRandomPosition();
			maze.Draw(this);
			user.draw2D(this);
			user.draw3D(this, &maze);
		}

		//shrink maze
		if (GetKey(olc::Key::B).bReleased)
		{
			nMazeHeight--;
			nMazeWidth--;
			maze = cMaze(nMazeWidth, nMazeHeight);
			maze.createRandomMaze(nlengthCorridor, nTries);
			user.setRandomPosition();
			maze.Draw(this);
			user.draw2D(this);
			user.draw3D(this, &maze);
		}

		// get distance to end
		if (GetKey(olc::Key::O).bReleased)
		{
			point pUser = user.getPos();
			point pEnd = maze.getEnd();
			int nDeltaX = pUser.x - pEnd.x;
			int nDeltaY = pUser.y - pEnd.y;
			fDistanceToEnd = sqrt(nDeltaX * nDeltaX + nDeltaY * nDeltaY);
		}

		// User Navigation
		// Rotate
		if (GetKey(olc::Key::A).bHeld)
		{
			user.rotate(-2.0f * fElapsedTime);
			user.draw2D(this);
			bUserInteraction = true;
		}
		if (GetKey(olc::Key::D).bHeld)
		{
			user.rotate(2.0f * fElapsedTime);
			user.draw2D(this);
			bUserInteraction = true;
		}

		// Move forward - backward
		if (GetKey(olc::Key::W).bHeld)
		{
			user.move(3.0f * fElapsedTime);
			user.draw2D(this);
			bUserInteraction = true;
		}
		if (GetKey(olc::Key::S).bHeld)
		{
			user.move(-3.0f * fElapsedTime);
			user.draw2D(this);
			bUserInteraction = true;
		}

		// move: shift left - right
		if (GetKey(olc::Key::P).bHeld)
		{
			user.moveSide(-3.0f * fElapsedTime, true);
			user.draw2D(this);
			bUserInteraction = true;
		}
		if (GetKey(olc::Key::I).bHeld)
		{
			user.moveSide(-3.0f * fElapsedTime, false);
			user.draw2D(this);
			bUserInteraction = true;
		}

		// only draw 3d if user interaction occured
		if (bUserInteraction)
		{
			user.draw3D(this, &maze);

			// draw user angle and position
			float xVec = cosf(user.getAngle());
			float yVec = sinf(user.getAngle());
			FillRect(1250 - 40, 600 - 40, 260, 90, olc::YELLOW);
			DrawRect(1250 - 40, 600 - 40, 260, 90, olc::BLACK);
			DrawCircle(1250, 600, 32, olc::DARK_BLUE);
			DrawLine(1250, 600, 1250 + xVec * 30, 600 + yVec * 30, olc::BLACK);
			DrawString(1300, 590, to_string(user.getPos().x), olc::BLACK, 4);
			DrawString(1400, 590, to_string(user.getPos().y), olc::BLACK, 4);
			DrawString(1300, 630, to_string(fDistanceToEnd), olc::BLACK, 2);

			bUserInteraction = false;
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