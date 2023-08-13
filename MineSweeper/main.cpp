#include <SFML/Graphics.hpp>
#include <array>
#include <random>
#include <string>
#include <iomanip>
#include <sstream>

using namespace sf;

const constexpr int EMPTY_CELL_NUMBER = 0;
const constexpr int CLOSED_CELL_NUMBER = 10;
const constexpr int FLAG_CELL_NUMBER = 11;
const constexpr int MINE_CELL_NUMBER = 9;
const constexpr int CELL_WIDTH = 32;
const constexpr int WINDOW_HEIGHT = 384;
const constexpr int WINDOW_WIDTH = 384;
const constexpr int LOSE_GAME = -1;
const constexpr int CONTINUE_GAME = 0;
const constexpr int WIN_GAME = 1;
const constexpr int TEXT_SIZE = 20;
const Color TEXT_COLOR = Color::White;

std::array<std::array<int, 10>, 10> gridLogic{};
std::array<std::array<int, 10>, 10> gridView{};
std::array<Sprite, 12> sprites;
Font font;
Texture texture;

void initializeSpritesArray(std::array<Sprite, 12> &array);
void initializeGridView();
void initializeGridLogic();
int isGameOver(int seconds);
void handleEvent(RenderWindow &window, bool gameOver);
void handleLose(RenderWindow &window);
void handleWin(RenderWindow &window);
void printTimer(RenderWindow &window, const Clock &clock, int seconds, bool gameOver);
std::string getTimeAsString(int seconds);
void printGrid(RenderWindow &window);
void printText(const std::string &message, RenderWindow &window, const float x, const float y);

int main() {
	ContextSettings settings;
	settings.antialiasingLevel = 1;
	RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Minesweeper", Style::Default, settings);
	texture.loadFromFile("MineSweeper.png");
	font.loadFromFile("Font.ttf");

	initializeSpritesArray(sprites);
	initializeGridView();
	initializeGridLogic();

	Clock clock;
	clock.restart();
	int seconds{};

	while(window.isOpen()) {
		int gameOver = isGameOver(600 - seconds);
		handleEvent(window, gameOver == LOSE_GAME || gameOver == WIN_GAME);

		window.clear();

		if(gameOver == LOSE_GAME)
			handleLose(window);
		if(gameOver == WIN_GAME)
			handleWin(window);

		printTimer(window, clock, seconds, gameOver);
		printGrid(window);	

		window.display();
	}

}

void initializeSpritesArray(std::array<Sprite, 12> &array) {
	for(size_t i = 0; i < array.size(); i++) {
		array.at(i).setTexture(texture);
		array.at(i).setTextureRect(IntRect(i * CELL_WIDTH, 0, CELL_WIDTH, CELL_WIDTH));
	}
}

void initializeGridView() {
	for(size_t i = 0; i < gridView.size(); i++)
		for(size_t j = 0; j < gridView.at(i).size(); j++)
			gridView.at(i).at(j) = CLOSED_CELL_NUMBER;
}

void initializeGridLogic() {
	for(size_t i = 0; i < gridLogic.size(); i++) {
		for(size_t j = 0; j < gridLogic.at(i).size(); j++) {
			if(std::mt19937(std::random_device{}())() % 5 == 0)
				gridLogic.at(i).at(j) = MINE_CELL_NUMBER;
			else
				gridLogic.at(i).at(j) = EMPTY_CELL_NUMBER;
		}
	}

	for(int i = 0; i < gridLogic.size(); i++) {
		for(int j = 0; j < gridLogic.at(i).size(); j++) {
			if(gridLogic.at(i).at(j) == MINE_CELL_NUMBER)
				continue;

			for(int x = i - 1; x <= i + 1; x++)
				for(int y = j - 1; y <= j + 1; y++)
					if(x >= 0 && x < gridLogic.size() && y >= 0 && y < gridLogic.at(i).size() && gridLogic.at(x).at(y) == MINE_CELL_NUMBER)
						gridLogic.at(i).at(j)++;
		}
	}
}

int isGameOver(int seconds) {
	if(seconds <= 0)
		return LOSE_GAME;

	for(size_t i = 0; i < gridView.size(); i++)
		for(size_t j = 0; j < gridView.at(i).size(); j++)
			if(gridView.at(i).at(j) == MINE_CELL_NUMBER)
				return LOSE_GAME;

	for(size_t i = 0; i < gridView.size(); i++) {
		for(size_t j = 0; j < gridView.at(i).size(); j++) {
			if(gridView.at(i).at(j) == CLOSED_CELL_NUMBER)
				return CONTINUE_GAME;

			if(gridLogic.at(i).at(j) == MINE_CELL_NUMBER)
				if(gridView.at(i).at(j) != FLAG_CELL_NUMBER)
					return CONTINUE_GAME;

			if(gridLogic.at(i).at(j) != MINE_CELL_NUMBER && gridView.at(i).at(j) == FLAG_CELL_NUMBER)
				return CONTINUE_GAME;
		}
	}

	return WIN_GAME;
}

void handleEvent(RenderWindow &window, bool gameOver) {
	Event event;
	while(window.pollEvent(event)) {
		if(event.type == Event::Closed)
			window.close();

		if(event.type == Event::MouseButtonPressed) {
			Vector2i pos = Mouse::getPosition(window);
			int x = pos.x / CELL_WIDTH - 1;
			int y = pos.y / CELL_WIDTH - 1;
			if(x >= 0 && x < gridView.size() && y >= 0 && y < gridView.at(0).size() && gameOver == CONTINUE_GAME) {
				if(event.key.code == Mouse::Left)
					gridView.at(x).at(y) = gridLogic.at(x).at(y);
				
				if(event.key.code == Mouse::Right) {
					if(gridView.at(x).at(y) == CLOSED_CELL_NUMBER)
						gridView.at(x).at(y) = FLAG_CELL_NUMBER;
					else if(gridView.at(x).at(y) == FLAG_CELL_NUMBER)
						gridView.at(x).at(y) = CLOSED_CELL_NUMBER;
				}
			}
		}
	}
}

void handleLose(RenderWindow &window) {
	gridView = gridLogic;
	printText("You lose", window, 4.75f * CELL_WIDTH, 0);
}

void handleWin(RenderWindow &window) {
	printText("You won", window, 4.75f * CELL_WIDTH, 0);
}

void printTimer(RenderWindow &window, const Clock &clock, int seconds, bool gameOver) {
	if(seconds != clock.getElapsedTime().asSeconds() && gameOver == CONTINUE_GAME) {
		seconds = static_cast<int>(clock.getElapsedTime().asSeconds());
		printText("Timer: " + getTimeAsString(600 - seconds), window, 4.3f * CELL_WIDTH, 11.f * CELL_WIDTH);
	}
}

std::string getTimeAsString(int seconds) {
	int minutes = seconds / 60;
	int remainingSeconds = seconds % 60;

	std::ostringstream formattedTime;
	formattedTime << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2) << remainingSeconds;

	return formattedTime.str();
}

void printGrid(RenderWindow &window) {
	for(int x = 0; x < gridView.size(); x++) {
		for(int y = 0; y < gridView.at(x).size(); y++) {
			sprites.at(gridView.at(x).at(y)).setPosition(CELL_WIDTH + x * CELL_WIDTH, CELL_WIDTH + y * CELL_WIDTH);
			window.draw(sprites.at(gridView.at(x).at(y)));
		}
	}
}

void printText(const std::string &message, RenderWindow &window, const float x, const float y) {
	Text text(message, font, TEXT_SIZE);
	text.setPosition(x, y);
	text.setFillColor(TEXT_COLOR);
	window.draw(text);
}