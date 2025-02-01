#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <utility>

// Структура для 4D-вектора
struct Vec4 {
    float x, y, z, w;
};

// Структура для 3D-вектора
struct Vec3 {
    float x, y, z;
};

// Функция, выполняющая последовательные повороты 4D-вектора в 6 плоскостях
Vec4 rotate4D(const Vec4& v, float angleXY, float angleXZ, float angleXW,
    float angleYZ, float angleYW, float angleZW)
{
    Vec4 r = v;
    float temp;

    // Поворот в плоскости XY
    temp = r.x;
    r.x = r.x * cos(angleXY) - r.y * sin(angleXY);
    r.y = temp * sin(angleXY) + r.y * cos(angleXY);

    // Поворот в плоскости XZ
    temp = r.x;
    r.x = r.x * cos(angleXZ) - r.z * sin(angleXZ);
    r.z = temp * sin(angleXZ) + r.z * cos(angleXZ);

    // Поворот в плоскости XW
    temp = r.x;
    r.x = r.x * cos(angleXW) - r.w * sin(angleXW);
    r.w = temp * sin(angleXW) + r.w * cos(angleXW);

    // Поворот в плоскости YZ
    temp = r.y;
    r.y = r.y * cos(angleYZ) - r.z * sin(angleYZ);
    r.z = temp * sin(angleYZ) + r.z * cos(angleYZ);

    // Поворот в плоскости YW
    temp = r.y;
    r.y = r.y * cos(angleYW) - r.w * sin(angleYW);
    r.w = temp * sin(angleYW) + r.w * cos(angleYW);

    // Поворот в плоскости ZW
    temp = r.z;
    r.z = r.z * cos(angleZW) - r.w * sin(angleZW);
    r.w = temp * sin(angleZW) + r.w * cos(angleZW);

    return r;
}

// Проекция из 4D в 3D с использованием перспективы по w
Vec3 project4Dto3D(const Vec4& v, float distance4d)
{
    float factor = distance4d / (distance4d - v.w);
    return { v.x * factor, v.y * factor, v.z * factor };
}

// Проекция из 3D в 2D с использованием перспективы по z
sf::Vector2f project3Dto2D(const Vec3& v, float distance3d)
{
    float factor = distance3d / (distance3d - v.z);
    return sf::Vector2f(v.x * factor, v.y * factor);
}

int main()
{
    const int windowWidth = 800;
    const int windowHeight = 600;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "4D Cube");
    window.setFramerateLimit(60);

    // Формирование вершин тессеракта (4-мерного куба): каждая координата равна ±1
    std::vector<Vec4> vertices;
    for (int i = 0; i < 16; ++i) {
        Vec4 v;
        v.x = (i & 1) ? 1.0f : -1.0f;
        v.y = (i & 2) ? 1.0f : -1.0f;
        v.z = (i & 4) ? 1.0f : -1.0f;
        v.w = (i & 8) ? 1.0f : -1.0f;
        vertices.push_back(v);
    }

    // Генерация рёбер: соединяются пары вершин, различающиеся ровно по одной координате
    std::vector<std::pair<int, int>> edges;
    for (int i = 0; i < 16; ++i) {
        for (int j = i + 1; j < 16; ++j) {
            int diffCount = 0;
            if (vertices[i].x != vertices[j].x) diffCount++;
            if (vertices[i].y != vertices[j].y) diffCount++;
            if (vertices[i].z != vertices[j].z) diffCount++;
            if (vertices[i].w != vertices[j].w) diffCount++;
            if (diffCount == 1) {
                edges.push_back({ i, j });
            }
        }
    }

    sf::Clock clock;

    // Основной цикл приложения
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float time = clock.getElapsedTime().asSeconds();

        // Выбор углов поворота для каждой из 6 плоскостей (значения можно менять для различных эффектов)
        float angleXY = time * 0.5f;
        float angleXZ = time * 0.3f;
        float angleXW = time * 0.2f;
        float angleYZ = time * 0.4f;
        float angleYW = time * 0.3f;
        float angleZW = time * 0.2f;

        // Вектор для хранения проецированных на 2D точек
        std::vector<sf::Vector2f> projectedPoints;
        for (const auto& vertex : vertices)
        {
            // Поворот вершины в 4D
            Vec4 rotated = rotate4D(vertex, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);

            // Проекция из 4D в 3D
            Vec3 projected3d = project4Dto3D(rotated, 3.0f);  // расстояние для 4D-проекции (подберите >1)

            // Проекция из 3D в 2D
            sf::Vector2f projected2d = project3Dto2D(projected3d, 5.0f);  // расстояние для 3D-проекции (подберите >1)

            // Масштабирование и перенос в центр окна
            float scale = 100.0f;
            projected2d.x = projected2d.x * scale + windowWidth / 2;
            projected2d.y = projected2d.y * scale + windowHeight / 2;
            projectedPoints.push_back(projected2d);
        }

        window.clear(sf::Color::Black);

        // Отрисовка рёбер
        for (const auto& edge : edges)
        {
            sf::Vertex line[] =
            {
                sf::Vertex(projectedPoints[edge.first], sf::Color::White),
                sf::Vertex(projectedPoints[edge.second], sf::Color::White)
            };
            window.draw(line, 2, sf::Lines);
        }

        window.display();
    }

    return 0;
}
