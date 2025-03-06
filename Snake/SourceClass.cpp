#include <SFML/Graphics.hpp>
#include <vector>
#include <set>
#include <iostream>

using namespace sf;

const int CELL_SIZE = 50;
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 15;

const Color GRID_COLOR = Color(50, 50, 50);


const int WINDOW_WIDTH = CELL_SIZE * GRID_WIDTH;
const int WINDOW_HEIGHT = CELL_SIZE * GRID_HEIGHT;



enum GameState
{
	Menu,
    Play,
    GameOver
};

class Game
{
    RenderWindow window;
    RectangleShape cellShape;
    RectangleShape gameOverPanel;
    Font font;

    std::vector<Vector2i> snake;
    Vector2i food;
    Vector2i direction;
    int score = 0;

    GameState gameState;

    // menu
    Text startText;
    Text startSubText;
    // игра
    Text scoreText;
    // поражение
    Text gameOverText;
    Text gameOverSubText;

    struct Vector2iCompare
    {
	    bool operator()(const Vector2i & a, const Vector2i & b) const
	    {
            if (a.x == b.x)
            {
                return a.y > b.y;
            }
            return a.x > b.x;
	    }
    };

	std::set<Vector2i, Vector2iCompare> availablePositions;


public:
    Game() :
		window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SNAKE"),
		cellShape(Vector2f(CELL_SIZE, CELL_SIZE)),
		gameOverPanel(Vector2f(WINDOW_WIDTH, WINDOW_WIDTH))
    {
        window.setFramerateLimit(5);
        gameOverPanel.setFillColor(Color(0, 0, 0, 100));

        gameState = Menu;
        score = 0;

        font.loadFromFile("arial.ttf");

        startText.setString("SNAKE GAME");
    	startText.setFont(font);
        startText.setCharacterSize(100);
        startText.setPosition(100, 100);

        startSubText.setString("To start press space");
    	startSubText.setFont(font);
        startSubText.setCharacterSize(50);
        startSubText.setPosition(100, 400);

    	// Игра
    	scoreText.setFont(font);
        scoreText.setPosition(10, 10);
        scoreText.setCharacterSize(30);

        // Поражение
        gameOverText.setString("GAME OVER!");
    	gameOverText.setFont(font);
        gameOverText.setCharacterSize(100);
        gameOverText.setPosition(100, 100);

        gameOverSubText.setString("press space to main menu");
    	gameOverSubText.setFont(font);
        gameOverSubText.setCharacterSize(50);
        gameOverSubText.setPosition(100, 400);
    }

    void Run()
    {
	    while (window.isOpen())
	    {
            HandleEvents();
            UpdateLogic();
            Render();		    
	    }
    }

private:
    void HandleEvents()
    {
        if (gameState == Menu) HandleMenuEvents();
        else if (gameState == Play) HandlePlayEvents();
        else if (gameState == GameOver) HandleGameOverEvents();
    }

    void HandleMenuEvents()
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
                    gameState = Play;
                    resetGame();
                }
            }
        }
    }
    void HandlePlayEvents()
    {
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
    }
    void HandleGameOverEvents()
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
    }
    void UpdateLogic()
    {
        if (gameState == Menu)
        {

        }
        else if (gameState == Play) UpdatePlayLogic();
        else if (gameState == GameOver)
        {
	        
        }
    }


    void UpdatePlayLogic()
    {
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
            food = spawnFood();
            score++;
            scoreText.setString("Score: " + std::to_string(score));
        }
        else
        {
            availablePositions.insert(snake[snake.size() - 1]);

            for (int i = 3; i < snake.size(); i++)
            {
                if (snake[i] == newHead)
                {
                    gameState = GameOver;
                }
            }
        }
        // двигаем все тело змеи
        for (int i = snake.size() - 1; i >= 1; i--)
        {
            snake[i] = snake[i - 1];
        }
        snake[0] = newHead;

        availablePositions.erase(newHead);

    }
    void Render()
    {
        window.clear();

        if (gameState == Menu) RenderMenu();
        else if (gameState == Play) RenderPlay();
        else if (gameState == GameOver) RenderGameOver();

        window.display();
    }

    void RenderMenu()
    {
        window.draw(startText);
        window.draw(startSubText);
	    
    }
    void RenderPlay()
    {
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
    }
    void RenderGameOver()
    {
        RenderPlay();
        window.draw(gameOverPanel);
        window.draw(gameOverText);
        window.draw(gameOverSubText);
    }
    void resetGame()
    {
        availablePositions.clear();
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            for (int y = 0; y < GRID_HEIGHT; y++)
            {
                availablePositions.insert(Vector2i(x, y));
            }
        }

        snake.clear();
        // начальные положения
        snake.emplace_back(GRID_WIDTH / 2, GRID_HEIGHT / 2);
        snake.emplace_back(GRID_WIDTH / 2 + 1, GRID_HEIGHT / 2);
        snake.emplace_back(GRID_WIDTH / 2 + 2, GRID_HEIGHT / 2);

        for (int i = 0; i < snake.size(); i++)
        {
            availablePositions.erase(snake[i]);
        }

        food = spawnFood();

        score = 0;
        scoreText.setString("Score: " + std::to_string(score));

        direction = Vector2i(-1, 0);

	    
    }

	Vector2i spawnFood()
	{
        int randIndex = rand() % availablePositions.size();

        auto it = availablePositions.begin();
        std::advance(it, randIndex);
        return *it;
	}
};

int main()
{
    Game game;
    game.Run();
}