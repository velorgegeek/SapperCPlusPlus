#pragma once
#include <time.h>
#include "difficulty.h"
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include "json.hpp"
using json = nlohmann::json;
enum statisticsError{OK,FileNotExist,InvalidFile};
enum GameResult { WIN, LOSS };
class statisticsNode {
public:
	time_t DateTimeGame;
	float gameTime;
	Difficulty diff;
	GameResult result;
	statisticsNode(float gameTime, Difficulty dif,GameResult);
	statisticsNode() = default; // Нужен для десериализации

	json toJson() const;
	static statisticsNode fromJson(const json& j);
};
	class Statistics {
	public:
		json jArrayGame;
		short countWin =0;
		short countLoss=0;
		int totalGames=0;
		float winrate=0;
		std::vector<statisticsNode> nodes;
		std::filesystem::path directory;
		std::filesystem::path pathToFile;
		std::vector<std::string> statisticsBuilder();
		Statistics();
		statisticsError save();
		statisticsError load();
		void addStatistics(statisticsNode s);
		~Statistics();
		void addWin();
		void addLose();
		void winUpdate();
		std::string getTotalStats();
	};