RenderWindow *window;

struct Position {
    float x;
    float y;
};

enum Direction {
    LEFT, RIGHT, UP, DOWN
};

const int GAME_WDTH = 690;
const int GAME_HGHT = 300;

const int OFFSET_WDTH = (WDTH - GAME_WDTH) / 2;
const int OFFSET_HGHT = (HGHT - GAME_HGHT) / 2 - HGHT / 15 / 20 * 20 + 20;

int points = 0;
int maxPoints;

void delay(int time) {
    Clock clock;
    Time waitFor = milliseconds(time);
    Time elapsed = clock.getElapsedTime();
    clock.restart();
    while (elapsed <= waitFor)
        elapsed = clock.getElapsedTime();
}

void printTextSnake(Position pos, string text, float size, float outlineSize) {
    Font font;
    font.loadFromFile(PREFIX + "Resources/font.ttf");
    Text textSFML;
    textSFML.setFont(font);
    textSFML.setCharacterSize(size);
    textSFML.setFillColor(Color::White);
    textSFML.setOutlineColor(Color::Black);
    textSFML.setOutlineThickness(outlineSize);
    textSFML.setString(text);
    textSFML.setPosition(pos.x, pos.y);
    window->draw(textSFML);
}

void retrieveMaxScore() {
    ifstream fin(PREFIX + "Resources/maxscore.txt");
    fin >> maxPoints;
    fin.close();
}

void writeScore() {
    if (points > maxPoints) {
        ofstream fout(PREFIX + "Resources/maxscore.txt");
        fout << points;
        fout.close();
    }
}

void score() {
    char text[100];
    sprintf(text, "%d", points);
    printTextSnake({30 + OFFSET_WDTH, OFFSET_HGHT - 1}, "Score:", 24, 3);
    printTextSnake({105 + OFFSET_WDTH, OFFSET_HGHT}, text, 24, 3);
}

void maxScore() {
    char text[100];
    sprintf(text, "%d", maxPoints);
    int x = 0;
    if (maxPoints > 9)
        x = 11;
    else if (maxPoints > 99)
        x = 22;
    printTextSnake({OFFSET_WDTH + GAME_WDTH - 185, OFFSET_HGHT - 1}, "High Score:", 24, 3);
    printTextSnake({static_cast<float>(OFFSET_WDTH + GAME_WDTH - 44 - x), OFFSET_HGHT}, text, 24, 3);
}

struct GameSquare {
    Position pos;
    void display() {
        RectangleShape square;
        square.setPosition(pos.x, pos.y);
        square.setSize(Vector2f(30, 30));
        square.setFillColor(Color::Transparent);
        square.setOutlineColor(Color(0x999999ff));
        square.setOutlineThickness(-1);
        window->draw(square);
    };
} gameMatrix[500][500];

int rowCount;
int colCount;

struct AppWindowSnake {
    int width;
    int height;
    bool displayGrid;

    void init() {
        width = GAME_WDTH;
        height = GAME_HGHT;
        displayGrid = false;
        colCount = GAME_WDTH / 30 - 2;
        rowCount = GAME_HGHT / 30 - 2;

        Position currentPos = {OFFSET_WDTH, OFFSET_HGHT};
        for (int i = 0; i <= rowCount + 1; i++) {
            for (int j = 0; j <= colCount + 1; j++) {
                gameMatrix[i][j].pos = currentPos;
                currentPos.x += 30;
            }
            currentPos.y += 30;
            currentPos.x = OFFSET_WDTH;
        }
    }

    void display() {
        RectangleShape screen;
        screen.setSize(Vector2f(width, height));
        screen.setPosition(OFFSET_WDTH, OFFSET_HGHT);
        screen.setFillColor(Color::Black);
        screen.setOutlineColor(Color::White);
        screen.setOutlineThickness(-3);
        window->draw(screen);

        RectangleShape gridOutline;
        gridOutline.setSize(Vector2f(width - 60, height - 60));
        gridOutline.setPosition(OFFSET_WDTH + 30, OFFSET_HGHT + 30);
        gridOutline.setFillColor(Color(0x111111ff));
        gridOutline.setOutlineColor(Color::Red);
        gridOutline.setOutlineThickness(-3);
        window->draw(gridOutline);

        if (displayGrid)
            for (int i = 1; i <= rowCount; i++)
                for (int j = 1; j <= colCount; j++)
                    gameMatrix[i][j].display();
    }
} appWindowSnake;

void snakeEye(float side, Position pos) {
    RectangleShape square;
    square.setSize(Vector2f(side / 6, side / 6));
    square.setPosition(Vector2f(pos.x + 2.5f, pos.y + 2.5f));
    square.setFillColor(Color::Black);
    window->draw(square);
}

struct Square {
    Square *next;
    int row, col;
    Position pos;

    void display(float side) {
        RectangleShape squareShape;
        squareShape.setSize(Vector2f(side, side));
        squareShape.setPosition(Vector2f(pos.x + 2.5f, pos.y + 2.5f));
        window->draw(squareShape);
    }
};

struct Snake {
    float squareSide;
    Square *first;
    Square *last;
    Direction state;

    void display() {
        for (Square *currentSquare = first; currentSquare; currentSquare = currentSquare->next)
            currentSquare->display(squareSide);
        switch (state) {
          case RIGHT:
            snakeEye(squareSide, {last->pos.x + 2 * squareSide / 3, last->pos.y + 6 * squareSide / 10});
            snakeEye(squareSide, {last->pos.x + 2 * squareSide / 3, last->pos.y + 3 * squareSide / 10});
            break;
          case LEFT:
            snakeEye(squareSide, {last->pos.x + squareSide / 4, last->pos.y + 6 * squareSide / 10});
            snakeEye(squareSide, {last->pos.x + squareSide / 4, last->pos.y + 3 * squareSide / 10});
            break;
          case UP:
            snakeEye(squareSide, {last->pos.x + 6 * squareSide / 10, last->pos.y + squareSide / 4});
            snakeEye(squareSide, {last->pos.x + 3 * squareSide / 10, last->pos.y + squareSide / 4});
            break;
          case DOWN:
            snakeEye(squareSide, {last->pos.x + 6 * squareSide / 10, last->pos.y + 2 * squareSide / 3});
            snakeEye(squareSide, {last->pos.x + 3 * squareSide / 10, last->pos.y + 2 * squareSide / 3});
            break;
        }
    }
} snake;

void initSnake() {
    snake.squareSide = 25;
    snake.first = nullptr;
    snake.first = new Square;
    snake.first->row = rowCount / 2;
    snake.first->col = colCount / 2;
    snake.first->pos = gameMatrix[snake.first->row][snake.first->col].pos;
    snake.first->next = nullptr;
    snake.last = snake.first;
}

bool checkOverlap(Position pos) {
    for (Square *currentSquare = snake.first; currentSquare; currentSquare = currentSquare->next)
        if ((pos.x == gameMatrix[currentSquare->row][currentSquare->col].pos.x) &&
            (pos.y == gameMatrix[currentSquare->row][currentSquare->col].pos.y))
                return true;
    return false;
}

bool lose = false;

void newSquare(Snake& snake) {
    Square *newSquare = new Square;
    switch (snake.state) {
      case RIGHT:
        newSquare->row = snake.last->row;
        newSquare->col = snake.last->col + 1;
        break;
      case LEFT:
        newSquare->row = snake.last->row;
        newSquare->col = snake.last->col - 1;
        break;
      case UP:
        newSquare->row = snake.last->row - 1;
        newSquare->col = snake.last->col;
        break;
      case DOWN:
        newSquare->row = snake.last->row + 1;
        newSquare->col = snake.last->col;
        break;
    }
    newSquare->pos = gameMatrix[newSquare->row][newSquare->col].pos;
    newSquare->next = nullptr;
    if (newSquare->col == 0 || newSquare->col >= colCount + 1 ||
        newSquare->row == 0 || newSquare->row == rowCount + 1 ||
        checkOverlap(newSquare->pos)) {
            writeScore();
            lose = true;
        }
    snake.last->next = newSquare;
    snake.last = newSquare;
}

bool grow = false;
bool moveDone;

void snakeMove(Snake& snake) {
    delay(250);
    newSquare(snake);
    if (!grow)
        snake.first = snake.first->next;
    grow = false;
    moveDone = true;
}

struct Fruit {
    Position pos;
    int row, col;

    void repos() {
        row = rand() % rowCount + 1;
        col = rand() % colCount + 1;
        pos.x = gameMatrix[row][col].pos.x;
        pos.y = gameMatrix[row][col].pos.y;
    }

    void display() {
        CircleShape circle;
        circle.setRadius(11.5f);
        circle.setFillColor(Color::Red);
        circle.setPosition(pos.x + 3.5f, pos.y + 3.5f);
        window->draw(circle);
    }

    bool isWhitin(Position posOfSnake) {
        return posOfSnake.x == pos.x
            && posOfSnake.y == pos.y;
    }
} fruit;

void snakeNew() {
    initSnake();
    snake.state = RIGHT; newSquare(snake);
    snake.state = RIGHT; newSquare(snake);
    snake.state = RIGHT; newSquare(snake);
    snake.state = RIGHT; newSquare(snake);
    snake.state = RIGHT; newSquare(snake);
    snake.state = RIGHT; newSquare(snake);
}

void initSnakeAtStartup() {
    retrieveMaxScore();
    appWindowSnake.init();
    snakeNew();
    do
        fruit.repos();
    while (checkOverlap(fruit.pos));
}

void updateSnake() {
    snakeMove(snake);
    if (fruit.isWhitin(snake.last->pos)) {
        do
            fruit.repos();
        while (checkOverlap(fruit.pos));
        grow = true;
        points++;
    }
}

void drawSnake() {
    appWindowSnake.display();
    fruit.display();
    snake.display();
    score();
    maxScore();
}

bool gameOpen = false;

void openSnake() {
    points = 0;
    initSnakeAtStartup();
    gameOpen = true;
    lose = false;
    appWindowSnake.display();
}

void closeSnake() {
    gameOpen = false;
}
