#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/AllWidgets.hpp>
class UserDiffWindow {
	sf::RenderWindow window;
	tgui::Gui gui;
	tgui::EditBox::Ptr userBomb = tgui::EditBox::create();
	tgui::EditBox::Ptr userRows = tgui::EditBox::create();
	tgui::EditBox::Ptr userColomns = tgui::EditBox::create();
	tgui::Button::Ptr maxButton = tgui::Button::create();
	tgui::Button::Ptr okButton = tgui::Button::create();
	std::vector<int> result;
	bool okButtonClicked = false;
	UserDiffWindow();
	void loadWidget();
	void callback();
		std::vector<int> run();

public:
	static std::vector<int> UserDiffReturn();
};