#pragma once
#ifndef DIFFICULTY_H
#define DIFFICULTY_H

#include "MapSize.h"
#include <String>
class Difficulty {
public:
	enum diff {Beginner,Skilled, minesweeper};
	NumOfCell size;
	unsigned int countBomb;
	std::string nameDifficilty;

	Difficulty() = default;
	Difficulty(NumOfCell n, unsigned int c, std::string name) : size(n), countBomb(c), nameDifficilty(name) {};
}; 
#endif // !1