#include "Statistics.h"
#include "json.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include  <iostream>
#include <chrono>

using json = nlohmann::json;

Statistics::Statistics() : nodes(), jArrayGame(json::array()) {
	countWin = 0;
	countLoss = 0;
	totalGames = 0;
	winrate = 0.0f;
	directory = "Statistics";
	if (!exists(directory)) {
		std::filesystem::create_directory(directory);
	}
	pathToFile = directory / "statistics.json";
	if (!exists(pathToFile)) {

		std::ofstream file;
		file.open(pathToFile);
		file.close();

	}
	load();
}

statisticsNode::statisticsNode(float gameTime,Difficulty dif,GameResult result) : diff(dif), gameTime(gameTime) ,result(result),DateTimeGame(time(0)) {
	
};
statisticsNode statisticsNode::fromJson(const json& j) {
	statisticsNode node;
	node.DateTimeGame = j["DateTimeGame"];
	node.gameTime = j["DurationGame"];
	node.diff.nameDifficilty = j["Difficulty"]["name"];
	node.diff.countBomb = j["Difficulty"]["bomb"];
	node.diff.size.rows = j["Difficulty"]["rows"];
	node.result = (j["result"] == "WIN")? GameResult::WIN : GameResult::LOSS;
	node.diff.size.columns = j["Difficulty"]["columns"];
	return node;
}

statisticsError Statistics::load() {
	std::ifstream f(pathToFile);
	if (!f.is_open()) {
		return statisticsError::FileNotExist;
	}
	if (f.peek() == std::ifstream::traits_type::eof()) {
		// Файл пустой - инициализируем пустыми значениями
		nodes.clear();
		countWin = 0;
		countLoss = 0;
		totalGames = 0;
		winrate = 0.0f;
		
		jArrayGame = json::array(); // Создаем пустой JSON объект
		return OK;
	}
	json j;
	f >> j;
	nodes.clear();
	countWin = j["TotalStat"]["Win"];
	countLoss = j["TotalStat"]["Loss"];
	totalGames = j["TotalStat"]["totalGames"];
	winrate = j["TotalStat"]["Winrate"];
	jArrayGame = j["games"];
	for (auto& i : j["games"]) {
		nodes.push_back(statisticsNode::fromJson(i));
	}

	f.close();
	return OK;
}
json statisticsNode::toJson() const {
	return {
		{"Difficulty",{
			{"name",diff.nameDifficilty},
			{"columns",diff.size.columns},
			{"rows",diff.size.rows},
			{"bomb",diff.countBomb}
			}},
		{"result", (result == GameResult::WIN) ? "WIN" : "LOSS"},
		{"DateTimeGame", DateTimeGame},
		{"DurationGame", gameTime}
	};
}
void Statistics::addWin() {
	countWin++;
	totalGames++;
	winUpdate();
}
void Statistics::addLose(){
	countLoss++;
	totalGames++;
	winUpdate();
}
void Statistics::winUpdate(){
	if (totalGames == 0) return;
	double q = static_cast<double>(countWin) / totalGames;
	winrate = std::round(q * 100.0f) / 100.0f;
}
statisticsError Statistics::save() {
	std::ofstream file(pathToFile);
	if (!file.is_open()) { return statisticsError::FileNotExist; }
	json j;
	j["TotalStat"] ={
		{"Win",countWin},
		{"Loss",countLoss},
		{"totalGames",totalGames},
		{"Winrate",winrate}
	};


	j["games"] = jArrayGame;
	file <<j.dump(2);
	file.close();
	return statisticsError::OK;
}
std::string Statistics::getTotalStats() {
	return u8"Кол-во игр: " + std::to_string(totalGames) + u8" Побед: " + std::to_string(countWin) + "\n"
		+ u8"Поражений: " + std::to_string(countLoss)
		+ u8" Винрейт " + std::to_string(winrate);

}
std::vector<std::string> Statistics::statisticsBuilder() {
	std::vector<std::string> stats;
	for (auto& i : nodes) {
		std::tm dateTime = {};

		// Безопасная версия для Windows
		errno_t err = localtime_s(&dateTime, &i.DateTimeGame);
		if (err != 0) {
			// Обработка ошибки, например:
			continue; // пропустить некорректную дату
		}

		char date[20];
		strftime(date, sizeof(date), "%d-%m-%Y", &dateTime);

		std::string q; 

		if (i.result == GameResult::WIN) {
			q = u8"Вы прошли сапер за ";
			q += std::to_string(static_cast<int>(i.gameTime)) + u8" секунд ";
		}
		else {
			q = u8"Вы проиграли в сапер ";
		}

		q += u8"играя на сложности " + i.diff.nameDifficilty + ",\n"
			+ u8"размер карты " + std::to_string(i.diff.size.columns)
			+ "x" + std::to_string(i.diff.size.rows) +
			u8", кол-во бомб " + std::to_string(i.diff.countBomb)
			+ u8", дата: " + date;
		stats.push_back(q);
	}
	return stats;
}
void Statistics::addStatistics(statisticsNode s) {
	if (s.result == GameResult::WIN) {
		addWin();

	}
	else {
		addLose();
	}
	winUpdate();
	nodes.push_back(s);
	jArrayGame.push_back(s.toJson());
	save();
}
Statistics::~Statistics() {
	save();
};
