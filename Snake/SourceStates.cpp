#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

using namespace sf;

const int CELL_SIZE = 50;
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 15;

const Color GRID_COLOR = Color(50, 50, 50);


const int WINDOW_WIDTH = CELL_SIZE * GRID_WIDTH;
const int WINDOW_HEIGHT = CELL_SIZE * GRID_HEIGHT;

Vector2i spawnFood(std::vector<Vector2i>& snake)
{
    Vector2i position;

    std::vector<Vector2i> availablePositions;

    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            bool isInSnake = false;
            for (int i = 0; i < snake.size(); i++)
            {
                if (snake[i].x == x && snake[i].y == y)
                {
                    isInSnake = true;
                    break;
                }
            }
            if (isInSnake == false)
            {
                availablePositions.emplace_back(x, y);
            }
        }
    }

    return availablePositions[rand() % availablePositions.size()];
}

enum GameState
{
	Menu,
    Game,
    GameOver
};


int main()
{
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SNAKE");
    window.setFramerateLimit(5);

    // создаем змейку
    std::vector<Vector2i> snake;

    RectangleShape cellShape(Vector2f(CELL_SIZE, CELL_SIZE));

    Vector2i direction = Vector2i(-1, 0);

    Vector2i food;

    GameState gameState = Menu;


    int score = 0;

    Font font;
    font.loadFromFile("arial.ttf");

    // Главный экран
    Text startText("SNAKE GAME", font);
    startText.setCharacterSize(100);
    startText.setPosition(100, 100);

    Text startSubText("To start press space", font);
    startSubText.setCharacterSize(50);
    startSubText.setPosition(100, 400);

    // Игра
    Text scoreText("Score: " + std::to_string(score), font);
    scoreText.setPosition(10, 10);
    scoreText.setCharacterSize(30);

    // Поражение
    Text gameOverText("GAME OVER!", font);
    gameOverText.setCharacterSize(100);
    gameOverText.setPosition(100, 100);

    Text gameOverSubText("press space to main menu", font);
    gameOverSubText.setCharacterSize(50);
    gameOverSubText.setPosition(100, 400);

    while (window.isOpen())
    {
        if (gameState == Menu)
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
                if (event.type == Event::KeyPressed)
                {
                    if (event.key.code == Keyboard::Space)
                    {
                        gameState = Game;

                        snake.clear();
                        // начальные положения
                        snake.emplace_back(GRID_WIDTH / 2, GRID_HEIGHT / 2);
                        snake.emplace_back(GRID_WIDTH / 2 + 1, GRID_HEIGHT / 2);
                        snake.emplace_back(GRID_WIDTH / 2 + 2, GRID_HEIGHT / 2);

                        food = spawnFood(snake);

                        score = 0;
                        scoreText.setString("Score: " + std::to_string(score));

                        direction = Vector2i(-1, 0);
                    }
                }
            }

            window.clear();
            window.draw(startText);
            window.draw(startSubText);
            window.display();
        }
        else if (gameState == Game) 
        {
            //////////////////////////////// ОБРАБОТКА СОБЫТИЙ ////////////////////////////////
            Vector2i newDirection = direction;
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
                if (event.type == Event::KeyPressed)
                {
                    if (event.key.code == Keyboard::Left && direction.x != 1)
                        newDirection = Vector2i(-1, 0);
                    if (event.key.code == Keyboard::Right && direction.x != -1)
                        newDirection = Vector2i(1, 0);
                    if (event.key.code == Keyboard::Up && direction.y != 1)
                        newDirection = Vector2i(0, -1);
                    if (event.key.code == Keyboard::Down && direction.y != -1)
                        newDirection = Vector2i(0, 1);
                }
            }
            direction = newDirection;
            //////////////////////////////////// ИГРОВАЯ ЛОГИКА //////////////////////////////
            // вычисляем новую позицию головы змейки
            Vector2i newHead = Vector2i(snake[0].x + direction.x, snake[0].y + direction.y);

            // проверяем выход за границы сетки
            if (newHead.x < 0)
                newHead.x = GRID_WIDTH - 1;
            if (newHead.x >= GRID_WIDTH)
                newHead.x = 0;
            if (newHead.y < 0)
                newHead.y = GRID_HEIGHT - 1;
            if (newHead.y >= GRID_HEIGHT)
                newHead.y = 0;

            if (newHead == food)
            {
                snake.push_back(food);
                food = spawnFood(snake);
                score++;
                scoreText.setString("Score: " + std::to_string(score));
            }
            else
            {
                for (int i = 3; i < snake.size(); i++)
                {
                    if (snake[i] == newHead)
                    {
                        gameState = GameOver;
                        //window.close();
                    }
                }
            }
            // двигаем все тело змеи
            for (int i = snake.size() - 1; i >= 1; i--)
            {
                snake[i] = snake[i - 1];
            }
            snake[0] = newHead;


            //////////////////////////////////// ОТРИСОВКА ///////////////////////////////////
            window.clear();
            // Отрисовка змейки
            for (int i = 0; i < snake.size(); i++)
            {
                cellShape.setPosition(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE);
                if (i == 0)
                    cellShape.setFillColor(Color::Yellow);
                else
                    cellShape.setFillColor(Color::Green);

                window.draw(cellShape);
            }
            // Отрисовка еды
            cellShape.setFillColor(Color::Red);
            cellShape.setPosition(food.x * CELL_SIZE, food.y * CELL_SIZE);
            window.draw(cellShape);
            // Отрисовка вертикальных линий
            for (int i = 0; i < GRID_WIDTH; i++)
            {
                Vertex line[] = {
                    Vertex(Vector2f(i * CELL_SIZE, 0), GRID_COLOR),
                    Vertex(Vector2f(i * CELL_SIZE, WINDOW_HEIGHT), GRID_COLOR),
                };

                window.draw(line, 2, Lines);
            }
            // Отрисовка горизонтальных линий
            for (int i = 0; i < GRID_HEIGHT; i++)
            {
                Vertex line[] = {
                    Vertex(Vector2f(0, i * CELL_SIZE), GRID_COLOR),
                    Vertex(Vector2f(WINDOW_WIDTH, i * CELL_SIZE), GRID_COLOR),
                };

                window.draw(line, 2, Lines);
            }

            window.draw(scoreText);
			window.display();
        }
        else if (gameState == GameOver)
        {

            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
                if (event.type == Event::KeyPressed)
                {
                    if (event.key.code == Keyboard::Space)
                    {
                        gameState = Menu;
                    }
                }
            }

            window.clear();
            window.draw(gameOverText);
            window.draw(gameOverSubText);
            window.draw(scoreText);
            window.display();
        }
    }

    return 0;
}