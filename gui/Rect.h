#pragma once
class Rect
{
public:
	int x, y, width, height;

	Rect(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}

	bool contains(int px, int py) {
		return px >= x && px <= x + width, py >= y, py <= y + height;
	}
};

