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
	cBlock() : bTransparent(false), eBlockType(FREE) {}


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

class cPlayer;

void drawBGandString(olc::PixelGameEngine *engine, int x, int y, string s, olc::Pixel p)
{
	int deltaX = 910;
	int deltaY = 570;
	int fontSize = 34;

	int xpos = deltaX + x * fontSize;
	int ypos = deltaY + y * fontSize;

	engine->FillRect(xpos, ypos, (s.length() + 2) * fontSize, fontSize, olc::YELLOW);
	engine->DrawString(xpos + 2, ypos + 2, s, p, 4);
}

class cMaze
{
public:
	cMaze(int x, int y) : xSize(x), ySize(y)
	{
		vecMaze.resize(xSize * ySize);
		initMaze();
		createRandomMaze(100);
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
	olc::Pixel floorColor = olc::DARK_GREEN;

public:
	point getSize() const { return { xSize, ySize }; }
	int getIndex(int x, int y) const {  return (xSize * y + x); }
	int getBlockType(int x, int y) const { return vecMaze[getIndex(x, y)]; }

	float getRatio() const
	{
		float nFree = 0;
		for (auto v : vecMaze) 
			if (v == 0) nFree++; 
		// ratio free blocks versus occupied blocks without border
		return nFree / ((xSize - 2) * (ySize - 2));
	}

 	olc::Pixel getFloorColor() const { return floorColor; }
	
	
	point getEnd() const
	{
		for (int i = 0; i < (int)vecMaze.size(); i++) {
			if (vecMaze[i] == END)
			{
				return point{ i % xSize, int(i / xSize) };
			}
		}
		return point{ 0, 0 };
	}

public:
	point getRandomFreePoint() const
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
	void createRandomMaze(int nlengthCorridor)
	{
		this->nlengthCorridor = nlengthCorridor;
		int nIterations = xSize * ySize * 5;

		bool isFirst = true;
		srand((int)time(0));

		initMaze();

		while (nIterations > 0)
		{
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

			nIterations--;
			
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
		int i = (int)sqrt(xSize * ySize);
		while (i > 0)
		{
			i--;
			int x = rand() % (xSize - 2) + 1;
			int y = rand() % (ySize - 2) + 1;

			if (vecMaze[getIndex(x, y)] == WALL1
				&& vecMaze[getIndex(x + 1, y)] == vecMaze[getIndex(x - 1, y)]
				&& vecMaze[getIndex(x, y + 1)] == vecMaze[getIndex(x, y - 1)])
			{
				if (vecMaze[getIndex(x + 1, y)] != vecMaze[getIndex(x, y + 1)])
					vecMaze[getIndex(x, y)] = FREE;
			}
		}
		
		// create rooms
		for (int i = 0; i < sqrt(xSize * ySize) / 4; i++)
		{
			int xp = rand() % (xSize - 2) + 1;
			int yp = rand() % (ySize - 2) + 1;
			int xWidth = 3 + rand() % (xSize/5);
			int yHeight = 3 + rand() % (ySize/5);

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
		for (int i = 0; i < sqrt(xSize * ySize) / 2; i++)
		{
			int xp = rand() % (xSize - 2) + 1;
			int yp = rand() % (ySize - 2) + 1;
			int xWidth = 3 + rand() % (xSize / 5);
			int yHeight = 3 + rand() % (ySize / 5);

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


class cPlayer
{
public:
	cPlayer(cMaze *m) : maze(m)
	{
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
	cMaze *maze;
	point p;
	float fxPos;
	float fyPos;
	float fxPosOld;
	float fyPosOld;
	float fAngle = -3.14159f / 2;
	float fxVec;          // x vector if fAngle
	float fyVec;          // y vector of fAngle
	float fx90leftVec;    // sidestep left
	float fx90rightVec;   // sidestep right
	float fUserSize = 0.2f;
	vector<path> vecPath; // save the path the user moved around
	
public:
	float getAngle() const { return fAngle; }
	point getPos() const { return point{ (int)fxPos, (int)fyPos }; }
	int getPathLength() const { return vecPath.size(); }

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
		// restrict fDistance to  -0.5 to 0.5 in order to not jump over boxes and border.
		if (fDistance > 0.5f) { fDistance = 0.5f; }
		else if (fDistance < -0.5f) { fDistance = -0.5f; }
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
		fxPosOld = fxPos;
		fyPosOld = fyPos;
		
		if (bDirection)
		// right shift
		{
			fxPos += fyVec * fDistance;
			fyPos += -fxVec * fDistance;
		}
		else
		// left shift
		{
			fxPos += -fyVec * fDistance;
			fyPos += fxVec * fDistance;
		}
		if (collisionDetection())
		{
			// try to slide along the y axis
			fxPos = fxPosOld;
			fyPos += fyVec * fDistance;

			if (collisionDetection())
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
	int nTries = 50000;
	int nMazeWidth = 40;
	int nMazeHeight = 45;
	cMaze maze = cMaze(nMazeWidth, nMazeHeight);
	cPlayer user = cPlayer(&maze);
	float fAngle = 0;
	bool bUserInteraction = false;
	bool bDraw2D = false;
	float fDistanceToEnd = 0.0f;
	int mouseX = GetMouseX();
	int mouseY = GetMouseY();
	int mouseOldX = mouseX;
	int mouseOldY = mouseY;

private:
	void drawParams()
	{		
		drawBGandString(this, 0, 0, to_string(maze.getSize().x), olc::DARK_GREY);
		drawBGandString(this, 0, 1, to_string(maze.getSize().y), olc::DARK_GREY);
		drawBGandString(this, 4, 0, to_string(user.getPos().x), olc::BLUE);
		drawBGandString(this, 4, 1, to_string(user.getPos().y), olc::BLUE);
		//drawBGandString(this, 8, 0, to_string(maze.getEnd().x), olc::GREEN);
		//drawBGandString(this, 8, 1, to_string(maze.getEnd().y), olc::GREEN);
		drawBGandString(this, 0, 2, to_string(fDistanceToEnd), olc::BLACK);
		drawBGandString(this, 0, 3, to_string(user.getPathLength()), olc::BLACK);
		drawBGandString(this, 12, 0, to_string(mouseX), olc::BLACK);
		drawBGandString(this, 12, 1, to_string(mouseY), olc::BLACK);
	}

public:
	bool OnUserCreate() override
	{
		FillRect(0, 0, 1699, 899, olc::YELLOW);
		{
			int deltaX = 1100;
			int deltaY = 720;
			DrawString(deltaX, deltaY, "[A][S][D][W][I][P]  move", olc::BLACK, 2);
			DrawString(deltaX, deltaY + 20, "Mouse               move", olc::BLACK, 2);
			DrawString(deltaX, deltaY + 40, "[M]                 toggle draw 2D", olc::BLACK, 2);
			DrawString(deltaX, deltaY + 60, "[O]                 distance to end", olc::BLACK, 2);
			DrawString(deltaX, deltaY + 80, "[X][Y][V]           grow maze", olc::BLACK, 2);
			DrawString(deltaX, deltaY + 100, "[B]                 shrink maze", olc::BLACK, 2);
		}
		maze.createRandomMaze(nlengthCorridor);
		user.setRandomPosition();
		user.draw3D(this, &maze);
		maze.drawGrid(this);
		user.draw2D(this);
		drawParams();
		drawBGandString(this, 12, 2, to_string(maze.getRatio()), olc::BLUE);
		return true;
	}

public:
	bool OnUserUpdate(float fElapsedTime) override
	{
		mouseX = GetMouseX();
		mouseY = GetMouseY();

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
			maze.createRandomMaze(nlengthCorridor);
			user.setRandomPosition();
			user.draw3D(this, &maze);
			maze.drawGrid(this);
			user.draw2D(this);
			drawParams();
			drawBGandString(this, 12, 2, to_string(maze.getRatio()), olc::BLUE);
		}

		// grow maze x and y
		if (GetKey(olc::Key::V).bReleased)
		{
			bDraw2D = false;
			nMazeHeight += 10;
			nMazeWidth += 10;
			maze = cMaze(nMazeWidth, nMazeHeight);
			maze.createRandomMaze(nlengthCorridor);
			user.setRandomPosition();
			user.draw3D(this, &maze);
			maze.drawGrid(this);
			user.draw2D(this);
			drawParams();
			drawBGandString(this, 12, 2, to_string(maze.getRatio()), olc::BLUE);

		}

		//shrink maze x and y
		if (GetKey(olc::Key::B).bReleased)
		{
			bDraw2D = false;
			nMazeHeight -= 10;
			nMazeWidth -= 10;
			if (nMazeHeight < 5) { nMazeHeight = 5; }
			if (nMazeWidth < 5) { nMazeWidth = 5; }
			maze = cMaze(nMazeWidth, nMazeHeight);
			maze.createRandomMaze(nlengthCorridor);
			user.setRandomPosition();
			user.draw3D(this, &maze);
			maze.drawGrid(this);
			user.draw2D(this);
			drawParams();
			drawBGandString(this, 12, 2, to_string(maze.getRatio()), olc::BLUE);

		}

		// grow maze x
		if (GetKey(olc::Key::X).bReleased)
		{
			bDraw2D = false;
			nMazeWidth++;
			maze = cMaze(nMazeWidth, nMazeHeight);
			maze.createRandomMaze(nlengthCorridor);
			user.setRandomPosition();
			user.draw3D(this, &maze);
			maze.drawGrid(this);
			user.draw2D(this);
			drawParams();
			drawBGandString(this, 12, 2, to_string(maze.getRatio()), olc::BLUE);

		}

		// grow maze y
		if (GetKey(olc::Key::Z).bReleased)
		{
			bDraw2D = false;
			nMazeHeight++;
			maze = cMaze(nMazeWidth, nMazeHeight);
			maze.createRandomMaze(nlengthCorridor);
			user.setRandomPosition();
			user.draw3D(this, &maze);
			maze.drawGrid(this);
			user.draw2D(this);
			drawParams();
			drawBGandString(this, 12, 2, to_string(maze.getRatio()), olc::BLUE);

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
		if (mouseX != mouseOldX && mouseX > 820 && mouseX < 1680 && mouseY > 50 && mouseY < 550)
		{
			user.rotate(0.3f * fElapsedTime * (mouseX - mouseOldX));
			mouseOldX = mouseX;
			bUserInteraction = true;
		}

		// Move forward - backward
		if (GetKey(olc::Key::W).bHeld || GetMouse(0).bHeld)
		{
			user.move(3.0f * fElapsedTime);
			bUserInteraction = true;
		}
		if (GetKey(olc::Key::S).bHeld || GetMouse(1).bHeld)
		{
			user.move(-3.0f * fElapsedTime);
			bUserInteraction = true;
		}
		if (mouseY != mouseOldY && mouseX > 820 && mouseX < 1680 && mouseY > 50 && mouseY < 550)
		{
			user.move(-0.3f * fElapsedTime * (mouseY - mouseOldY));
			mouseOldY = mouseY;
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
				
			// draw user angle
			float xVec = cosf(user.getAngle());
			float yVec = sinf(user.getAngle());
			FillRect(850 - 40, 600 - 40, 260, 110, olc::YELLOW);
			DrawCircle(850, 600, 32, olc::DARK_BLUE);
			DrawLine(850, 600, 850 + (int)(xVec * 32), 600 + (int)(yVec * 32), olc::DARK_BLUE);
			// draw misc. params
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