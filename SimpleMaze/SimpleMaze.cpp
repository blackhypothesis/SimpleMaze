#define OLC_PGE_APPLICATION
#include"olcPixelGameEngine.h"

using namespace std;

enum BlockType { FREE, BORDER, WALL1, WALL2, WALL3, WALL4, START, END };
enum Direction { NORTH, WEST, SOUTH, EAST };

typedef struct sPoint
{
	int x;
	int y;
} point;

class cBlock
{
public:
	cBlock()
	{
		bTransparent = false;
		eBlockType = FREE;
	}

private:
	bool bTransparent;
	BlockType eBlockType;

public:
	bool getTransparent() { return bTransparent; }
	int getBlockType() { return eBlockType; }

};

typedef struct sPath
{
	float x;
	float y;
	float angle;
} path;

class cUser;

void drawBGandString(olc::PixelGameEngine *engine, int x, int y, string s, olc::Pixel p)
{
	int deltaX = 800;
	int deltaY = 650;
	int fontSize = 34;

	int xpos = deltaX + x * fontSize;
	int ypos = deltaY + y * fontSize;

	engine->FillRect(xpos, ypos, (s.length() + 2) * fontSize, fontSize, olc::WHITE);
	engine->DrawString(xpos + 2, ypos + 2, s, p, 4);
}

class cMaze
{
public:
	cMaze(int xSize, int ySize)
	{
		this->xSize = xSize;
		this->ySize = ySize;
		vecMaze.resize(xSize * ySize);
		initMaze();
		createRandomMaze(100, 10000);
		if (xDraw / xSize < yDraw / ySize)
		{
			nBoxSize = xDraw / xSize;
		}
		else
		{
			nBoxSize = yDraw / ySize;
		}
	}

public:
	int nBoxSize;
	vector<int> vecMaze;

private:
	int xDraw = 800;
	int yDraw = 900;
	int xSize;
	int ySize;
	int nlengthCorridor = 0;
	int nTries = 0;
	olc::Pixel floorColor = olc::DARK_GREEN;

public:
	point getSize() { return { xSize, ySize }; }
	int getIndex(int x, int y)
	{
		if (xSize * y + x > vecMaze.size())
		{
			cout << "error:  ySize = " << ySize << " x = " << x << " y = " << y << "  " << (xSize * y + x) << " > " << vecMaze.size() << endl;
		}
		return (xSize * y + x);
	}
	int getBlockType(int x, int y) { return vecMaze[getIndex(x, y)]; }
 	olc::Pixel getFloorColor() { return floorColor; }
	
	point getEnd()
	{
		for (int i = 0; i < vecMaze.size(); i++) {
			if (vecMaze[i] == END)
			{
				return point{ i % xSize, int(i / xSize) };
			}
		}
		return point{ 0, 0 };
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
					vecMaze[getIndex(x, y)] = WALL1;
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

			if (vecMaze[getIndex(x, y)] == WALL1
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

		// set randomly some areas with WALL2 blocktype
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
					if (vecMaze[getIndex(x, y)] == WALL1)
						vecMaze[getIndex(x, y)] = WALL2;
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
	bool isFree(int x, int y)
	{
		if (vecMaze[getIndex(x, y)] != BORDER && vecMaze[getIndex(x, y)] != WALL1 && vecMaze[getIndex(x, y)] != WALL2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

public:
	void drawGrid(olc::PixelGameEngine *engine)
	{
		engine->FillRect(0, 0, xDraw, yDraw, olc::YELLOW);
		for (int x = 0; x < xSize; x++)
		{
			for (int y = 0; y < ySize; y++)
			{
				engine->DrawRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::GREY);
				if (vecMaze[getIndex(x, y)] == BORDER)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::DARK_GREY);
			}
		}
	}

public:
	void draw(olc::PixelGameEngine *engine)
	{
		engine->FillRect(0, 0, xDraw, yDraw, olc::YELLOW);
		for (int x = 0; x < xSize; x++)
		{
			for (int y = 0; y < ySize; y++)
			{ 
				if (vecMaze[getIndex(x, y)] == BORDER)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::DARK_GREY);
				else if (vecMaze[getIndex(x, y)] == WALL1)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::DARK_RED);
				else if (vecMaze[getIndex(x, y)] == WALL2)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::DARK_BLUE);
				else if (vecMaze[getIndex(x, y)] == FREE)
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, floorColor);
				else if (vecMaze[getIndex(x, y)] == START)
				{
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, olc::WHITE);
					engine->FillCircle(x * nBoxSize + nBoxSize / 2, y * nBoxSize + nBoxSize / 2, nBoxSize / 2, olc::RED);
				}
				else if (vecMaze[getIndex(x, y)] == END)
				{
					engine->FillRect(x * nBoxSize, y * nBoxSize, nBoxSize, nBoxSize, floorColor);
					engine->FillCircle(x * nBoxSize + nBoxSize / 2, y * nBoxSize + nBoxSize / 2, nBoxSize / 2, olc::GREEN);
				}
			}
		}
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
		vecPath.clear();
		vecPath.push_back({ fxPos, fyPos });
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
	float fAngle = -3.14159 / 2;
	float fxVec;          // x vector if fAngle
	float fyVec;          // y vector of fAngle
	float fx90leftVec;    // sidestep left
	float fx90rightVec;   // sidestep right
	float fUserSize = 0.2f;
	vector<path> vecPath; // save the path the user moved around
	cMaze *maze;
	
public:
	float getAngle() { return fAngle; }
	point getPos() { return point{ (int)fxPos, (int)fyPos }; }
	int getPathLength() { return vecPath.size(); }

public:
	void setRandomPosition()
	{
		p = maze->getRandomFreePoint();
		fxPos = (float)p.x + 0.5f;
		fyPos = (float)p.y + 0.5f;
		fxPosOld = fxPos;
		fyPosOld = fyPos;
		vecPath.clear();
		vecPath.push_back({ fxPos, fyPos, fAngle });
	}

public:
	void rotate(float fDelta)
	{
		fAngle += fDelta;
		fxVec = cosf(fAngle);
		fyVec = sinf(fAngle);
		vecPath.push_back({ fxPos, fyPos, fAngle });
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
		if (fxPos != fxPosOld || fyPos != fyPosOld)
		{
			vecPath.push_back({ fxPos, fyPos, fAngle });
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
			else
				vecPath.push_back({ fxPos, fyPos, fAngle });
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
			else
				vecPath.push_back({ fxPos, fyPos, fAngle });
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
	void draw2DPath(olc::PixelGameEngine *engine)
	{
		for (auto v : vecPath)
		{
			int xp = (int)(v.x * (float)maze->nBoxSize);
			int yp = (int)(v.y * (float)maze->nBoxSize);
			engine->FillCircle(xp, yp, int(fUserSize * maze->nBoxSize), olc::GREY);
		}
	}
public:
	void draw2D(olc::PixelGameEngine *engine)
	{
		int xp = (int)(fxPos * (float)maze->nBoxSize);
		int yp = (int)(fyPos * (float)maze->nBoxSize);
		int xpOld = (int)(fxPosOld * (float)maze->nBoxSize);
		int ypOld = (int)(fyPosOld * (float)maze->nBoxSize);

		engine->FillCircle(xpOld, ypOld, int(fUserSize * maze->nBoxSize), maze->getFloorColor());
		engine->FillCircle(xp, yp, int(fUserSize * maze->nBoxSize), olc::BLUE);
		engine->DrawLine(xp, yp, int(xp + fxVec * (maze->nBoxSize * fUserSize * 0.5f)),
			(int)(yp + fyVec * (maze->nBoxSize * fUserSize * 0.5f)), olc::WHITE);
	}

public:
	void draw3D(olc::PixelGameEngine *engine, cMaze *maze)
	{
		int nScreenWidth = 860;
		int nScreenHeight = 500;
		int nXoffset = 820;
		int nYoffset = 50;

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
					{
						bHitWall = true;
					}
					else
					{
						fDistanceToWall -= fStepSize;
						fStepSize *= 0.5f;
					}
				}
			}

			int nShadow = 255 - (int)(fDistanceToWall * 15);
			if (nShadow < 0)
				nShadow = 0;
			olc::Pixel pWall;

			switch (maze->getBlockType(int(nTestX), int(nTestY))) {
				case BORDER: pWall = olc::Pixel(nShadow/2, nShadow/2, nShadow/2); break;
				case WALL1: pWall = olc::Pixel(nShadow, 0, 0); break;
				case WALL2: pWall = olc::Pixel(0, 0, nShadow); break;
				case END: pWall = olc::Pixel(0, nShadow, 0); break;
			}

			float nStripe = 0.02f;
			// check if nTestX and nTestY are near an integer number
			float nFractX = nTestX - (int)nTestX;
			float nFractY = nTestY - (int)nTestY;
			// if this is the case, draw a stripe
			if (nFractX < nStripe && nFractY < nStripe || nFractX < nStripe && 1 - nFractY < nStripe
				|| 1 - nFractX < nStripe && nFractY < nStripe || 1 - nFractX < nStripe && 1 - nFractY < nStripe)
				pWall = olc::Pixel(nShadow, nShadow, nShadow);

			int nCeiling = (int)(nScreenHeight / 2.0f - nScreenHeight / fDistanceToWall);
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

public:
	void replayPath3D(olc::PixelGameEngine *engine, cMaze *maze) 
	{
		float fxPosBackup = fxPos;
		float fyPosBackup = fyPos;
		float fAngleBackup = fAngle;

		for (auto v : vecPath)
		{
			fxPos = v.x;
			fyPos = v.y;
			fAngle = v.angle;
			draw3D(engine, maze);
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
	int nTries = 10000;
	int nMazeWidth = 16;
	int nMazeHeight = 18;
	cMaze maze = cMaze(nMazeWidth, nMazeHeight);
	cUser user = cUser(&maze);
	float fAngle = 0;
	bool bUserInteraction = false;
	bool bDraw2D = false;
	float fDistanceToEnd = 0.0f;

private:
	void drawParams()
	{		
		drawBGandString(this, 0, 0, to_string(maze.getSize().x), olc::RED);
		drawBGandString(this, 0, 1, to_string(maze.getSize().y), olc::RED);
		drawBGandString(this, 5, 0, to_string(user.getPos().x), olc::BLUE);
		drawBGandString(this, 5, 1, to_string(user.getPos().y), olc::BLUE);
		drawBGandString(this, 10, 0, to_string(maze.getEnd().x), olc::GREEN);
		drawBGandString(this, 10, 1, to_string(maze.getEnd().y), olc::GREEN);
		drawBGandString(this, 0, 3, to_string(fDistanceToEnd), olc::BLACK);
		drawBGandString(this, 0, 4, to_string(user.getPathLength()), olc::BLACK);
	}

public:
	bool OnUserCreate() override
	{
		FillRect(0, 0, 1699, 899, olc::YELLOW);
		maze.createRandomMaze(nlengthCorridor, nTries);
		user.setRandomPosition();
		user.draw3D(this, &maze);
		maze.drawGrid(this);
		user.draw2D(this);
		drawParams();
		return true;
	}

public:
	bool OnUserUpdate(float fElapsedTime) override
	{
		// draw 2D?
		if (GetKey(olc::Key::M).bReleased)
		{
			bDraw2D = !bDraw2D;
			if (bDraw2D)
				maze.draw(this);
			else
				maze.drawGrid(this);
			user.draw2DPath(this);
			user.draw2D(this);
		}
		// new game
		if (GetKey(olc::Key::N).bReleased)
		{
			bDraw2D = false;
			maze.createRandomMaze(nlengthCorridor, nTries);
			user.setRandomPosition();
			user.draw3D(this, &maze);
			maze.drawGrid(this);
			user.draw2D(this);
			drawParams();
		}

		// grow maze x and y
		if (GetKey(olc::Key::V).bReleased)
		{
			bDraw2D = false;
			nMazeHeight++;
			nMazeWidth++;
			maze = cMaze(nMazeWidth, nMazeHeight);
			maze.createRandomMaze(nlengthCorridor, nTries);
			user.setRandomPosition();
			user.draw3D(this, &maze);
			maze.drawGrid(this);
			user.draw2D(this);
			drawParams();
		}

		//shrink maze x and y
		if (GetKey(olc::Key::B).bReleased)
		{
			bDraw2D = false;
			nMazeHeight--;
			nMazeWidth--;
			if (nMazeHeight < 5) { nMazeHeight = 5; }
			if (nMazeWidth < 5) { nMazeWidth = 5; }
			maze = cMaze(nMazeWidth, nMazeHeight);
			maze.createRandomMaze(nlengthCorridor, nTries);
			user.setRandomPosition();
			user.draw3D(this, &maze);
			maze.drawGrid(this);
			user.draw2D(this);
			drawParams();
		}

		// grow maze x
		if (GetKey(olc::Key::X).bReleased)
		{
			bDraw2D = false;
			nMazeWidth++;
			maze = cMaze(nMazeWidth, nMazeHeight);
			maze.createRandomMaze(nlengthCorridor, nTries);
			user.setRandomPosition();
			user.draw3D(this, &maze);
			maze.drawGrid(this);
			user.draw2D(this);
			drawParams();
		}

		// grow maze y
		if (GetKey(olc::Key::Z).bReleased)
		{
			bDraw2D = false;
			nMazeHeight++;
			maze = cMaze(nMazeWidth, nMazeHeight);
			maze.createRandomMaze(nlengthCorridor, nTries);
			user.setRandomPosition();
			user.draw3D(this, &maze);
			maze.drawGrid(this);
			user.draw2D(this);
			drawParams();
		}

		// get distance to end
		if (GetKey(olc::Key::O).bReleased)
		{
			point pUser = user.getPos();
			point pEnd = maze.getEnd();
			int nDeltaX = pUser.x - pEnd.x;
			int nDeltaY = pUser.y - pEnd.y;
			fDistanceToEnd = sqrtf((float)(nDeltaX * nDeltaX + nDeltaY * nDeltaY));
		}

		// User Navigation
		// Rotate
		if (GetKey(olc::Key::A).bHeld)
		{
			user.rotate(-2.0f * fElapsedTime);
			bUserInteraction = true;
		}
		if (GetKey(olc::Key::D).bHeld)
		{
			user.rotate(2.0f * fElapsedTime);
			bUserInteraction = true;
		}

		// Move forward - backward
		if (GetKey(olc::Key::W).bHeld)
		{
			user.move(3.0f * fElapsedTime);
			bUserInteraction = true;
		}
		if (GetKey(olc::Key::S).bHeld)
		{
			user.move(-3.0f * fElapsedTime);
			bUserInteraction = true;
		}

		// move: shift left - right
		if (GetKey(olc::Key::P).bHeld)
		{
			bUserInteraction = true;
			user.moveSide(-3.0f * fElapsedTime, true);
		}
		if (GetKey(olc::Key::I).bHeld)
		{
			bUserInteraction = true;
			user.moveSide(-3.0f * fElapsedTime, false);
		}

		// replay user path in 3D
		if (GetKey(olc::Key::R).bReleased)
		{
			user.replayPath3D(this, &maze);
		}

		// only draw 3d if user interaction occured
		if (bUserInteraction)
		{
			user.draw3D(this, &maze);
			user.draw2D(this);
				
			// draw user angle and position
			float xVec = cosf(user.getAngle());
			float yVec = sinf(user.getAngle());
			FillRect(1250 - 40, 600 - 40, 260, 110, olc::YELLOW);
			DrawCircle(1250, 600, 32, olc::DARK_BLUE);
			DrawLine(1250, 600, 1250 + xVec * 32, 600 + yVec * 32, olc::DARK_BLUE);

			drawParams();
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