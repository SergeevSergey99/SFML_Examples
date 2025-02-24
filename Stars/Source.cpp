#include <SFML/Graphics.hpp>
using namespace sf;

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

struct Camera
{
	float length;
	float scale = 10;

	Camera(float l)
	{
		length = l;
	}

	Vector2f Projection(Vector3f point)
	{
		float K = (point.x + length) / length;

		Vector2f screenPoint;
		screenPoint.y = WINDOW_HEIGHT - (point.y / K * scale + WINDOW_HEIGHT / 2);
		screenPoint.x = point.z / K * scale + WINDOW_WIDTH / 2;

		return screenPoint;
	}

};

int main()
{
	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Stars", Style::Fullscreen);
	window.setFramerateLimit(30);

	Camera camera(200);

	CircleShape shape(10);
	float speed = 10;


	Vector3f points3d[1000];
	for (int i = 0; i < 1000; i++)
	{
		points3d[i].x = 100 + rand() % 2000;
		points3d[i].y = -300 + rand() % 600;
		points3d[i].z = -300 + rand() % 600;
	}

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Up){
					camera.length -= 10;
				}
				if (event.key.code == Keyboard::Down){
					camera.length += 10;
				}
				if (event.key.code == Keyboard::Left){
					camera.scale -= 1;
				}
				if (event.key.code == Keyboard::Right){
					camera.scale += 1;
				}
				if (event.key.code == Keyboard::PageUp){
					speed++;
				}
				if (event.key.code == Keyboard::PageDown){
					speed--;
				}
			}
		}

		for (int i = 0; i < 1000; i++)
		{
			points3d[i].x-= speed;
			if (points3d[i].x <= 0)
				points3d[i].x += 2000;
		}


		window.clear();
		for (int i = 0; i < 1000; i++)
		{
			Vector2f screenPoint = camera.Projection(points3d[i]);
			shape.setPosition(screenPoint);
			shape.setRadius(10 - 10 * (points3d[i].x / 2000));
			window.draw(shape);
		}
		window.display();
	}

	return 0;
}