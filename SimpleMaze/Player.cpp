#include "Player.h"



Player::Player(Maze *m) : maze(m)
{
	p = maze->getRandomFreeLocation();
	// center the user's position into the middle of the square
	fxPos = (float)p.getX() + 0.5f;
	fyPos = (float)p.getY() + 0.5f;
	vecPath.clear();
	vecPath.push_back({ fxPos, fyPos });
	setVisited(maze);
	fxPosOld = fxPos;
	fyPosOld = fyPos;
	fxVec = cosf(fAngle);
	fyVec = sinf(fAngle);
	fAngle = -3.14159f / 2;
	fUserSize = 0.2f;
	std::vector<path> vecPath; // save the path the user moved around
}


Player::~Player()
{
}


float Player::getAngle() const { return fAngle; }
Location Player::getPos() const { return Location{ (int)fxPos, (int)fyPos }; }
int Player::getPathLength() const { return vecPath.size(); }


void Player::setRandomPosition()
{
	p = maze->getRandomFreeLocation();
	fxPos = (float)p.getX() + 0.5f;
	fyPos = (float)p.getY() + 0.5f;
	fxPosOld = fxPos;
	fyPosOld = fyPos;
	vecPath.clear();
	vecPath.push_back({ fxPos, fyPos, fAngle });
}


void Player::rotate(float fDelta)
{
	fAngle += fDelta;
	fxVec = cosf(fAngle);
	fyVec = sinf(fAngle);
	vecPath.push_back({ fxPos, fyPos, fAngle });
}

void Player::move(float fDistance)
{
	// restrict fDistance to  -0.5 to 0.5 in order to not jump over blocks and border.
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
		setVisited(maze);
	}
}

void Player::moveSide(float fDistance, bool bDirection)
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
		setVisited(maze);
	}
}

bool Player::collisionDetection()
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

/*
void Player::setVisited(Maze* maze)
{
	for (int x = (int)fxPos - 1; x < (int)fxPos + 2; x++)
	{
		for (int y = (int)fyPos - 1; y < (int)fyPos + 2; y++)
		{
			maze->getBlock(x, y)->setVisited(true);
		}
	}
}
*/

void Player::setVisited(Maze* maze)
{
	maze->getBlock((int)fxPos, (int)fyPos)->setVisited(true);
}


void Player::draw2DPath(olc::PixelGameEngine *engine)
{
	for (auto v : vecPath)
	{
		int xp = (int)(v.x * (float)maze->getBlockSize());
		int yp = (int)(v.y * (float)maze->getBlockSize());
		engine->FillCircle(xp, yp, int(fUserSize * maze->getBlockSize()), olc::GREY);
	}
}


void Player::draw2D(olc::PixelGameEngine *engine)
{
	int xp = (int)(fxPos * (float)maze->getBlockSize());
	int yp = (int)(fyPos * (float)maze->getBlockSize());
	int xpOld = (int)(fxPosOld * (float)maze->getBlockSize());
	int ypOld = (int)(fyPosOld * (float)maze->getBlockSize());

	engine->FillCircle(xpOld, ypOld, int(fUserSize * maze->getBlockSize()), maze->getFloorColor());
	engine->FillCircle(xp, yp, int(fUserSize * maze->getBlockSize()), olc::BLUE);
	engine->DrawLine(xp, yp, int(xp + fxVec * (maze->getBlockSize() * fUserSize * 0.5f)),
		(int)(yp + fyVec * (maze->getBlockSize() * fUserSize * 0.5f)), olc::WHITE);
}


void Player::draw3D(olc::PixelGameEngine *engine, Maze *maze)
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

			if (maze->getBlock((int)nTestX, (int)nTestY)->getBlockType() != Block::FREE)
			{
				if (fStepSize < 0.01f)
				{
					bHitWall = true;

					// set bVisited
					maze->getBlock((int)nTestX, (int)nTestY)->setVisited(true);
				}
				else
				{
					fDistanceToWall -= fStepSize;
					fStepSize *= 0.5f;
				}
			}
		}

		// calculate the distance using for drawing the maze, this is necessary to avoid fisheye effect.
		float fDistance = fDistanceToWall * cosf(fAngle - fRayAngle);

		int nShadow = 255 - (int)(fDistance * 15);
		if (nShadow < 0)
			nShadow = 0;
		olc::Pixel pWall;

		switch (maze->getBlock(int(nTestX), int(nTestY))->getBlockType()) {
		case Block::BORDER: pWall = olc::Pixel(nShadow / 2, nShadow / 2, nShadow / 2); break;
		case Block::WALL1: pWall = olc::Pixel(nShadow, 0, 0); break;
		case Block::WALL2: pWall = olc::Pixel(0, 0, nShadow); break;
		case Block::END: pWall = olc::Pixel(0, nShadow, 0); break;
		}

		float nStripe = 0.02f;
		// check if nTestX and nTestY are near an integer number
		float nFractX = nTestX - (int)nTestX;
		float nFractY = nTestY - (int)nTestY;
		// if this is the case, draw a stripe
		if (nFractX < nStripe && nFractY < nStripe || nFractX < nStripe && 1 - nFractY < nStripe
			|| 1 - nFractX < nStripe && nFractY < nStripe || 1 - nFractX < nStripe && 1 - nFractY < nStripe)
			pWall = olc::Pixel(nShadow, nShadow, nShadow);

		int nCeiling = (int)(nScreenHeight / 2.0f - nScreenHeight / fDistance);
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
