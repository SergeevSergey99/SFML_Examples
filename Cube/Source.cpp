#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

// Структура для работы с 3D-точками
struct Point3D {
    float x, y, z;
};

// Функция для проекции 3D-точки в 2D-пространство
sf::Vector2f projectPoint(const Point3D& point, float fov, float viewerDistance, float width, float height) {
    float factor = fov / (viewerDistance + point.z);
    float x = point.x * factor + width / 2;
    float y = -point.y * factor + height / 2;
    return sf::Vector2f(x, y);
}

// Функция для вращения точки в 3D-пространстве вокруг осей
void rotatePoint(Point3D& point, float angleX, float angleY, float angleZ) {
    // Вращение вокруг оси X
    float cosX = std::cos(angleX);
    float sinX = std::sin(angleX);
    float y = point.y * cosX - point.z * sinX;
    float z = point.y * sinX + point.z * cosX;
    point.y = y;
    point.z = z;

    // Вращение вокруг оси Y
    float cosY = std::cos(angleY);
    float sinY = std::sin(angleY);
    float x = point.x * cosY + point.z * sinY;
    z = -point.x * sinY + point.z * cosY;
    point.x = x;
    point.z = z;

    // Вращение вокруг оси Z
    float cosZ = std::cos(angleZ);
    float sinZ = std::sin(angleZ);
    x = point.x * cosZ - point.y * sinZ;
    y = point.x * sinZ + point.y * cosZ;
    point.x = x;
    point.y = y;
}

int main() {
    const int width = 800;
    const int height = 600;
    sf::RenderWindow window(sf::VideoMode(width, height), "3D Cube with SFML");
    window.setFramerateLimit(60);

    // Задаем вершины куба
    std::vector<Point3D> vertices = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
        {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}
    };

    // Определяем рёбра куба (индексы вершин)
    std::vector<std::pair<int, int>> edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Задняя грань
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Передняя грань
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Соединяющие рёбра
    };

    float angleX = 0;
    float angleY = 0;
    float angleZ = 0;
    float fov = 500; // Поле зрения
    float viewerDistance = 4; // Расстояние до "камеры"

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Обновление углов вращения
        angleX += 0.01f;
        angleY += 0.01f;
        angleZ += 0.01f;

        // Очистка окна
        window.clear(sf::Color::Black);

        // Рисуем рёбра куба
        for (const auto& edge : edges) {
            Point3D p1 = vertices[edge.first];
            Point3D p2 = vertices[edge.second];

            // Вращение точек
            rotatePoint(p1, angleX, angleY, angleZ);
            rotatePoint(p2, angleX, angleY, angleZ);

            // Проекция точек
            sf::Vector2f point1 = projectPoint(p1, fov, viewerDistance, width, height);
            sf::Vector2f point2 = projectPoint(p2, fov, viewerDistance, width, height);

            // Отрисовка линии
            sf::Vertex line[] = {
                sf::Vertex(point1, sf::Color::White),
                sf::Vertex(point2, sf::Color::White)
            };
            window.draw(line, 2, sf::Lines);
        }

        // Отображение содержимого окна
        window.display();
    }

    return 0;
}
