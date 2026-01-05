#pragma once
#ifndef ModalWindow_h
#define ModalWindow_h
#include <SFML/Graphics.hpp>
#include <string>
class ModelWindow {
public:
    sf::RectangleShape background;
    sf::Text text;
    sf::Text newGameText;
    sf::Font font;
    bool isVisible = 0;
    void show();
    void hide();
    ModelWindow();

    ModelWindow(float posX, float posY, sf::Color color, std::string text, sf::Font textfont);
    ModelWindow(float posX, float posY, sf::Color color, const wchar_t text[], sf::Font textfont, sf::Color textColor);


    void init(float posX, float posY, sf::Color color, const wchar_t text[], sf::Font textfont, sf::Color textColor);

    void resize(sf::RenderWindow& window);
    void centerText();
    void draw(sf::RenderWindow& window);

};
#endif