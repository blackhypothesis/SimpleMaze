#include "Block.h"

int Block::nCounter = 0;


Block::Block() : bVisited(false), eBlockType(BORDER), nBlockId(nCounter++) {}
Block::Block(BlockType bt) : bVisited(false), eBlockType(bt), nBlockId(nCounter++) {}

bool Block::getVisited() { return bVisited; }
void Block::setVisited(bool v) { bVisited = v; }

Block::BlockType Block::getBlockType() { return eBlockType; }
void Block::setBlockType(BlockType bt) { eBlockType = bt; }

int Block::getId() { return nBlockId; }