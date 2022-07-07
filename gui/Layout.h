#include <vector>
#include "GUISprite.h"

#pragma once
class Layout
{
	std::vector<GUISprite> sprites;

	GUISprite getSpriteAt(int x, int y) {
		for (auto sprite : sprites) {
			if (sprite.rect.contains(x, y)) {
				return sprite;
			}
		}
	}
};

