#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

// Структура для работы с 3D-точками
struct Point3D {
    float x, y, z;
};

struct Face {
    std::vector<int> indices;
    float depth;
    sf::Color color;
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

    std::vector<Point3D> vertices = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
        {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}
    };

    std::vector<Face> faces = {
        {{0, 1, 2, 3}, 0, sf::Color::Red},
        {{4, 5, 6, 7}, 0, sf::Color::Green},
        {{0, 1, 5, 4}, 0, sf::Color::Blue},
        {{2, 3, 7, 6}, 0, sf::Color::Yellow},
        {{0, 3, 7, 4}, 0, sf::Color::Cyan},
        {{1, 2, 6, 5}, 0, sf::Color::Magenta}
    };

    float angleX = 0;
    float angleY = 0;
    float angleZ = 0;
    float fov = 500;
    float viewerDistance = 4;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        angleX += 0.01f;
        angleY += 0.01f;
        angleZ += 0.01f;

        window.clear(sf::Color::Black);

        for (auto& face : faces) {
            face.depth = 0;
            for (int index : face.indices) {
                Point3D p = vertices[index];
                rotatePoint(p, angleX, angleY, angleZ);
                face.depth += p.z;
            }
            face.depth /= face.indices.size();
        }

        std::sort(faces.begin(), faces.end(), [](const Face& a, const Face& b) {
            return a.depth > b.depth;
            });

        for (const auto& face : faces) {
            sf::ConvexShape polygon;
            polygon.setPointCount(4);
            polygon.setFillColor(face.color);

            for (size_t j = 0; j < 4; ++j) {
                Point3D p = vertices[face.indices[j]];
                rotatePoint(p, angleX, angleY, angleZ);
                polygon.setPoint(j, projectPoint(p, fov, viewerDistance, width, height));
            }

            window.draw(polygon);
        }

        window.display();
    }

    return 0;
}