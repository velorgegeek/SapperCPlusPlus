#include "ModalWindow.h"
#include <SFML/Graphics.hpp>
#include <string>

void ModelWindow::show() {
    isVisible = 1;
}
void ModelWindow::hide() {
    isVisible = 0;
}
ModelWindow::ModelWindow() {};

ModelWindow::ModelWindow(float posX, float posY, sf::Color color, std::string text, sf::Font textfont) {
    this->text.setString(text);
    font = textfont;
    newGameText.setCharacterSize(26);
    newGameText.setFont(textfont);
    newGameText.setString(L"Нажмите R для перезапуска");
    this->text.setFont(font);
    this->text.setCharacterSize(36);
    background.setSize(sf::Vector2f(posX, posY));
    background.setFillColor(color);
    background.setPosition(0, 0);
    centerText();
}
ModelWindow::ModelWindow(float posX, float posY, sf::Color color, const wchar_t text[], sf::Font textfont, sf::Color textColor) {
    this->text.setString(text);
    font = textfont;
    this->text.setFont(font);
    this->text.setCharacterSize(36);
    newGameText.setCharacterSize(18);
    newGameText.setFont(font);
    newGameText.setString(L"Нажмите R для перезапуска");

    background.setSize(sf::Vector2f(posX, posY));
    background.setFillColor(color);
    background.setPosition(0, 0);
    this->text.setFillColor(textColor);
    this->text.setOutlineColor(sf::Color::Black);
    this->text.setOutlineThickness(1);
    centerText();
    newGameText.setPosition(this->text.getPosition().x, this->text.getPosition().y + this->text.getCharacterSize() + 10);


}

void ModelWindow::init(float posX, float posY, sf::Color color, const wchar_t text[], sf::Font textfont, sf::Color textColor) {
    this->text.setString(text);
    font = textfont;
    this->text.setFont(font);
    this->text.setCharacterSize(36);
    newGameText.setCharacterSize(18);
    newGameText.setFont(font);
    newGameText.setString(L"Нажмите R для перезапуска");

    background.setSize(sf::Vector2f(posX, posY));
    background.setFillColor(color);
    background.setPosition(0, 0);
    this->text.setFillColor(textColor);
    this->text.setOutlineColor(sf::Color::Black);
    this->text.setOutlineThickness(1);
    centerText();
    newGameText.setPosition(this->text.getPosition().x, this->text.getPosition().y + this->text.getCharacterSize() + 10);
    newGameText.setOutlineColor(sf::Color::Black);
    newGameText.setOutlineThickness(1);
}

void ModelWindow::resize(sf::RenderWindow& window) {

    sf::Vector2u windowSize = window.getSize();

    // Устанавливаем размер фона
    background.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    text.setCharacterSize(windowSize.y / 10); // Динамический размер текста
    newGameText.setCharacterSize(windowSize.y / 20);
    centerText();
    newGameText.setPosition(this->text.getPosition().x, this->text.getPosition().y + this->text.getCharacterSize() + newGameText.getCharacterSize());
}
void ModelWindow::centerText() {
    sf::FloatRect textBounds = text.getLocalBounds();
    sf::FloatRect bgBounds = background.getGlobalBounds();

    text.setPosition(
        bgBounds.left + bgBounds.width / 2 - textBounds.width / 2,
        bgBounds.top + bgBounds.height / 2 - textBounds.height / 2 - 10
    );
    newGameText.setPosition(bgBounds.left + bgBounds.width / 2 - textBounds.width / 2,
        bgBounds.top + bgBounds.height / 2 - textBounds.height / 2 + 40);
}
void ModelWindow::draw(sf::RenderWindow& window) {
    if (!isVisible) return;
    centerText();
    window.draw(background);
    window.draw(text);
    window.draw(newGameText);
}