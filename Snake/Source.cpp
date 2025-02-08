#include <SFML/Graphics.hpp>
#include <vector>
using namespace sf;

const int CELL_SIZE = 50;
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 15;

const Color GRID_COLOR = Color(50, 50, 50);


const int WINDOW_WIDTH = GRID_WIDTH * CELL_SIZE;
const int WINDOW_HEIGHT = GRID_HEIGHT * CELL_SIZE;

Vector2i spawnFood()
{
    Vector2i pos;
    pos.x = rand() % GRID_WIDTH;
    pos.y = rand() % GRID_HEIGHT;
    return pos;
}

int main()
{
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML works!");
    window.setFramerateLimit(5);

    std::vector<Vector2i> snake;
    // начальное положение
    snake.push_back(Vector2i( GRID_WIDTH / 2, GRID_HEIGHT / 2 ));
    snake.push_back(Vector2i(GRID_WIDTH / 2 + 1, GRID_HEIGHT / 2 ));
    snake.push_back(Vector2i(GRID_WIDTH / 2 + 2, GRID_HEIGHT / 2 ));

    // Объект для отрисовки змейки
    RectangleShape cellShape(Vector2f(CELL_SIZE, CELL_SIZE));
    cellShape.setFillColor(Color::Green);

    // Начальное направление движения: вправо
    Vector2i direction(-1, 0);

    Vector2i food = spawnFood();

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            // Обработка нажатий клавиш
            if (event.type == sf::Event::KeyPressed)
            {
                // Запрещаем разворот на 180 градусов
                if (event.key.code == sf::Keyboard::Up)
                    direction = { 0, -1 };
                else if (event.key.code == sf::Keyboard::Down)
                    direction = { 0, 1 };
                else if (event.key.code == sf::Keyboard::Left)
                    direction = { -1, 0 };
                else if (event.key.code == sf::Keyboard::Right)
                    direction = { 1, 0 };
            }
        }
        ////////////////////// ЛОГИКА /////////////////////////////
        Vector2i newHead = Vector2i(snake[0].x + direction.x, snake[0].y + direction.y);

        if (newHead.x < 0) 
            newHead.x = GRID_WIDTH - 1;
        if (newHead.x >= GRID_WIDTH) 
            newHead.x = 0;
        if (newHead.y < 0)
            newHead.y = GRID_HEIGHT - 1;
        if (newHead.y >= GRID_HEIGHT)
            newHead.y = 0;

        for (int i = snake.size() - 1; i > 0; i--)
        {
            snake[i] = snake[i - 1];
        }
        snake[0] = newHead;

        if (newHead == food)
        {
            snake.push_back(food);
            food = spawnFood();
        }
        
        ////////////////////////// ОТРИСОВКА //////////////////////////
        window.clear();
    	// Отрисовка змейки
        for (int i = 0; i < snake.size(); i++)
        {
            cellShape.setPosition(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE);
            window.draw(cellShape);
        }
        cellShape.setPosition(food.x * CELL_SIZE, food.y * CELL_SIZE);
        window.draw(cellShape);
        // Отрисовка сетки
        for (int i = 0; i <= GRID_WIDTH; i++)
        {
            Vertex line[] = {
                Vertex(Vector2f(i * CELL_SIZE, 0), GRID_COLOR),
                Vertex(Vector2f(i * CELL_SIZE, WINDOW_HEIGHT), GRID_COLOR)
            };
            window.draw(line, 2, Lines);
        }
        for (int i = 0; i <= GRID_HEIGHT; i++)
        {
            Vertex line[] = {
                Vertex(Vector2f(0, i * CELL_SIZE), GRID_COLOR),
                Vertex(Vector2f(WINDOW_WIDTH, i * CELL_SIZE), GRID_COLOR)
            };
            window.draw(line, 2, sf::Lines);
        }
        window.display();
    }

    return 0;
}