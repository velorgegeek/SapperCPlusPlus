
#include <SFML/Graphics.hpp>
#include<vector>
#include <iostream>
#include <random>
#include <math.h>
using namespace std;
sf::Texture num[8];
sf::Texture WrongFlag;
sf::Texture texture;
sf::Texture OpenCell;
sf::Texture QuestionCell;
sf::Texture BombTexture;
sf::Texture ActivatedBomb;
const int offsetX = 0;
const int offsetY = 50;

int sizeCell = 32;

sf::Text CountFlagText;

enum class GameStatus { Play, Win, Lose, none };
void initTexture() {

    for (int i = 0; i < sizeof(num) / sizeof(num[0]); i++) {
        //спрайты для ячеек рядом с бомбами
        if (!num[i].loadFromFile("Спрайты/cell" + std::to_string(i) + ".png")) {
            std::cerr << "Failed to load texture for number " << (i) << std::endl;
        }
        num[i].setSmooth(false);
    }
    texture.loadFromFile("Спрайты/Flags.png"); //спрайт флага
    texture.setSmooth(false);
    OpenCell.loadFromFile("Спрайты/openCell.png");// просто открытой ячейки
    OpenCell.setSmooth(false);
    QuestionCell.loadFromFile("Спрайты/Question.png");//спрайт вопроса
    QuestionCell.setSmooth(false);
    ActivatedBomb.loadFromFile("Спрайты/activatedBomb.png");
    ActivatedBomb.setSmooth(false);
    WrongFlag.loadFromFile("Спрайты/WrongFlagged.png"); // неправильного флага
    WrongFlag.setSmooth(false);

    BombTexture.loadFromFile("Спрайты/bomb.png");
    BombTexture.setSmooth(false);
}
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
class NumOfCell {
public:
    short columns;
    short rows;
    NumOfCell() {
        columns = 9;
        rows = 9;
    }
    NumOfCell(short c, short r) : columns(c), rows(r) {};
    bool operator==(const NumOfCell& other) const {
        return columns == other.columns && rows == other.rows;
    }
};
vector<pair<NumOfCell, short>> difficulty{
    //сложность, первое число колонки, второе строки
    //третье число, это сколько бомб
    make_pair(NumOfCell(9,9),10),
    make_pair(NumOfCell(16,16),40),
    make_pair(NumOfCell(30,16),99),
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
        auto it = find_if(difficulty.begin(), difficulty.end(),
            [this](const pair<NumOfCell, short>& p) {
                return p.first.columns == size.columns && p.first.rows == size.rows;
            }); //получение размера,сложности

        CountFlags = it->second; //выставление кол-во бомб/флагов
        cells.resize(n, vector<Cell>(y));
        countCell = it->first.columns * it->first.rows - it->second;

    }
    //инициилизация бомб
    void calcBomb(int x, int y) {
        vector<pair<int, int>> neighbour; //соседи рядом с бомбами

        std::random_device rd;
        std::mt19937 rnd(rd());

        auto it = find_if(difficulty.begin(), difficulty.end(),
            [this](const pair<NumOfCell, short>& p) {
                return p.first.columns == size.columns && p.first.rows == size.rows;
            });//получение кол-во бомб

        std::uniform_int_distribution<int> distX(0, size.columns - 1);
        std::uniform_int_distribution<int> distY(0, size.rows - 1);//ограничение по рандому
        CountBomb = it->second;
        for (int i = 0; i < it->second; i++) {
            int x1 = distX(rnd);
            int y1 = distY(rnd);// генерация бомб

            if (x1 != x && y1 != y && !cells[x1][y1].bomb) {
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
    void rightclick(int x, int y, int& moves, GameStatus& status) {
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
    //чек позиции 
    void checkPos(int x, int y, GameStatus& status, int& moves) {
        if (x < 0 || x >= size.columns || y < 0 || y >= size.rows) {
            return;
        }
        if (moves == 0) {
            status = GameStatus::Play;
            calcBomb(x, y);
        }
        if (cells[x][y].open || cells[x][y].cellstatus == Cell::flagged) {
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
};
class Game {
public:

    GameStatus status = GameStatus::none;
    GameMap map;
    int moves = 0;
    sf::Time timer;
    Game() : map(9, 9) {
        status = GameStatus::Play;
    }
    int getTime() {
        return static_cast<int>(timer.asSeconds());
    }
    void rightclick(int x, int y) {
        map.rightclick(x, y, moves, status);
        wincheck();
    }
    void checkpos(int x, int y) {
        map.checkPos(x, y, status, moves);
        if (status == GameStatus::Lose) {
        }
        else {
            wincheck();
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
        map.countCell = 0;
        map.correctFlags = 0;
        map.CountOpenCell = 0;
        status = GameStatus::none;



    }
    void mapPrint(sf::RenderWindow& window) {
        map.mapPrint(window, status);
    }
    void wincheck() {
        if (map.correctFlags == map.CountBomb && map.countCell == map.CountOpenCell) {
            status = GameStatus::Win;
        }
    }
    void updateTime(sf::Time time) {
        if (status == GameStatus::Play) {
            timer += time;
        }
    }
};
int main()
{
    sf::RenderWindow window(sf::VideoMode(288, 288), L"Сапер работает ахуеть");
    Game game;
    initTexture();
    game.resizeWindow(window);

    sf::Clock clock;
    sf::Font MyFont;
    if (!MyFont.loadFromFile("Fonts/arial.ttf")) {
    }
    sf::Text text;
    CountFlagText.setFont(MyFont);
    CountFlagText.setCharacterSize(36);
    CountFlagText.setFillColor(sf::Color::Red);
    CountFlagText.setPosition(0, 0);

    CountFlagText.setString(to_string(game.map.CountFlags));
    text.setFont(MyFont);
    text.setCharacterSize(36);
    text.setFillColor(sf::Color::Red);
    text.setPosition(255, 0);

    while (window.isOpen())
    {
        sf::Time time = clock.getElapsedTime();
        clock.restart();
        game.updateTime(time);
        text.setString(to_string(game.getTime()));
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                    game.newgame();
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && (game.status == GameStatus::Play || game.status == GameStatus::none)) {
                    sf::Vector2i localPosition = sf::Mouse::getPosition(window);
                    game.rightclick((localPosition.x - offsetX) / sizeCell, (localPosition.y - offsetY) / sizeCell);
                }

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (game.status == GameStatus::Play || game.status == GameStatus::none)) {
                    sf::Vector2i localPosition = sf::Mouse::getPosition(window);

                    game.checkpos((localPosition.x - offsetX) / sizeCell, (localPosition.y - offsetY) / sizeCell);
                }
            }
        }


        window.clear(sf::Color::White);
        game.mapPrint(window);
        window.draw(text);
        window.draw(CountFlagText);
        window.display();
        if (game.status == GameStatus::Win) {
            sf::RenderWindow windows(sf::VideoMode(144, 144), L"Вы победил");
            while (window.isOpen())
            {
                sf::Event event;
                while (windows.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                }
                windows.display();
            }
        }
    }

    return 0;
}