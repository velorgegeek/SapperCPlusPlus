#include <vector>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/AllWidgets.hpp>
#include "UserDifficultyWindow.h"
const unsigned short maxBomb  = 999;
const unsigned short maxRows = 99;
const unsigned short maxColomns = 99;


UserDiffWindow::UserDiffWindow() :window(sf::VideoMode(500, 500), L"Выбор сложности", sf::Style::Titlebar | sf::Style::Close), gui(window) {
		loadWidget();
		callback();
	}
void UserDiffWindow::loadWidget() {
	userBomb->setSize(120, 30);
	userBomb->setPosition(50, 50);
	userBomb->setDefaultText(L"Количество бомб");
	userBomb->setInputValidator(tgui::EditBox::Validator::UInt);

	userRows->setSize(120, 30);
	userRows->setPosition(200, 50);
	userRows->setDefaultText(L"Количество строк");
	userRows->setInputValidator(tgui::EditBox::Validator::UInt);

	userColomns->setSize(120, 30);
	userColomns->setPosition(50, 100);
	userColomns->setDefaultText(L"Количество столбцов");
	userColomns->setInputValidator(tgui::EditBox::Validator::UInt);

	// Создаем кнопку "Максимум"
	maxButton->setSize(150, 30);
	maxButton->setPosition(200, 100);
	maxButton->setText(L"Установить максимум");

	// Создаем кнопку "OK"
	okButton->setSize(80, 30);
	okButton->setPosition(160, 160);
	okButton->setText(L"OK");

	gui.add(userBomb);
	gui.add(userRows);
	gui.add(userColomns);
	gui.add(maxButton);
	gui.add(okButton);
}
void UserDiffWindow::callback() {
	maxButton->onPress([this]() {
		userBomb->setText(std::to_string(maxBomb));
		userRows->setText(std::to_string(maxRows));
		userColomns->setText(std::to_string(maxColomns));
	});

	okButton->onPress([this]() {
		result.clear();
		(userColomns->getText().toInt() <= maxColomns) ?
			result.push_back(userColomns->getText().toInt()) :
			result.push_back(maxColomns);

		(userRows->getText().toInt() <= maxRows) ?
			result.push_back(userRows->getText().toInt()) :
			result.push_back(maxRows);

		(userBomb->getText().toInt() <= maxBomb) ?
			result.push_back(userBomb->getText().toInt()) :
			result.push_back(maxBomb);
		okButtonClicked = true;
		window.close();
	});
}
std::vector<int> UserDiffWindow::run() {
	while (window.isOpen() && !okButtonClicked) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed()) {
				window.close();
				return {};
			}
			gui.handleEvent(event);
		}
		window.clear(sf::Color::White);
		gui.draw();
		window.display();
	}
	return result;
}
std::vector<int> UserDiffWindow::UserDiffReturn() {
	UserDiffWindow userWindow;
	return userWindow.run();

}