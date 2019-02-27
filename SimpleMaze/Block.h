#pragma once


class Block
{
public:
	enum BlockType { FREE, BORDER, WALL1, WALL2, START, END };
public:
	Block();
	Block(BlockType);

private:
	static int nCounter;
	int nBlockId;
	bool bVisited;
	BlockType eBlockType;

public:
	bool getVisited();
	void setVisited(bool);

	BlockType getBlockType();
	void setBlockType(BlockType);

	int getId();
};

