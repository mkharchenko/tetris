#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace sf;
using namespace std;

RenderWindow window(VideoMode(320, 480), "Tetris");

Texture tile;
Texture background;
Texture frame;
Texture nextframe;
Font smw;

const int Height = 20;
const int Width = 10;

#define ColorsNum 7
#define FiguresNum 7

enum colors { Blue, Purple, Red, Green, Yellow, SemiBlue, Orange };
enum Figures { I, Z, S, T, L, J, O };

struct Point {
	int x, y;
	Point() {
		x = 0;
		y = 0;
	}
};

class Brick {
private:
	int Color;
	Point vector2d;
	Sprite img;
public:
	Brick() {
		Color = 0;
		vector2d.x = 0;
		vector2d.y = 0;
		img.setTexture(tile);
	}
	Brick(int x, int y, int clr = 0) {
		Color = clr;
		vector2d.x = x;
		vector2d.y = y;
		img.setTexture(tile);
	}
	Brick(Brick& b) {
		Color = b.Color;
		vector2d.x = b.vector2d.x;
		vector2d.y = b.vector2d.y;
		img = b.img;
	}
	void Draw(bool next = false) {
		img.setTextureRect(IntRect(Color * 18, 0, 18, 18));
		
		if (!next)
		{
			img.setPosition(vector2d.x * 18, vector2d.y * 18);
			img.move(28.0, 31.0); //offset
		}
		else
		{
			img.setPosition(vector2d.x * 18 + 18, vector2d.y * 18);
			img.move(237.0, 55.0);
		}
		window.draw(img);
	}
	Point getPosition() {
		return vector2d;
	}
	void setPosition(int x, int y) {
		vector2d.x = x;
		vector2d.y = y;
	}
};

class Figure {
protected:
	Brick* bricks[4];
	bool canRotate = true;
	Figure() {  };
public:
	static Figure* createFigure(int ID, int clr);
	void Draw(bool next = false) {
		for (int i = 0; i < 4; i++) {
			bricks[i]->Draw(next);
		}
	}
	Brick* getBrick(int n) {
		if (n < 0 || n > 3) {
			abort();
		}
		return bricks[n];
	}
	void moveFigure(int dx, int dy) {
		for (int i = 0; i < 4; i++) {
			bricks[i]->setPosition(bricks[i]->getPosition().x + dx, bricks[i]->getPosition().y + dy);
		}
	}
	bool getColision(int x, int y)
	{
		for (int i = 0; i < 4; i++)
		{
			int tx = bricks[i]->getPosition().x;
			int ty = bricks[i]->getPosition().y;
			if (tx < 0 || tx >= Width || ty < 0 || ty >= Height) return false;
			if (tx == x && ty == y) return false;
		}
		return true;
	}
	bool getRotateCollision(int x, int y)
	{
		if (!canRotate) return false;
		Figure* f = getRotatedFigure();
		bool ans = f->getColision(x, y);
		delete f;
		return ans;
	}
	void rotate() {
		Figure* f = getRotatedFigure();
		for (int i = 0; i < 4; i++) {
			bricks[i]->setPosition(f->bricks[i]->getPosition().x, f->bricks[i]->getPosition().y);
		}
		delete f;
	}
	virtual Figure* getRotatedFigure() = 0;
	~Figure() {
		for (int i = 0; i < 4; i++) delete bricks[i];
	}
};

bool** copy(bool** a, int n)
{
	bool** c = new bool* [n];
	for (int i = 0; i < n; ++i)
	{
		c[i] = new bool[n];
		for (int j = 0; j < n; ++j)
		{
			c[i][j] = a[i][j];
		}
	}
	return c;
}

void dispose(bool** a, int n)
{
	for (int i = 0; i < n; ++i)
	{
		delete[] a[i];
	}
	delete[] a;
}

void matrRotate(bool** a, int n)
{
	bool** c = copy(a, n);

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			a[j][n - 1 - i] = c[i][j];
		}
	}

	dispose(c, n);
}

class figI : public Figure {
public:
	figI(int clr) : Figure()
	{
		bricks[0] = new Brick(0, 0, clr);
		bricks[1] = new Brick(0, 1, clr);
		bricks[2] = new Brick(0, 2, clr);
		bricks[3] = new Brick(0, 3, clr);
	}
	Figure* getRotatedFigure() {
		Point p = bricks[1]->getPosition();
		Figure* ans = Figure::createFigure(0, 0);
		for (int i = 0; i < 4; i++) {
			int tx, ty;
			tx = p.x - bricks[i]->getPosition().y + p.y;
			ty = p.y - bricks[i]->getPosition().x + p.x;
			ans->getBrick(i)->setPosition(tx, ty);
		}
		return ans;
	}
};

class figZ : public Figure {
public:
	figZ(int clr) : Figure()
	{
		bricks[0] = new Brick(0, 0, clr);
		bricks[1] = new Brick(0, 1, clr);
		bricks[2] = new Brick(1, 1, clr);
		bricks[3] = new Brick(1, 2, clr);
	}
	Figure* getRotatedFigure() {
		Point p = bricks[1]->getPosition();
		Figure* ans = Figure::createFigure(0, 0);
		for (int i = 0; i < 4; i++) {
			int tx, ty;
			tx = p.x - bricks[i]->getPosition().y + p.y;
			ty = p.y - bricks[i]->getPosition().x + p.x;
			ans->getBrick(i)->setPosition(tx, ty);
		}
		return ans;
	}
};
class figS : public Figure {
public:
	figS(int clr) : Figure()
	{
		bricks[0] = new Brick(1, 0, clr);
		bricks[1] = new Brick(1, 1, clr);
		bricks[2] = new Brick(0, 1, clr);
		bricks[3] = new Brick(0, 2, clr);
	}
	Figure* getRotatedFigure() {
		Point p = bricks[1]->getPosition();
		Figure* ans = Figure::createFigure(0, 0);
		for (int i = 0; i < 4; i++) {
			int tx, ty;
			tx = p.x - bricks[i]->getPosition().y + p.y;
			ty = p.y - bricks[i]->getPosition().x + p.x;
			ans->getBrick(i)->setPosition(tx, ty);
		}
		return ans;
	}
};
class figT : public Figure {
public:
	figT(int clr) : Figure()
	{
		bricks[0] = new Brick(1, 0, clr);
		bricks[1] = new Brick(1, 1, clr);
		bricks[2] = new Brick(1, 2, clr);
		bricks[3] = new Brick(0, 1, clr);
	}
	Figure* getRotatedFigure() {
		Figure* ans = Figure::createFigure(0, 0);
		bool** matr = new bool* [4];
		for (int i = 0; i < 4; i++)
			matr[i] = new bool[4]{ 0 };
		int minx = Width, miny = Height;
		for (int i = 0; i < 4; i++)
		{
			if (bricks[i]->getPosition().x < minx) minx = bricks[i]->getPosition().x;
			if (bricks[i]->getPosition().y < miny) miny = bricks[i]->getPosition().y;
		}
		for (int i = 0; i < 4; i++)
		{
			int x = bricks[i]->getPosition().x;
			int y = bricks[i]->getPosition().y;
			matr[x - minx][y - miny] = true;
		}
		int shiftX = minx;
		int shiftY = miny;
		matrRotate(matr, 4);
		int z = 0;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (matr[i][j])
				{
					ans->getBrick(z)->setPosition(shiftX + i, shiftY + j);
					z++;
				}
			}
		}
		return ans;
	}
};
class figL : public Figure {
public:
	figL(int clr) : Figure()
	{
		bricks[0] = new Brick(0, 0, clr);
		bricks[1] = new Brick(0, 1, clr);
		bricks[2] = new Brick(1, 1, clr);
		bricks[3] = new Brick(1, 2, clr);
	}
	Figure* getRotatedFigure() {
		Figure* ans = Figure::createFigure(0, 0);
		bool** matr = new bool* [4];
		for (int i = 0; i < 4; i++)
			matr[i] = new bool[4]{ 0 };
		int minx = Width, miny = Height;
		for (int i = 0; i < 4; i++)
		{
			if (bricks[i]->getPosition().x < minx) minx = bricks[i]->getPosition().x;
			if (bricks[i]->getPosition().y < miny) miny = bricks[i]->getPosition().y;
		}
		for (int i = 0; i < 4; i++)
		{
			int x = bricks[i]->getPosition().x;
			int y = bricks[i]->getPosition().y;
			matr[x - minx][y - miny] = true;
		}
		int shiftX = minx;
		int shiftY = miny;
		matrRotate(matr, 4);
		int z = 0;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (matr[i][j])
				{
					ans->getBrick(z)->setPosition(shiftX + i, shiftY + j);
					z++;
				}
			}
		}
		return ans;
	}
};
class figJ : public Figure {
public:
	figJ(int clr) : Figure()
	{
		bricks[0] = new Brick(1, 0, clr);
		bricks[1] = new Brick(1, 1, clr);
		bricks[2] = new Brick(1, 2, clr);
		bricks[3] = new Brick(0, 2, clr);
	}
	Figure* getRotatedFigure() {
		Figure* ans = Figure::createFigure(0, 0);
		bool** matr = new bool* [4];
		for (int i = 0; i < 4; i++)
			matr[i] = new bool[4]{ 0 };
		int minx = Width, miny = Height;
		for (int i = 0; i < 4; i++)
		{
			if (bricks[i]->getPosition().x < minx) minx = bricks[i]->getPosition().x;
			if (bricks[i]->getPosition().y < miny) miny = bricks[i]->getPosition().y;
		}
		for (int i = 0; i < 4; i++)
		{
			int x = bricks[i]->getPosition().x;
			int y = bricks[i]->getPosition().y;
			matr[x - minx][y - miny] = true;
		}
		int shiftX = minx;
		int shiftY = miny;
		matrRotate(matr, 4);
		int z = 0;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (matr[i][j])
				{
					ans->getBrick(z)->setPosition(shiftX + i, shiftY + j);
					z++;
				}
			}
		}
		return ans;
	}
};
class figO : public Figure {
public:
	figO(int clr) : Figure() {
		bricks[0] = new Brick(0, 0, clr);
		bricks[1] = new Brick(0, 1, clr);
		bricks[2] = new Brick(1, 1, clr);
		bricks[3] = new Brick(1, 0, clr);
		canRotate = false;
	}
	Figure* getRotatedFigure() { return nullptr; };
};

Figure* Figure::createFigure(int ID, int clr) {
	switch (ID) {
		case I: {
			Figure* t = new figI(clr);
			return t;
		}
		case Z: {
			Figure* t = new figZ(clr);
			return t;
		}
		case S: {
			Figure* t = new figS(clr);
			return t;
		}
		case T: {
			Figure* t = new figT(clr);
			return t;
		}
		case L: {
			Figure* t = new figL(clr);
			return t;
		}
		case J: {
			Figure* t = new figJ(clr);
			return t;
		}
		case O: {
			Figure* t = new figO(clr);
			return t;
		}
	}
}

class Game {
private:
	Sprite backgrnd;
	Sprite frm;
	Sprite nxtfrm;
	Text scr;
	Figure* dropFigure;
	Figure* nextFigure;
	Brick* bricks[Width][Height];
	int gameState = 0;
	int score = 0;
	float dropTime = 0.7;
	float time = 0;
public:
	Game() {
		backgrnd.setTexture(background);
		frm.setTexture(frame);
		nxtfrm.setTexture(nextframe);
		scr.setFillColor(Color::White);
		scr.setCharacterSize(18);
		scr.setFont(smw);
		scr.setPosition(39, 424);
		nxtfrm.move(231, 47);
		dropFigure = createRandomFigure();
		nextFigure = createRandomFigure();
	}
	void Tick(float dt) {
		if (gameState != 0) return;
		time += dt;
		if (time >= dropTime) {
			moveFigure(0, 1);
			time = 0;
		}
	}
	bool checkColision(int dx, int dy) {
		for (int i = 0; i < 4; i++) {
			int nx = dropFigure->getBrick(i)->getPosition().x + dx;
			int ny = dropFigure->getBrick(i)->getPosition().y + dy;
			if (ny >= Height || ny < 0 || nx >= Width || nx < 0) {
				return false;
			}
			for (int i = 0; i < Width; i++)
			{
				for (int j = 0; j < Height; j++)
				{
					if (bricks[i][j] == nullptr) continue;
					if (bricks[i][j]->getPosition().x == nx && bricks[i][j]->getPosition().y == ny)
					{
						return false;
					}
				}
			}
		}
		return true;
	}
	Figure* createRandomFigure() {
		return Figure::createFigure(rand() % FiguresNum, rand() % ColorsNum);
	}
	void moveFigure(int dx, int dy) {
		dropTime = 0.7;
		if (checkColision(dx, dy) == false) {
			if (dy > 0 && dx == 0) {
				for (int i = 0; i < 4; i++) {
					int x = dropFigure->getBrick(i)->getPosition().x;
					int y = dropFigure->getBrick(i)->getPosition().y;
					bricks[x][y] = new Brick(*dropFigure->getBrick(i));
				}
				for (int i = 0; i < Width; i++)
				{
					for (int j = 0; j < Height; j++)
					{
						if (bricks[i][j] == nullptr) continue;
						if (!nextFigure->getColision(i, j))
						{
							gameState = 2;
							return;
						}
					}
				}
				delete dropFigure;
				dropFigure = nextFigure;
				nextFigure = createRandomFigure();
			}
			else {
				return;
			}
		}
		else {
			checkLines();
			dropFigure->moveFigure(dx, dy);
		}
	}
	void checkLines(int combo = 0) {
		for (int i = 0; i < Height; i++) {
			bool isChecked = true;
			for (int j = 0; j < Width; j++) {
				if (bricks[j][i] == nullptr) isChecked = false;
			}
			if (isChecked) {
				combo++;
				for (int j = 0; j < Width; j++) {
					if (bricks[j][i] != nullptr) { delete bricks[j][i]; bricks[j][i] = nullptr; }
				}
				for (int z = i - 1; z >= 0; z--) {
					if (z < 0) continue;
					for (int j = 0; j < Width; j++) {
						if (bricks[j][z] == nullptr) continue;
						bricks[j][z]->setPosition(bricks[j][z]->getPosition().x, bricks[j][z]->getPosition().y + 1);
						bricks[j][z + 1] = bricks[j][z];
						bricks[j][z] = nullptr;
					}
				}
				switch (combo)
				{
					case 1:
					{
						score += 100;
						break;
					}
					case 2:
					{
						score += 200;
						break;
					}
					case 3:
					{
						score += 400;
						break;
					}
					case 4:
					{
						score += 800;
						break;
					}
				}
				checkLines(combo);
			}
		}
	}
	void Draw() {
		window.clear(Color::White);
		window.draw(backgrnd);
		dropFigure->Draw();
		nextFigure->Draw(true);
		for (int i = 0; i < Width; i++) {
			for (int j = 0; j < Height; j++) {
				if (bricks[i][j] != nullptr) bricks[i][j]->Draw();
			}
		}
		scr.setString("Score: " + to_string(score));
		window.draw(scr);
		window.draw(nxtfrm);
		window.draw(frm);
		window.display();
	}
	int getGameState() {
		return gameState;
	}
	void PauseGame()
	{
		if (gameState == 1) gameState = 0;
		else if (gameState == 0) gameState = 1;
	}
	void dropDown() {
		if (gameState != 0) return;
		dropTime = 0.03;
	}
	void FigRotate() {
		if (gameState != 0) return;
		for (int i = 0; i < Width; i++)
		{
			for (int j = 0; j < Height; j++) {
				if (bricks[i][j] == nullptr) continue;
				if (!dropFigure->getRotateCollision(bricks[i][j]->getPosition().x, bricks[i][j]->getPosition().y)) return;
			}
		}
		dropFigure->rotate();
	}
	void figMove(bool dir) {
		if (gameState != 0) return;
		if (!dir) {
			moveFigure(-1, 0);
		}
		else {
			moveFigure(1, 0);
		}
	}
	~Game()
	{
		for (int i = 0; i < Width; i++) {
			for (int j = 0; j < Height; j++)
			{
				if (bricks[i][j] != nullptr) { delete bricks[i][j]; }
			}
		}
		delete nextFigure;
	}
};

int main()
{
	srand(time(0));
	tile.loadFromFile("images/tiles.png");
	background.loadFromFile("images/background.png");
	frame.loadFromFile("images/frame.png");
	nextframe.loadFromFile("images/nextframe.png");
	smw.loadFromFile("fonts/smw.ttf");
	Game* game = new Game;
	Clock clock;
	window.setFramerateLimit(100);
	while (window.isOpen())
	{
		game->Tick(clock.getElapsedTime().asSeconds());
		clock.restart();

		Event e;
		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed)
				window.close();
			if (e.type == Event::KeyPressed)
				if (e.key.code == Keyboard::Left) game->figMove(false);
				else if (e.key.code == Keyboard::Right) game->figMove(true);
				else if (e.key.code == Keyboard::Up) game->FigRotate();
				else if (e.key.code == Keyboard::Escape)
				{
					if (game == nullptr) continue;
					if (game->getGameState() < 2) game->PauseGame();
				}
				else if (e.key.code == Keyboard::Space)
				{
					if (game == nullptr) continue;
					if (game->getGameState() == 2)
					{
						delete game;
						game = new Game;
					}
				}
		}
		if (Keyboard::isKeyPressed(Keyboard::Down)) game->dropDown();
		game->Draw();
	}
}