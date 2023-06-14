#include<SFML/Graphics.hpp>
#include<iostream>
#include<fstream>
#include<memory>

using namespace std;

class SpeedComponent {
public:
	unique_ptr<sf::Shape> shape;
	float xSpd;
	float ySpd;

	SpeedComponent(float xSpeed, float ySpeed) {
		xSpd = xSpeed;
		ySpd = ySpeed;
	}
};

void loadFromFile(sf::RenderWindow& window, sf::Font& font, sf::Color& fontColor, int& fontSize, vector<shared_ptr<SpeedComponent>>& shapes, vector<string>& names, vector<SpeedComponent*>& speeds, const string& filename) {
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
			shapes.push_back(shape);
			names.push_back(name);
			speeds.push_back(new SpeedComponent(spdX, spdY));
		}
		else if (firstToken == "Rectangle") {
			string name;
			float x, y, spdX, spdY, width, height;
			int rColor, gColor, bColor;
			fin >> name >> x >> y >> spdX >> spdY >> rColor >> gColor >> bColor >> width >> height;
			shared_ptr<sf::Shape> shape = make_shared<sf::RectangleShape>(sf::Vector2f(width, height));
			shape->setFillColor(sf::Color(rColor, gColor, bColor));
			shape->setPosition(x, y);
			shapes.push_back(shape);
			names.push_back(name);
			speeds.push_back(new SpeedComponent(spdX, spdY));
		}
	}
}

void draw(sf::RenderWindow& window, vector<shared_ptr<SpeedComponent>>& shapes, vector<string> names, sf::Font font, sf::Color fontColor, int fontSize) {

	for (auto i=0; i<shapes.size(); i++) {
		window.draw(*shapes[i]);
		sf::Vector2f pos = shapes[i]->getPosition();
		sf::FloatRect bounds = shapes[i]->getLocalBounds();

		sf::Text text(names[i], font, fontSize);
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

void computeMovement(vector<shared_ptr<SpeedComponent>>& shapes, vector<SpeedComponent*>& speeds, unsigned int winWidth, unsigned int winHeight) {
	for (size_t i = 0; i < shapes.size(); i++) {
		sf::Vector2f currentPos = shapes[i]->getPosition();
		sf::FloatRect bounds = shapes[i]->getLocalBounds();

		if (currentPos.x + bounds.width + speeds[i]->xSpd > winWidth || currentPos.x + speeds[i]->xSpd < 0) {
			speeds[i]->xSpd = -speeds[i]->xSpd;
		}

		if (currentPos.y + bounds.width + speeds[i]->ySpd > winHeight || currentPos.y + speeds[i]->ySpd < 0) {
			speeds[i]->ySpd = -speeds[i]->ySpd;
		}

		shapes[i]->setPosition(currentPos.x + speeds[i]->xSpd, currentPos.y + speeds[i]->ySpd);
	}
}

int main(int argc, char* argv[]) {
	sf::RenderWindow window;
	//vector<sf::Shape*> shapes;
	vector<shared_ptr<SpeedComponent>> shapes;
	vector<string> names;
	vector<SpeedComponent*> speeds;
	sf::Font font;
	int fontSize;
	sf::Color fontColor;

	loadFromFile(window, font, fontColor, fontSize, shapes, names, speeds, "config.txt");
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

			if (event.type == sf::Event::KeyPressed) {
				cout << "Key pressed with code = " << event.key.code << endl;

				if (event.key.code == sf::Keyboard::X) {
					speeds[0]->xSpd *= -1.0f;
				}
			}
		}

		window.clear();
		draw(window, shapes, names, font, fontColor, fontSize);
		computeMovement(shapes, speeds, winWidth, winHeight);
		window.display();
	}

	return 0;
}