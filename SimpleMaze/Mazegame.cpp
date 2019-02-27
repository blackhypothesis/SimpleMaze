#include "MazeGame.h"



MazeGame::MazeGame() : player(&maze)
{
	sAppName = "Simple Maze";

	nMazeWidth = 20;
	nMazeHeight = 24;
	maze = Maze(nMazeWidth, nMazeHeight);
	fAngle = 0;
	bPlayerInteraction = false;
	bDraw2D = false;
	fDistanceToEnd = 0.0f;
	mouseX = GetMouseX();
	mouseY = GetMouseY();
	mouseOldX = mouseX;
	mouseOldY = mouseY;
}


MazeGame::~MazeGame()
{
}


void MazeGame::drawBGandString(olc::PixelGameEngine *engine, int x, int y, std::string s, olc::Pixel p)
{
	int deltaX = 910;
	int deltaY = 570;
	int fontSize = 34;

	int xpos = deltaX + x * fontSize;
	int ypos = deltaY + y * fontSize;

	engine->FillRect(xpos, ypos, (s.length() + 2) * fontSize, fontSize, olc::YELLOW);
	engine->DrawString(xpos + 2, ypos + 2, s, p, 4);
}


void MazeGame::drawParams()
{
	drawBGandString(this, 0, 0, std::to_string(maze.getXSize()), olc::DARK_GREY);
	drawBGandString(this, 0, 1, std::to_string(maze.getYSize()), olc::DARK_GREY);
	//drawBGandString(this, 4, 0, std::to_string(player.getPos().x), olc::BLUE);
	//drawBGandString(this, 4, 1, std::to_string(player.getPos().y), olc::BLUE);
	//drawBGandString(this, 8, 0, to_string(maze.getEnd().x), olc::GREEN);
	//drawBGandString(this, 8, 1, to_string(maze.getEnd().y), olc::GREEN);
	drawBGandString(this, 0, 2, std::to_string(fDistanceToEnd), olc::BLACK);
	//drawBGandString(this, 0, 3, std::to_string(std::floor((player.getPathLength() * 100) + .5) / 100), olc::BLACK);
	drawBGandString(this, 12, 0, std::to_string(mouseX), olc::BLACK);
	drawBGandString(this, 12, 1, std::to_string(mouseY), olc::BLACK);
}


bool MazeGame::OnUserCreate()
{
	FillRect(0, 0, 1699, 899, olc::YELLOW);
	{
		int deltaX = 900;
		int deltaY = 720;
		DrawString(deltaX, deltaY, "[A][S][D][W][I][P] Mouse    move", olc::BLACK, 2);
		DrawString(deltaX, deltaY + 20, "[M]                     toggle draw 2D", olc::BLACK, 2);
		DrawString(deltaX, deltaY + 40, "[O]                     distance to end", olc::BLACK, 2);
		DrawString(deltaX, deltaY + 60, "[X][Y][V]               grow maze", olc::BLACK, 2);
		DrawString(deltaX, deltaY + 80, "[B]                     shrink maze", olc::BLACK, 2);
		DrawString(deltaX, deltaY + 100, "[1][2][3]               init, corridor, loop", olc::BLACK, 2);
		DrawString(deltaX, deltaY + 120, "[4][5][6]               room, WALL2, 10 corridors", olc::BLACK, 2);
	}

	maze.createRandomMaze(nLengthCorridor);
	maze.draw2D(this);
	drawParams();

	maze.createRandomMaze(nLengthCorridor);
	player.setRandomPosition();
	player.draw3D(this, &maze);
	//maze.drawGrid(this);
	player.draw2D(this);
	drawParams();
	drawBGandString(this, 12, 2, std::to_string(maze.getRatio()), olc::BLUE);
	return true;

}

bool MazeGame::OnUserUpdate(float fElapsedTime)
{
	mouseX = GetMouseX();
	mouseY = GetMouseY();

	// draw 2D?
	if (GetKey(olc::Key::M).bReleased)
	{
		bDraw2D = !bDraw2D;
		if (bDraw2D)
			maze.draw2D(this);
		else
			maze.drawGrid(this);
		player.draw2DPath(this);
		player.draw2D(this);
	}
	// new game
	if (GetKey(olc::Key::N).bReleased)
	{
		bDraw2D = false;
		maze.createRandomMaze(nLengthCorridor);
		player.setRandomPosition();
		player.draw3D(this, &maze);
		maze.drawGrid(this);
		player.draw2D(this);
		drawParams();
		drawBGandString(this, 12, 2, std::to_string(maze.getRatio()), olc::BLUE);
	}

	// grow maze x and y
	if (GetKey(olc::Key::V).bReleased)
	{
		bDraw2D = false;
		nMazeHeight += 10;
		nMazeWidth += 10;
		maze = Maze(nMazeWidth, nMazeHeight);
		player.setRandomPosition();
		player.draw3D(this, &maze);
		maze.drawGrid(this);
		player.draw2D(this);
		drawParams();
		drawBGandString(this, 12, 2, std::to_string(maze.getRatio()), olc::BLUE);

	}

	//shrink maze x and y
	if (GetKey(olc::Key::B).bReleased)
	{
		bDraw2D = false;
		nMazeHeight -= 10;
		nMazeWidth -= 10;
		if (nMazeHeight < 5) { nMazeHeight = 5; }
		if (nMazeWidth < 5) { nMazeWidth = 5; }
		maze = Maze(nMazeWidth, nMazeHeight);
		player.setRandomPosition();
		player.draw3D(this, &maze);
		maze.drawGrid(this);
		player.draw2D(this);
		drawParams();
		drawBGandString(this, 12, 2, std::to_string(maze.getRatio()), olc::BLUE);

	}

	// grow maze x
	if (GetKey(olc::Key::X).bReleased)
	{
		bDraw2D = false;
		nMazeWidth++;
		maze = Maze(nMazeWidth, nMazeHeight);
		player.setRandomPosition();
		player.draw3D(this, &maze);
		maze.drawGrid(this);
		player.draw2D(this);
		drawParams();
		drawBGandString(this, 12, 2, std::to_string(maze.getRatio()), olc::BLUE);

	}

	// grow maze y
	if (GetKey(olc::Key::Z).bReleased)
	{
		bDraw2D = false;
		nMazeHeight++;
		maze = Maze(nMazeWidth, nMazeHeight);
		player.setRandomPosition();
		player.draw3D(this, &maze);
		maze.drawGrid(this);
		player.draw2D(this);
		drawParams();
		drawBGandString(this, 12, 2, std::to_string(maze.getRatio()), olc::BLUE);

	}

	// get distance to end
	if (GetKey(olc::Key::O).bReleased)
	{
		Location pPlayer = player.getPos();
		Location pEnd = maze.getEnd();
		int nDeltaX = pPlayer.getX() - pEnd.getX();
		int nDeltaY = pPlayer.getY() - pEnd.getY();
		fDistanceToEnd = sqrtf((float)(nDeltaX * nDeltaX + nDeltaY * nDeltaY));
	}

	// User Navigation
	// Rotate
	if (GetKey(olc::Key::A).bHeld)
	{
		player.rotate(-2.0f * fElapsedTime);
		bPlayerInteraction = true;
	}
	if (GetKey(olc::Key::D).bHeld)
	{
		player.rotate(2.0f * fElapsedTime);
		bPlayerInteraction = true;
	}
	if (mouseX != mouseOldX && mouseX > 820 && mouseX < 1680 && mouseY > 50 && mouseY < 550)
	{
		player.rotate(0.3f * fElapsedTime * (mouseX - mouseOldX));
		mouseOldX = mouseX;
		bPlayerInteraction = true;
	}

	// Move forward - backward
	if (GetKey(olc::Key::W).bHeld || GetMouse(0).bHeld)
	{
		player.move(3.0f * fElapsedTime);
		bPlayerInteraction = true;
	}
	if (GetKey(olc::Key::S).bHeld || GetMouse(1).bHeld)
	{
		player.move(-3.0f * fElapsedTime);
		bPlayerInteraction = true;
	}
	if (mouseY != mouseOldY && mouseX > 820 && mouseX < 1680 && mouseY > 50 && mouseY < 550)
	{
		player.move(-0.3f * fElapsedTime * (mouseY - mouseOldY));
		mouseOldY = mouseY;
		bPlayerInteraction = true;
	}

	// move: shift left - right
	if (GetKey(olc::Key::P).bHeld)
	{
		bPlayerInteraction = true;
		player.moveSide(-3.0f * fElapsedTime, true);
	}
	if (GetKey(olc::Key::I).bHeld)
	{
		bPlayerInteraction = true;
		player.moveSide(-3.0f * fElapsedTime, false);
	}

	// replay player path in 3D
	if (GetKey(olc::Key::R).bReleased)
	{
		// player.replayPath3D(this, &maze);
	}

	// ----- create maze step by step
	// ------------------------------
	// create new maze
	if (GetKey(olc::Key::K1).bReleased)
	{
		maze.initMaze();
		maze.createCorridor(nLengthCorridor);
		player.setRandomPosition();
		player.draw3D(this, &maze);
		maze.draw2D(this);
		player.draw2D(this);
		drawParams();
		drawBGandString(this, 12, 2, std::to_string(maze.getRatio()), olc::BLUE);
	}

	// create corridor
	if (GetKey(olc::Key::K2).bReleased)
	{
		maze.createCorridor(nLengthCorridor);
		player.draw3D(this, &maze);
		maze.draw2D(this);
		player.draw2D(this);
		drawParams();
		drawBGandString(this, 12, 2, std::to_string(maze.getRatio()), olc::BLUE);
	}

	// connect corridors (create loop)
	if (GetKey(olc::Key::K3).bReleased)
	{
		maze.connectCorridors();
		player.draw3D(this, &maze);
		maze.draw2D(this);
		player.draw2D(this);
		drawParams();
		drawBGandString(this, 12, 2, std::to_string(maze.getRatio()), olc::BLUE);
	}

	// create room
	if (GetKey(olc::Key::K4).bReleased)
	{
		maze.createRoom();
		player.draw3D(this, &maze);
		maze.draw2D(this);
		player.draw2D(this);
		drawParams();
		drawBGandString(this, 12, 2, std::to_string(maze.getRatio()), olc::BLUE);
	}

	// create WALL2
	if (GetKey(olc::Key::K5).bReleased)
	{
		maze.createWall2();
		player.draw3D(this, &maze);
		maze.draw2D(this);
		player.draw2D(this);
		drawParams();
		drawBGandString(this, 12, 2, std::to_string(maze.getRatio()), olc::BLUE);
	}

	// create n corridors
	if (GetKey(olc::Key::K6).bHeld)
	{
		for (int i = 0; i < 10; i++)
			maze.createCorridor(nLengthCorridor);

		player.draw3D(this, &maze);
		maze.draw2D(this);
		player.draw2D(this);
		drawParams();
		drawBGandString(this, 12, 2, std::to_string(maze.getRatio()), olc::BLUE);
	}

	// ----- only draw 3d if player interaction occured
	// ----------------------------------------------
	if (bPlayerInteraction)
	{
		player.draw3D(this, &maze);
		player.draw2D(this);

		// draw player angle
		float xVec = cosf(player.getAngle());
		float yVec = sinf(player.getAngle());
		FillRect(850 - 40, 600 - 40, 260, 110, olc::YELLOW);
		DrawCircle(850, 600, 32, olc::DARK_BLUE);
		DrawLine(850, 600, 850 + (int)(xVec * 32), 600 + (int)(yVec * 32), olc::DARK_BLUE);
		// draw misc. params
		drawParams();

		bPlayerInteraction = false;
	}
	return true;
}