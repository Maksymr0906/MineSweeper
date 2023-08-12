#include <SFML/Graphics.hpp>
#include <array>
#include <random>
#include <string>

using namespace sf;

const constexpr int EMPTY_OPEN_CELL_NUMBER = 0;
const constexpr int EMPTY_CLOSED_CELL_NUMBER = 10;
const constexpr int FLAG_CELL_NUMBER = 11;
const constexpr int MINE_NUMBER = 9;
const constexpr int CELL_WIDTH = 32;
const constexpr int WINDOW_HEIGHT = 384;
const constexpr int WINDOW_WIDTH = 384;
const constexpr int LOSE_GAME = -1;
const constexpr int WIN_GAME = 1;

std::array<std::array<int, 10>, 10> gridLogic{};
std::array<std::array<int, 10>, 10> gridView{};

void eventHandler(RenderWindow &window, bool gameOver);
void fillArrayWithSprites(std::array<Sprite, 12> &array, const Texture &texture);
int isGameOver(int seconds);

int main() {
	ContextSettings settings;
	settings.antialiasingLevel = 1;
	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Minesweeper", Style::Default, settings);
	
	Texture texture;
	texture.loadFromFile("MineSweeper.png");
	
	std::array<Sprite, 12> sprites;
	fillArrayWithSprites(sprites, texture);

	for(size_t i = 0; i < gridView.size(); i++) {
		for(size_t j = 0; j < gridView.at(i).size(); j++) {
			gridView.at(i).at(j) = EMPTY_CLOSED_CELL_NUMBER;
		}
	}

	for(size_t i = 0; i < gridLogic.size(); i++) {
		for(size_t j = 0; j < gridLogic.at(i).size(); j++) {
			if(std::mt19937(std::random_device{}())() % 5 == 0) {
				gridLogic.at(i).at(j) = MINE_NUMBER;
			}
			else {
				gridLogic.at(i).at(j) = EMPTY_OPEN_CELL_NUMBER;
			}
		}
	}

	for(int i = 0; i < gridLogic.size(); i++) {
		for(int j = 0; j < gridLogic.at(i).size(); j++) {
			if(gridLogic.at(i).at(j) == MINE_NUMBER)
				continue;

			for(int x = i - 1; x <= i + 1; x++)
				for(int y = j - 1; y <= j + 1; y++)
					if(x >= 0 && x < gridLogic.size() && y >= 0 && y < gridLogic.at(i).size() && gridLogic.at(x).at(y) == MINE_NUMBER)
						gridLogic.at(i).at(j)++;
		}
	}

	Clock clock;
	clock.restart();
	int seconds = 0;

	while(window.isOpen()) {
		int gameOver = isGameOver(600 - seconds);
		eventHandler(window, gameOver == LOSE_GAME || gameOver == WIN_GAME);

		window.clear();

		for(int x = 0; x < gridView.size(); x++) {
			for(int y = 0; y < gridView.at(x).size(); y++) {
				sprites.at(gridView.at(x).at(y)).setPosition(CELL_WIDTH + x * CELL_WIDTH, CELL_WIDTH + y * CELL_WIDTH);
				window.draw(sprites.at(gridView.at(x).at(y)));
			}
		}
			
		window.display();
	}

}

void fillArrayWithSprites(std::array<Sprite, 12> &array, const Texture &texture) {
	for(size_t i = 0; i < array.size(); i++) {
		array.at(i).setTexture(texture);
		array.at(i).setTextureRect(IntRect(i * CELL_WIDTH, 0, CELL_WIDTH, CELL_WIDTH));
	}
}

void eventHandler(RenderWindow &window, bool gameOver) {
	Event event;
	while(window.pollEvent(event)) {
		if(event.type == Event::Closed) {
			window.close();
		}

		if(event.type == Event::MouseButtonPressed) {
			Vector2i pos = Mouse::getPosition(window);
			int x = pos.x / CELL_WIDTH - 1;
			int y = pos.y / CELL_WIDTH - 1;
			if(x >= 0 && x < gridView.size() && y >= 0 && y < gridView.at(0).size()) {
				if(event.key.code == Mouse::Left) {
					gridView.at(x).at(y) = gridLogic.at(x).at(y);
				}
				if(event.key.code == Mouse::Right) {
					if(gridView.at(x).at(y) == EMPTY_CLOSED_CELL_NUMBER) {
						gridView.at(x).at(y) = FLAG_CELL_NUMBER;
					}
					else if(gridView.at(x).at(y) == FLAG_CELL_NUMBER) {
						gridView.at(x).at(y) = EMPTY_CLOSED_CELL_NUMBER;
					}
				}
			}
		}
	}
}

int isGameOver(int seconds) {
	return LOSE_GAME;
}