#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>

// Гравитационная постоянная (подобрана для визуализации)
const float G = 0.1f;

// Функция для отрисовки стрелки от точки start до end
void drawArrow(sf::RenderTarget &target, sf::Vector2f start, sf::Vector2f end, sf::Color color)
{
    // Основная линия стрелки
    sf::Vertex line[] =
    {
        sf::Vertex(start, color),
        sf::Vertex(end, color)
    };
    target.draw(line, 2, sf::Lines);

    // Вычисляем угол направления линии
    sf::Vector2f direction = end - start;
    float angle = std::atan2(direction.y, direction.x);
    // Параметры стрелочного наконечника
    float arrowHeadLength = 10.f;
    float arrowHeadAngle = 30.f * 3.14159265f / 180.f; // 30 градусов в радианах

    sf::Vector2f left = end - sf::Vector2f(arrowHeadLength * std::cos(angle - arrowHeadAngle),
                                              arrowHeadLength * std::sin(angle - arrowHeadAngle));
    sf::Vector2f right = end - sf::Vector2f(arrowHeadLength * std::cos(angle + arrowHeadAngle),
                                               arrowHeadLength * std::sin(angle + arrowHeadAngle));
    sf::Vertex arrowHead[] =
    {
        sf::Vertex(end, color),
        sf::Vertex(left, color),
        sf::Vertex(end, color),
        sf::Vertex(right, color)
    };
    target.draw(arrowHead, 4, sf::Lines);
}

// Структура для описания небесного тела
struct Body {
    sf::CircleShape shape;   // графическое представление
    float mass;              // масса
    sf::Vector2f position;   // положение
    sf::Vector2f velocity;   // скорость
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Гравитационная симуляция с силами и следами");
    window.setFramerateLimit(60);

    // Создаём render texture для накопления следов тел
    sf::RenderTexture trailTexture;
    if (!trailTexture.create(800, 600))
    {
        std::cerr << "Не удалось создать render texture для следов.\n";
        return -1;
    }
    // Изначально очищаем его чёрным
    trailTexture.clear(sf::Color::Black);
    trailTexture.display();

    // Вектор тел (первое тело — Солнце)
    std::vector<Body> bodies;

    // Создаём Солнце
    Body sun;
    sun.mass = 10000.f;
    sun.position = sf::Vector2f(400.f, 300.f);
    sun.velocity = sf::Vector2f(0.f, 0.f);
    sun.shape = sf::CircleShape(20.f);
    sun.shape.setFillColor(sf::Color::Yellow);
    sun.shape.setOrigin(20.f, 20.f);
    bodies.push_back(sun);

    // Лямбда-функция для создания планеты с орбитой по кругу
    auto createPlanet = [&](float orbitRadius, float mass, float visualRadius, sf::Color color)
    {
        Body planet;
        planet.mass = mass;
        // Размещаем планету справа от Солнца
        planet.position = sf::Vector2f(400.f + orbitRadius, 300.f);
        // Расчёт скорости для круговой орбиты: v = sqrt(G * M_солнца / r)
        float speed = std::sqrt(G * bodies[0].mass / orbitRadius);
        // Начальное направление скорости — перпендикулярно радиусу (вверх)
        planet.velocity = sf::Vector2f(0.f, speed);
        planet.shape = sf::CircleShape(visualRadius);
        planet.shape.setFillColor(color);
        planet.shape.setOrigin(visualRadius, visualRadius);
        bodies.push_back(planet);
    };

    // Добавляем планеты (Меркурий, Венера, Земля, Марс)
    createPlanet(50.f, 10.f, 4.f, sf::Color(169, 169, 169)); // Меркурий (серый)
    createPlanet(80.f, 20.f, 6.f, sf::Color(255, 223, 0));     // Венера (жёлтый)
    createPlanet(110.f, 30.f, 7.f, sf::Color::Blue);            // Земля
    createPlanet(140.f, 25.f, 5.f, sf::Color::Red);             // Марс

    // Корректируем скорость Солнца для сохранения суммарного импульса
    sf::Vector2f totalMomentum(0.f, 0.f);
    for (size_t i = 1; i < bodies.size(); ++i)
        totalMomentum += bodies[i].velocity * bodies[i].mass;
    bodies[0].velocity = -totalMomentum / bodies[0].mass;

    sf::Clock clock;

    // Коэффициенты масштабирования для отрисовки стрелок
    const float individualForceScale = 10.f;
    const float netForceScale = 10.f;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float dt = clock.restart().asSeconds();

        // Вычисляем для каждого тела суммарное ускорение от гравитационных сил
        std::vector<sf::Vector2f> accelerations(bodies.size(), sf::Vector2f(0.f, 0.f));
        for (size_t i = 0; i < bodies.size(); ++i)
        {
            sf::Vector2f acc(0.f, 0.f);
            for (size_t j = 0; j < bodies.size(); ++j)
            {
                if (i == j)
                    continue;
                sf::Vector2f diff = bodies[j].position - bodies[i].position;
                float distSquared = diff.x * diff.x + diff.y * diff.y;
                float distance = std::sqrt(distSquared);
                if (distance < 1.f)
                    continue;
                sf::Vector2f direction = diff / distance;
                // Ускорение от тела j: a = G * m_j / r^2
                acc += direction * (G * bodies[j].mass / distSquared);
            }
            accelerations[i] = acc;
        }

        // Обновляем скорость и положение тел
        for (size_t i = 0; i < bodies.size(); ++i)
        {
            bodies[i].velocity += accelerations[i] * dt;
            bodies[i].position += bodies[i].velocity * dt;
            bodies[i].shape.setPosition(bodies[i].position);
        }

        // Обновляем следы: постепенно затемняем старые точки и добавляем текущие позиции тел
        sf::RectangleShape fadeRect(sf::Vector2f(800, 600));
        fadeRect.setFillColor(sf::Color(0, 0, 0, 25)); // небольшая прозрачность для эффекта затухания
        trailTexture.draw(fadeRect);
        for (const auto &body : bodies)
        {
            sf::CircleShape trailDot;
            trailDot.setRadius(2.f);
            // Используем цвет тела с уменьшенной непрозрачностью
            sf::Color trailColor = body.shape.getFillColor();
            trailColor.a = 100;
            trailDot.setFillColor(trailColor);
            trailDot.setOrigin(2.f, 2.f);
            trailDot.setPosition(body.position);
            trailTexture.draw(trailDot);
        }
        trailTexture.display();

        // Отрисовка на основном окне
        window.clear(sf::Color::Black);

        // Сначала отрисовываем накопленные следы
        sf::Sprite trailSprite(trailTexture.getTexture());
        window.draw(trailSprite);

        // Для каждого тела отрисовываем стрелки индивидуальных сил и суммарную силу
        for (size_t i = 0; i < bodies.size(); ++i)
        {
            // Отрисовка индивидуальных сил от всех других тел
            for (size_t j = 0; j < bodies.size(); ++j)
            {
                if (i == j)
                    continue;
                sf::Vector2f diff = bodies[j].position - bodies[i].position;
                float distSquared = diff.x * diff.x + diff.y * diff.y;
                float distance = std::sqrt(distSquared);
                if (distance < 1.f)
                    continue;
                sf::Vector2f direction = diff / distance;
                // Сила от тела j на тело i: F = G * m_i * m_j / r^2
                sf::Vector2f force = direction * (G * bodies[i].mass * bodies[j].mass / distSquared);
                // Отрисовываем стрелку индивидуальной силы (зелёным цветом)
                drawArrow(window, bodies[i].position, bodies[i].position + force * individualForceScale, sf::Color(0, 255, 0));
            }
            // Суммарная сила = m * a
            sf::Vector2f netForce = bodies[i].mass * accelerations[i];
            // Отрисовываем стрелку суммарной силы (красным цветом)
            drawArrow(window, bodies[i].position, bodies[i].position + netForce * netForceScale, sf::Color::Red);
        }

        // Отрисовываем сами тела (Солнце и планеты)
        for (const auto &body : bodies)
            window.draw(body.shape);

        window.display();
    }

    return 0;
}