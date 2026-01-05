
#include <SFML/Graphics.hpp>
#include<vector>
#include <iostream>
#include <random>
#include "ModalWindow.h"
#include <math.h>
#include "MapSize.h"
#include "difficulty.h"
#include <TGUI/TGUI.hpp>
#include <TGUI/AllWidgets.hpp>
#include "Statistics.h"

using namespace std;

Statistics s;

sf::Texture num[8];
sf::Texture WrongFlag;
sf::Texture texture;
sf::Texture OpenCell;
sf::Texture QuestionCell;
sf::Texture BombTexture;
sf::Texture ActivatedBomb;
sf::Font MyFont;
sf::Text text;
const int offsetX = 0;
const int offsetY = 75;

int sizeCell = 32;

sf::Text CountFlagText;

enum class GameStatus { Play, Win, Lose, none };
void initTexture() {

    for (int i = 0; i < sizeof(num) / sizeof(num[0]); i++) {
        //спрайты для ячеек рядом с бомбами
        if (!num[i].loadFromFile("Sprite/cell" + std::to_string(i) + ".png")) {
            std::cerr << "Failed to load texture for number " << (i) << std::endl;
        }
        num[i].setSmooth(false);
    }
    texture.loadFromFile("Sprite/Flags.png"); //спрайт флага
    texture.setSmooth(false);
    OpenCell.loadFromFile("Sprite/openCell.png");// просто открытой ячейки
    OpenCell.setSmooth(false);
    QuestionCell.loadFromFile("Sprite/Question.png");//спрайт вопроса
    QuestionCell.setSmooth(false);
    ActivatedBomb.loadFromFile("Sprite/activatedBomb.png");
    ActivatedBomb.setSmooth(false);
    WrongFlag.loadFromFile("Sprite/WrongFlagged.png"); // неправильного флага
    WrongFlag.setSmooth(false);

    BombTexture.loadFromFile("Sprite/bomb.png");
    BombTexture.setSmooth(false);
}


ModelWindow WinWindow;
ModelWindow LoseWindow;

class Cell {
public:
    short countBomb;
    bool open;
    bool bomb;
    enum status { none, flagged, question };
    status cellstatus;
    Cell(int c, bool o, bool b) : countBomb(c), open(o), bomb(b) {};
    Cell() {
        countBomb = 0;
        open = false;
        bomb = false;
        cellstatus = status::none;
    };
};
vector<Difficulty> vectorDifficulty{
    Difficulty(NumOfCell(9,9),10,u8"Начинающий"),
    Difficulty(NumOfCell(16,16),40,u8"Любитель"),
    Difficulty(NumOfCell(30,16),99,u8"Сапер"),

};


class GameMap {
public:
    short CountFlags; //сколько флагов доступно
    NumOfCell size;
    vector<vector<Cell>> cells;
    int correctFlags;
    int CountOpenCell;
    int countCell;
    int CountBomb;

    void mapPrintDebug(sf::RenderWindow& window, GameStatus status) {
        sf::RectangleShape rectangle(sf::Vector2f(sizeCell, sizeCell));
        rectangle.setOutlineThickness(1);
        rectangle.setOutlineColor(sf::Color::Black);

        for (int i = 0; i < size.columns; i++) {
            for (int j = 0; j < size.rows; j++) {
                float x = std::round(i * sizeCell + offsetX);
                float y = std::round(j * sizeCell + offsetY);
                rectangle.setPosition(x, y);
                rectangle.setTexture(nullptr);
                rectangle.setFillColor(sf::Color::White); 
                if (cells[i][j].bomb) {
                    if (cells[i][j].cellstatus == Cell::status::flagged) {
                        rectangle.setTexture(&texture);
                    }
                    else {
                        rectangle.setFillColor(sf::Color::Yellow);
                    }

                }
                else if (!cells[i][j].open) {
                    switch (cells[i][j].cellstatus) {
                    case Cell::status::none:
                        rectangle.setTexture(&OpenCell);// просто открытая ячейка
                        break;
                    case Cell::status::flagged:
                        rectangle.setTexture(&texture); //флаг
                        break;
                    case Cell::status::question:
                        rectangle.setTexture(&QuestionCell);//вопрос
                        break;
                    }
                }
                else {
                    if (cells[i][j].bomb) {
                        rectangle.setTexture(&BombTexture);
                    }
                    else if (cells[i][j].countBomb >= 0) {
                        rectangle.setTexture(&num[cells[i][j].countBomb]);
                    }
                }
                window.draw(rectangle);
            }
        }
    }
    void mapPrint(sf::RenderWindow& window, GameStatus status) {
        sf::RectangleShape rectangle(sf::Vector2f(sizeCell, sizeCell));
        rectangle.setOutlineThickness(1);
        rectangle.setOutlineColor(sf::Color::Black);

        for (int i = 0; i < size.columns; i++) {
            for (int j = 0; j < size.rows; j++) {
                float x = std::round(i * sizeCell + offsetX);
                float y = std::round(j * sizeCell + offsetY);
                rectangle.setPosition(x, y);
                rectangle.setTexture(nullptr);
                rectangle.setFillColor(sf::Color::White);
                // Если проиграли и бомбы нет,но есть флаг
                if (status == GameStatus::Lose && !cells[i][j].bomb) {
                    if (cells[i][j].cellstatus == Cell::flagged) {

                        rectangle.setTexture(&WrongFlag);
                        window.draw(rectangle);
                        continue;
                    }
                }
                //если проиграли и бомба есть
                if (status == GameStatus::Lose && cells[i][j].bomb) {
                    if (cells[i][j].open) {
                        rectangle.setTexture(&ActivatedBomb); // то на какой подорвались
                    }
                    else if (cells[i][j].cellstatus == Cell::flagged) {
                        rectangle.setTexture(&texture); //правильный флаг т.е. на бомбе
                    }
                    else if (!cells[i][j].open) {
                        rectangle.setTexture(&BombTexture);// бомба
                    }

                }
                else if (!cells[i][j].open) {
                    switch (cells[i][j].cellstatus) {
                    case Cell::status::none:
                        rectangle.setTexture(&OpenCell);// просто открытая ячейка
                        break;
                    case Cell::status::flagged:
                        rectangle.setTexture(&texture); //флаг
                        break;
                    case Cell::status::question:
                        rectangle.setTexture(&QuestionCell);//вопрос
                        break;
                    }
                }
                else {
                    if (cells[i][j].bomb) {
                        rectangle.setTexture(&BombTexture);
                    }
                    else if (cells[i][j].countBomb >= 0) {
                        rectangle.setTexture(&num[cells[i][j].countBomb]);
                    }
                }
                window.draw(rectangle);
            }
        }
    }
    GameMap() {
        resize(9, 9);
        //map.resize(16, vector<Cell>(16));
    }

    GameMap(int n, int y) : size(n, y) {
        resize(n, y);
    }
    void resize(int n, int y) {
        size.columns = n;
        size.rows = y;
        auto it = getDifficulty(); //получение размера,сложности

        CountFlags = it->countBomb; //выставление кол-во бомб/флагов
        cells.resize(n, vector<Cell>(y));
        countCell = it->size.columns * it->size.rows - it->countBomb;

    }
    void resize(const Difficulty& diff) {
        size.columns = diff.size.columns;
        size.rows = diff.size.rows; //получение размера,сложности

        CountFlags = diff.countBomb; //выставление кол-во бомб/флагов


        cells.resize(size.columns, vector<Cell>(size.rows));
        for (int i = 0; i < size.rows; i++) {
            cells[i].resize(size.rows);
        }
        CountBomb = diff.countBomb;
        countCell = size.columns * size.rows - CountBomb;


    }
    //инициилизация бомб
    void calcBomb(int x, int y) {
        vector<pair<int, int>> neighbour; //соседи рядом с бомбами

        std::random_device rd;
        std::mt19937 rnd(rd());

        auto it = getDifficulty();

        std::uniform_int_distribution<int> distX(0, size.columns - 1);
        std::uniform_int_distribution<int> distY(0, size.rows - 1);//ограничение по рандому
        CountBomb = it->countBomb;
        for (int i = 0; i < CountBomb; i++) {
            int x1 = distX(rnd);
            int y1 = distY(rnd);// генерация бомб

            if (((abs(x -x1) >1) || (abs(y-y1) >1)) && !cells[x1][y1].bomb) {
                cells[x1][y1].bomb = 1;


                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {//запихиваем соседей для высчитывания  кол-во бомб
                        if (di == 0 && dj == 0) continue;
                        int dx = x1 + di;
                        int dy = y1 + dj;
                        if (dx >= 0 && dx < size.columns && dy >= 0 && dy < size.rows) {
                            if (!cells[dx][dy].bomb) {
                                neighbour.push_back(make_pair(dx, dy));
                            }
                        }
                    }
                }
            }
            else {
                i--;
            }
        }

        initCell(neighbour);
    }
    //подсчет кол-во бомб возле клетки
    void initCell(const vector<pair<int, int>>& neighbor) {
        for (int i = 0; i < neighbor.size(); i++) {
            int x = neighbor[i].first;
            int y = neighbor[i].second;
            short count = 0;
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    if (di == 0 && dj == 0) continue;
                    int dx = x + di;
                    int dy = y + dj;
                    if (dx >= 0 && dx < size.columns && dy >= 0 && dy < size.rows) {
                        if (cells[dx][dy].bomb) {
                            count++;
                        }
                    }
                }
            }
            cells[x][y].countBomb = count;
        }
    }
    void rightСlick(int x, int y, int& moves, GameStatus& status) {
        if (x < 0 || x >= size.columns || y < 0 || y >= size.rows) {
            return;
        }

        if (cells[x][y].open) {
            return;
        }

        switch (cells[x][y].cellstatus) {
        case Cell::none:
            CountFlags--;
            if (cells[x][y].bomb) correctFlags++;
            cells[x][y].cellstatus = Cell::flagged;
            cerr << "нажатый флаг:";
            cerr << endl << x << " " << y;
            CountFlagText.setString(to_string(CountFlags));
            break;
        case Cell::flagged:
            CountFlags++;
            if (cells[x][y].bomb) correctFlags--;
            cells[x][y].cellstatus = Cell::question;
            CountFlagText.setString(to_string(CountFlags));
            break;
        case Cell::question:
            cells[x][y].cellstatus = Cell::none;
            break;
        }
    }
    //рекурсивное открытие клетки
    void openCell(int x, int y) {
        vector <pair<short, short>> coordinat;
        coordinat.push_back(make_pair(x, y));
        while (!coordinat.empty()) {
            short newX = coordinat.back().first;
            short newY = coordinat.back().second;
            coordinat.pop_back();

            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    if (di == 0 && dj == 0) continue;
                    int dx = newX + di;
                    int dy = newY + dj;
                    if (dx >= 0 && dx < size.columns && dy >= 0 && dy < size.rows) {
                        if (!cells[dx][dy].open && !cells[dx][dy].bomb && cells[dx][dy].cellstatus != Cell::flagged) {
                            CountOpenCell++;
                            cells[dx][dy].open = true;
                            if (cells[dx][dy].countBomb == 0) {
                                coordinat.push_back(make_pair(dx, dy)); //если возле клетки нет бомб
                                //то открываются которые рядом
                            }
                        }
                    }
                }
            }
        }
    }
    bool clickOnCell(int x, int y) {
        vector <pair<short, short>> coordinat;

        bool activatedBomb = false;
        int countBomb = 0;
        for (int di = -1; di <= 1; di++) {
            for (int dj = -1; dj <= 1; dj++) {
                if (di == 0 && dj == 0) continue;
                int dx = x + di;
                int dy = y + dj;
                if (dx >= 0 && dx < size.columns && dy >= 0 && dy < size.rows) {
                    if (cells[dx][dy].cellstatus == Cell::flagged) {
                        countBomb++;
                        if (CountBomb == cells[x][y].countBomb) break;
                    }
                }
            }
        }
        if (countBomb == cells[x][y].countBomb) {
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int dx = x + di;
                    int dy = y + dj;
                    if (di == 0 && dj == 0) continue;
                    if (dx >= 0 && dx < size.columns && dy >= 0 && dy < size.rows) {
                        if (cells[dx][dy].cellstatus == Cell::flagged) continue;
                        coordinat.push_back(make_pair(x, y));
                        if (cells[dx][dy].bomb) {
                            activatedBomb = true;
                        }
                    }
                }
            }
        }
        for (auto& i : coordinat) {
            openCell(i.first,i.second);
        }
        return activatedBomb;
    }
    //чек позиции 
    void checkPos(int x, int y, GameStatus& status, int& moves) {
        if (x < 0 || x >= size.columns || y < 0 || y >= size.rows) {
            return;
        }
        if (moves == 0) {
            status = GameStatus::Play;
            calcBomb(x, y);
        }
        if (cells[x][y].cellstatus == Cell::flagged) {
            return;
        }
        if (cells[x][y].open && cells[x][y].countBomb>0) {
            if (clickOnCell(x, y)) {
                status = GameStatus::Lose;
            }
            return;
        }
        CountOpenCell++;
        cells[x][y].open = true;
        moves++;
        if (cells[x][y].bomb) {
            status = GameStatus::Lose;
            return;
        }
        else if (cells[x][y].countBomb == 0) {
            openCell(x, y);
        }


    }
    const Difficulty* getDifficulty() const {
        auto it = find_if(vectorDifficulty.begin(), vectorDifficulty.end(),
            [this](const Difficulty& p) {
                return p.size.columns == size.columns && p.size.rows == size.rows;
            });
        return it != vectorDifficulty.end() ? &(*it) : nullptr;
    }
    
};
class Game {
public:
    Difficulty::diff diffStatus;
    Difficulty dif;
    GameStatus status = GameStatus::none;
    GameMap map;
    bool debug = 0;
    int moves = 0;
    sf::Time timer;
    Game() : map(9, 9) {
        diffStatus == Difficulty::Beginner;
        status = GameStatus::Play;
        dif= vectorDifficulty[0];
    }
    int getTime() {
        return static_cast<int>(timer.asSeconds());
    }
    void rightclick(int x, int y) {
        map.rightСlick(x, y, moves, status);
        wincheck();
    }
    void checkpos(int x, int y) {
        map.checkPos(x, y, status, moves);
        if (status == GameStatus::Lose) {
            LoseWindow.show();
            s.addStatistics(statisticsNode(timer.asSeconds(), dif, GameResult::LOSS));
            
        }
        else {
            if (moves > 0) {
                wincheck();
            }

        }

    }
    void resizeWindow(sf::RenderWindow& window) {
        window.setSize(sf::Vector2u(map.size.columns * sizeCell + offsetX, map.size.rows * sizeCell + offsetY));
        window.setView(sf::View(sf::FloatRect(
            0, 0,
            map.size.columns * sizeCell + offsetX,
            map.size.rows * sizeCell + offsetY
        )));

    }
    void setstats() {
        map.CountFlags = map.CountBomb;
        CountFlagText.setString(to_string(map.CountFlags));
        timer = sf::seconds(0);
        moves = 0;
        map.correctFlags = 0;
        map.CountOpenCell = 0;
        status = GameStatus::none;
    }
    void newgame() {
        for (auto& i : map.cells) {
            for (auto& e : i) {
                e = Cell();
            }
        }
        map.CountFlags = map.CountBomb;
        CountFlagText.setString(to_string(map.CountFlags));
        timer = sf::seconds(0);
        moves = 0;
        map.correctFlags = 0;
        map.CountOpenCell = 0;
        status = GameStatus::none;
        text.setString("0");


    }
    void mapPrint(sf::RenderWindow& window) {
        if (debug) {
            map.mapPrintDebug(window, status);
        }
        else {
            map.mapPrint(window, status);
        }
    }
    void wincheck() {
        if ((map.correctFlags == map.CountBomb && map.CountFlags ==0) || map.countCell == map.CountOpenCell) {
            status = GameStatus::Win;
            WinWindow.show();
            if (!debug) {
                s.addStatistics(statisticsNode(timer.asSeconds(), dif,GameResult::WIN));
            }

        }
    }
    void updateTime(sf::Time time) {
        if (status == GameStatus::Play) {
            timer += time;
        }
    }
    void changeDifficulty(const Difficulty& dif) {

        map.resize(dif);

        newgame();
        setModalWindowToDefault();

    }
    void resizeModalWindow(sf::RenderWindow& window) {
        WinWindow.resize(window);
        LoseWindow.resize(window);
    }
    void setModalWindowToDefault() {
        WinWindow.hide();
        LoseWindow.hide();
    }
};
    int main()
    {
        sf::RenderWindow window(sf::VideoMode(288, 288), L"Сапер работает ахуеть");
        Game game;
        initTexture();
        game.resizeWindow(window);

        sf::Clock clock;

        if (!MyFont.loadFromFile("Fonts/arial.ttf")) {
        }

        CountFlagText.setFont(MyFont);
   
        CountFlagText.setCharacterSize(40);
        CountFlagText.setFillColor(sf::Color::Red);
        CountFlagText.setPosition(0, offsetY-50);

        CountFlagText.setString(to_string(game.map.CountFlags));
        text.setFont(MyFont);
        text.setCharacterSize(36);
        text.setFillColor(sf::Color::Red);
        text.setPosition(window.getSize().x-30, offsetY-50);
        WinWindow.init(window.getSize().x, window.getSize().y
            , sf::Color(0, 0, 0, 200), L"Вы победили!", MyFont, sf::Color::Green);
        LoseWindow.init(window.getSize().x, window.getSize().y,

            sf::Color(0, 0, 0, 200), L"Вы проиграли!", MyFont, sf::Color::Red);

        tgui::Gui gui(window);
        tgui::Font font("Fonts/arial.ttf");
        gui.setFont(font);
        auto menuBar = tgui::MenuBar::create();


        menuBar->setSize("100%", 30);

        menuBar->addMenu(L"игрa");
        menuBar->addMenuItem(L"Статистика");
        menuBar->addMenu(L"Уровень игры");
        menuBar->addMenuItem(L"Уровень игры", L"Новичок");
        menuBar->addMenuItem(L"Уровень игры", L"Опытный");
        menuBar->addMenuItem(L"Уровень игры", L"Сапер");

        sf::Time time;

        window.setVerticalSyncEnabled(true);
        bool ShowWindow = true;
        menuBar->onMenuItemClick([&game,&window](const tgui::String& item) {
            if (item == L"Статистика") {
                window.setActive(false);

                sf::RenderWindow windowStatistics(sf::VideoMode(500, 288), L"Статистика", sf::Style::Titlebar | sf::Style::Close);
                tgui::Gui gui(windowStatistics);
                sf::RectangleShape rec{ sf::Vector2f(500,60) };
                rec.setFillColor(sf::Color(255, 219, 139));
                auto label = tgui::Label::create();
                label->setText(s.getTotalStats());
                label->setPosition(0, 15);
                auto listBox = tgui::ListBox::create();
                listBox->setItemHeight(50);
                for (auto& stats : s.statisticsBuilder()) {
                    listBox->addItem(stats);
                }
                windowStatistics.setFramerateLimit(30);
                listBox->setPosition(0, 60);
                listBox->setSize("100%","80%");
                gui.add(listBox);
                gui.add(label);
                windowStatistics.setVerticalSyncEnabled(true);

                while (windowStatistics.isOpen()) {
                    sf::Event event;
                    while (windowStatistics.pollEvent(event))
                    {
                        gui.handleEvent(event);
                        if (event.type == sf::Event::Closed)
                            windowStatistics.close();

                    }
                    windowStatistics.clear();
                    windowStatistics.draw(rec);
                    gui.draw();
                    windowStatistics.display();
                }
                window.setActive(true);

            }
            else if(item == L"Новичок"&& game.diffStatus != Difficulty::Beginner) {
                game.diffStatus = Difficulty::Beginner;
                game.dif = vectorDifficulty[0];
                game.changeDifficulty(vectorDifficulty[0]);
                game.resizeWindow(window);
                game.resizeModalWindow(window);
            }
            else if (item == L"Опытный" && game.diffStatus != Difficulty::Skilled) {
                game.diffStatus = Difficulty::Skilled;
                game.changeDifficulty(vectorDifficulty[1]);
                game.dif = vectorDifficulty[1];
                game.resizeWindow(window);
                game.resizeModalWindow(window);
            }
            else if (item == L"Сапер" && game.diffStatus != Difficulty::minesweeper) {
                game.diffStatus = Difficulty::minesweeper;
                game.changeDifficulty(vectorDifficulty[2]);
                game.dif = vectorDifficulty[2];
                game.resizeWindow(window);
                game.resizeModalWindow(window);
            }
            });

        gui.add(menuBar);


    while (window.isOpen())
    {

            sf::Time time = clock.getElapsedTime();
            clock.restart();
            if (game.status == GameStatus::Play) {
            game.updateTime(time);
            text.setString(to_string(game.getTime()));
        }
        sf::Event event;
        while (window.pollEvent(event))
        {
            gui.handleEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized) {
                float windowRatio = (float)event.size.width / event.size.height;
                float viewWidth = game.map.size.columns * sizeCell;
                float viewHeight = game.map.size.rows * sizeCell + offsetY;
                float viewRatio = viewWidth / viewHeight;

                sf::FloatRect viewport(0, 0, 1, 1);

                if (windowRatio > viewRatio) {
                    viewport.width = viewRatio / windowRatio;
                    viewport.left = (1 - viewport.width) / 2;
                }
                else {
                    viewport.height = windowRatio / viewRatio;
                    viewport.top = (1 - viewport.height) / 2;
                }

                sf::View view(sf::FloatRect(0, 0, viewWidth, viewHeight));
                view.setViewport(viewport);
                window.setView(view);
                CountFlagText.setPosition(0, offsetY - 50);

                text.setPosition(window.getSize().x - 30, offsetY - 50);
                ShowWindow = true;
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::F9)) {
                    ShowWindow = true;
                    game.debug = !game.debug;

                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                    ShowWindow = true;
                    game.newgame();
                    LoseWindow.hide();
                    WinWindow.hide();
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                ShowWindow = true;
                auto focusedWidget = gui.getFocusedChild();;
                if (focusedWidget && focusedWidget->getWidgetType() == "MenuBarMenuPlaceholder") {
                    continue;
                }
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
                if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && (game.status == GameStatus::Play || game.status == GameStatus::none)) {
                    game.rightclick((int)(worldPos.x - offsetX) / sizeCell, (int)(worldPos.y - offsetY) / sizeCell);
                }

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (game.status == GameStatus::Play || game.status == GameStatus::none)) {
                    game.checkpos((int)(worldPos.x - offsetX) / sizeCell, (int)(worldPos.y - offsetY) / sizeCell);
                }
            }
        }

        if (ShowWindow) {
            window.clear(sf::Color::White);
            game.mapPrint(window);
            window.draw(text);
            window.draw(CountFlagText);
            WinWindow.draw(window);
            LoseWindow.draw(window);
            gui.draw();
            window.display();
            ShowWindow = false;
        }
        sf::sleep(sf::milliseconds(16));
    }
    return 0;
}