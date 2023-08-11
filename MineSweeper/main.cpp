#include <SFML/Graphics.hpp>

using namespace sf;

const constexpr int MINE_NUMBER = 9;
const constexpr int CELL_WIDTH = 32;

int main() {
	srand(time(0));

	RenderWindow window(VideoMode(400, 400), "Minesweeper");


	int gridLogic[11][11];
	int gridView[11][11];

	Texture t;
	t.loadFromFile("MineSweeper.png");
	Sprite s(t);

	for(int i = 1; i <= 10; i++) {
		for(int j = 1; j <= 10; j++) {
			gridView[i][j] = 10;
			if(rand() % 5 == 0)
				gridLogic[i][j] = MINE_NUMBER;
			else
				gridLogic[i][j] = 0;
		}
	}
		

	for(int i = 1; i <= 10; i++) {
		for(int j = 1; j <= 10; j++) {
			int n = 0;
			if(gridLogic[i][j] == MINE_NUMBER) continue;
			if(gridLogic[i + 1][j] == MINE_NUMBER) n++;
			if(gridLogic[i][j + 1] == MINE_NUMBER) n++;
			if(gridLogic[i - 1][j] == MINE_NUMBER) n++;
			if(gridLogic[i][j - 1] == MINE_NUMBER) n++;
			if(gridLogic[i + 1][j + 1] == MINE_NUMBER) n++;
			if(gridLogic[i - 1][j - 1] == MINE_NUMBER) n++;
			if(gridLogic[i - 1][j + 1] == MINE_NUMBER) n++;
			if(gridLogic[i + 1][j - 1] == MINE_NUMBER) n++;
			gridLogic[i][j] = n;
		}
	}

	while(window.isOpen()) {
		Vector2i pos = Mouse::getPosition(window);
		int x = pos.x / CELL_WIDTH;
		int y = pos.y / CELL_WIDTH;

		Event e;
		while(window.pollEvent(e)) {
			if(e.type == Event::Closed)
				window.close();

			if (e.type == Event::MouseButtonPressed) {
				if (FloatRect(0, 0, window.getSize().x, window.getSize().y).contains(pos.x, pos.y)) {
					if (e.mouseButton.button == Mouse::Left)
						gridView[x][y] = gridLogic[x][y];
					else if (e.mouseButton.button == Mouse::Right)
						gridView[x][y] = 11;
				}
			}

			window.clear(Color::White);

			for(int i = 1; i <= 10; i++)
				for(int j = 1; j <= 10; j++) {
					if(gridView[x][y] == MINE_NUMBER)
						gridView[i][j] = gridLogic[i][j];

					s.setTextureRect(IntRect(gridView[i][j] * CELL_WIDTH, 0, CELL_WIDTH, CELL_WIDTH));

					s.setPosition(i * CELL_WIDTH, j * CELL_WIDTH);

					window.draw(s);
				}
			window.display();
		}

		
	}

	return 0;
}