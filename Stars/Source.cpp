#include <SFML/Graphics.hpp>
using namespace sf;

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const int MAX_DISTANCE = 2000;
const int MAX_OFFSET = 500;
const int STARS_COUNT = 1000;
const int MAX_RADIUS = 10;


struct Camera
{
	float L;
	float scale;

	Camera(float l, float s)
	{
		L = l;
		scale = s;
	}

	Vector2f Projection(Vector3f point)
	{
		float K = (L + point.x) / L;

		Vector2f screenPoint;
		screenPoint.y = WINDOW_HEIGHT - (point.y / K * scale + WINDOW_HEIGHT / 2);
		screenPoint.x = point.z / K * scale + WINDOW_WIDTH / 2;

		return screenPoint;
	}

};

int main()
{
	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Stars", Style::Fullscreen);
	window.setFramerateLimit(60);

	Camera camera(200, 10);

	Vector3f points3d[STARS_COUNT];

	for (int i = 0; i < STARS_COUNT; i++)
	{
		points3d[i].x = rand() % MAX_DISTANCE;
		points3d[i].y = -MAX_OFFSET + rand() % (MAX_OFFSET * 2 + 1);
		points3d[i].z = -MAX_OFFSET + rand() % (MAX_OFFSET * 2 + 1);
	}

	CircleShape shape;

	float speed = 10;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Up)
				{
					camera.L -= 100;
				}
				if (event.key.code == Keyboard::Down)
				{
					camera.L += 100;
				}
				if (event.key.code == Keyboard::Left)
				{
					camera.scale--;
				}
				if (event.key.code == Keyboard::Right)
				{
					camera.scale++;
				}
				if (event.key.code == Keyboard::PageUp)
				{
					speed++;
				}
				if (event.key.code == Keyboard::PageDown)
				{
					speed--;
				}
				if (event.key.code == Keyboard::Escape)
				{
					window.close();
				}
			}
		}

		for (int i = 0; i < STARS_COUNT; i++)
		{
			points3d[i].x -= speed;
			if (points3d[i].x <= 0)
			{
				points3d[i].x = MAX_DISTANCE;
			}
			if (points3d[i].x > MAX_DISTANCE)
			{
				points3d[i].x = 0;
			}
		}

		window.clear();

		for (int i = 0; i < STARS_COUNT; i++)
		{
			shape.setPosition(camera.Projection(points3d[i]));
			shape.setRadius(MAX_RADIUS - MAX_RADIUS * points3d[i].x / MAX_DISTANCE);
			window.draw(shape);
		}
		window.display();
	}

	return 0;
}
