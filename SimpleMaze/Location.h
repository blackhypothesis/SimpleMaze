#pragma once



class Location
{
public:
	Location();
	Location(int, int);

private:
	int x;
	int y;

public:
	int getX() const;
	void setX(int);
	int getY() const;
	void setY(int);
};

