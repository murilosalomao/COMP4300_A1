#include<SFML/Graphics.hpp>
#include<iostream>
#include<fstream>
#include<memory>

using namespace std;

class SpeedComponent {
public:
	shared_ptr<sf::Shape> shape;
	float xSpd;
	float ySpd;
	string name;

	SpeedComponent(shared_ptr<sf::Shape> _shape, float xSpeed, float ySpeed, string _name) {
		shape = _shape;
		xSpd = xSpeed;
		ySpd = ySpeed;
		name = _name;
	}
};

void loadFromFile(sf::RenderWindow& window, sf::Font& font, sf::Color& fontColor, int& fontSize, vector<SpeedComponent>& shapes, const string& filename) {
	ifstream fin(filename);

	string firstToken;
	while (fin >> firstToken) {
		if (firstToken == "Window") {
			int width, height;
			fin >> width >> height;
			window.create(sf::VideoMode(width, height), "SMFL works!");
		}
		else if (firstToken == "Font") {
			string filename;
			int size, rColor, gColor, bColor;
			fin >> filename >> size >> rColor >> gColor >> bColor;
			font.loadFromFile(filename);
			fontSize = size;
			fontColor = sf::Color(rColor, gColor, bColor);
		}
		else if (firstToken == "Circle") {
			string name;
			float x, y, spdX, spdY, radius;
			int rColor, gColor, bColor;
			fin >> name >> x >> y >> spdX >> spdY >> rColor >> gColor >> bColor >> radius;
			shared_ptr<sf::Shape> shape = make_shared<sf::CircleShape>(radius);
			shape->setFillColor(sf::Color(rColor, gColor, bColor));
			shape->setPosition(x, y);
			shapes.push_back(SpeedComponent(shape, spdX, spdY, name));
		}
		else if (firstToken == "Rectangle") {
			string name;
			float x, y, spdX, spdY, width, height;
			int rColor, gColor, bColor;
			fin >> name >> x >> y >> spdX >> spdY >> rColor >> gColor >> bColor >> width >> height;
			shared_ptr<sf::Shape> shape = make_shared<sf::RectangleShape>(sf::Vector2f(width, height));
			shape->setFillColor(sf::Color(rColor, gColor, bColor));
			shape->setPosition(x, y);
			shapes.push_back(SpeedComponent(shape, spdX, spdY, name));
		}
	}
}

void draw(sf::RenderWindow& window, vector<SpeedComponent>& shapes, sf::Font font, sf::Color fontColor, int fontSize) {

	for (auto i=0; i<shapes.size(); i++) {
		window.draw(*shapes[i].shape);
		sf::Vector2f pos = shapes[i].shape->getPosition();
		sf::FloatRect bounds = shapes[i].shape->getLocalBounds();

		sf::Text text(shapes[i].name, font, fontSize);
		sf::FloatRect textBounds = text.getLocalBounds();

		text.setFillColor(fontColor);
		text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
		text.setPosition(
			pos.x + (bounds.width / 2),
			pos.y + (bounds.height / 2)
		);
		window.draw(text);
	}
}

void computeMovement(vector<SpeedComponent>& shapes, unsigned int winWidth, unsigned int winHeight) {
	for (size_t i = 0; i < shapes.size(); i++) {
		sf::Vector2f currentPos = shapes[i].shape->getPosition();
		sf::FloatRect bounds = shapes[i].shape->getLocalBounds();

		if (currentPos.x + bounds.width + shapes[i].xSpd > winWidth || currentPos.x + shapes[i].xSpd < 0) {
			shapes[i].xSpd = -shapes[i].xSpd;
		}

		if (currentPos.y + bounds.width + shapes[i].ySpd > winHeight || currentPos.y + shapes[i].ySpd < 0) {
			shapes[i].ySpd = -shapes[i].ySpd;
		}

		shapes[i].shape->setPosition(currentPos.x + shapes[i].xSpd, currentPos.y + shapes[i].ySpd);
	}
}

int main(int argc, char* argv[]) {
	sf::RenderWindow window;
	sf::Font font;
	sf::Color fontColor;
	vector<SpeedComponent> shapes;
	int fontSize;

	loadFromFile(window, font, fontColor, fontSize, shapes, "config.txt");
	window.setFramerateLimit(120);
	
	sf::Vector2u windowSize = window.getSize();
	unsigned int winWidth = windowSize.x;
	unsigned int winHeight = windowSize.y;

	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		window.clear();
		draw(window, shapes, font, fontColor, fontSize);
		computeMovement(shapes, winWidth, winHeight);
		window.display();
	}

	return 0;
}