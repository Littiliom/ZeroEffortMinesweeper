#include <SFML/Graphics.hpp>
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <vector>


class Cell
{
public:
	bool bombed;
	bool flagged;
	bool revealed;
	int around;
	Cell() {
		bombed = false;
		flagged = false;
		revealed = false;
		around = 0;
	}
};


const unsigned int bombCount = 10;
int revealed = 0;
std::vector<sf::Vector2i> mines;
Cell boardInfo[20][20];
void updateBoardInfo();

void bombTheField();
void zeroOutBoard();

void reveal(int x, int y);

int main()
{
	sf::RenderWindow window(sf::VideoMode(400, 400), "SFML works!");
	sf::CircleShape aroundOutput[8];
	for (int i = 0; i < 8; i++)
	{
		aroundOutput[i].setRadius(10);
		aroundOutput[i].setFillColor(sf::Color(69, 13, 37));
		aroundOutput[i].setPosition(10 + 25 * i , 10);
	}
	int isOutputingAround = 0;
	bool isMiddleAlreadyPressed = false;


	sf::RectangleShape wonShape(sf::Vector2f(200, 200));
	wonShape.setPosition(100, 100);
	wonShape.setFillColor(sf::Color(100, 200, 100, 160));
	sf::RectangleShape lostShape(sf::Vector2f(300, 300));
	lostShape.setPosition(50, 50);
	lostShape.setFillColor(sf::Color(200, 100, 100, 160));
	sf::RectangleShape board[20][20];
	sf::Vector2u winSize = window.getSize();
	bool isPlayerWon = true;
	bool playerLost = false;
	for (int i = 0; i < 20; i++)
		for (int j = 0; j < 20; j++)
			board[i][j].setPosition(i * 20, j * 20);


	srand(static_cast<int>(time(0)));
	bombTheField();

	updateBoardInfo();

	while (window.isOpen())
	{
		isPlayerWon = (revealed + bombCount) == 400;
	//	std::cout << revealed << ", " << bombCount << " = " << revealed + bombCount << std::endl;

		sf::Vector2i mouse = sf::Mouse::getPosition(window);
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		for (int i = 0; i < 400; i++)
		{
			const Cell const *cell = &boardInfo[i / 20][i % 20];
			sf::RectangleShape *shape = &board[i / 20][i % 20];
			shape->setSize(sf::Vector2f(20, 20));

			shape->setFillColor(sf::Color(32, 100, 64));
			if (cell->bombed)
				shape->setFillColor(sf::Color::Magenta);
			if (cell->flagged)
				shape->setFillColor(sf::Color::Cyan);
			if (cell->revealed)
			{
				char c = 40 + 25 * cell->around;
				shape->setFillColor(sf::Color(c, c, c));
			}
		}

		if (mouse.x > 0 && mouse.y > 0 && mouse.y < winSize.y && mouse.x < winSize.x)
		{
			
			if (isPlayerWon)
			{
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
				{
					zeroOutBoard();
					bombTheField();
					updateBoardInfo();
					revealed = 0;
					isPlayerWon = 0;
				}
			}
			else if(playerLost)
			{
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
				{
					zeroOutBoard();
					bombTheField();
					updateBoardInfo();
					revealed = 0;
					playerLost = 0;
				}
			}
			else
			{
				sf::Vector2i currentPos(mouse.x / 20, mouse.y / 20);
				Cell *info = &boardInfo[currentPos.x][currentPos.y];

				board[currentPos.x][currentPos.y].setFillColor(sf::Color::Cyan);
				//std::cout << info->around << std::endl;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::B) && !(info->bombed))
				{
					info->bombed = true;
					mines.push_back(currentPos);
					updateBoardInfo();
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && info->bombed) {
					info->bombed = false;
					updateBoardInfo();
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Middle) && !isMiddleAlreadyPressed)
				{
					info->flagged = !info->flagged;
					isMiddleAlreadyPressed = true;
				}
				if (isMiddleAlreadyPressed
					&& !sf::Mouse::isButtonPressed(sf::Mouse::Middle))
					isMiddleAlreadyPressed = false;

				if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
				{
					if (info->revealed)
						isOutputingAround = info->around;
				}

				if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !info->flagged)
				{
					if(info->bombed)
						playerLost = true;
					reveal(mouse.x / 20, mouse.y / 20);
				}

			}
		}

		window.clear();

		for (int i = 0; i < 20; i++)
			for (int j = 0; j < 20; j++)
				window.draw(board[i][j]);
		if (isPlayerWon)
			window.draw(wonShape);
		if (playerLost)
			window.draw(lostShape);
		if (isOutputingAround)
			for (int i = 0; i < isOutputingAround; i++)
				window.draw(aroundOutput[i]);

		window.display();
	


		isOutputingAround = false;
}

	return 0;
}
//void bombTheField()
void zeroOutBoard()
{
	for (int i = 0; i < 400; i++)
	{
		boardInfo[i % 20][i / 20].around = 0;
		boardInfo[i % 20][i / 20].revealed = 0;
		boardInfo[i % 20][i / 20].flagged = 0;
		boardInfo[i % 20][i / 20].bombed = 0;
	}

}

void bombTheField()
{
	mines.clear();
	for (int i = 0; i < bombCount;)
	{
		sf::Vector2<int> mineCoords;
		mineCoords.x = rand() % 20;
		mineCoords.y = rand() % 20;
		if (boardInfo[mineCoords.x][mineCoords.y].bombed)
			continue;
		boardInfo[mineCoords.x][mineCoords.y].bombed = true;
		mines.push_back(mineCoords);
		i++;
	}
}

void updateBoardInfo()
{
	for (size_t i = 0; i < 400; i++)
	{
		boardInfo[i % 20][i / 20].around = 0;
	}

	if (boardInfo[0][0].bombed)
	{
		boardInfo[1][0].around++;
		boardInfo[1][1].around++;
		boardInfo[0][1].around++;
	}

	if (boardInfo[0][19].bombed)
	{
		boardInfo[0][18].around++;
		boardInfo[1][19].around++;
		boardInfo[1][18].around++;
	}
	if (boardInfo[19][0].bombed)
	{
		boardInfo[19][1].around++;
		boardInfo[18][1].around++;
		boardInfo[18][0].around++;
	}
	if (boardInfo[19][19].bombed)
	{
		boardInfo[18][18].around++;
		boardInfo[18][19].around++;
		boardInfo[19][18].around++;
	}
	for(size_t i = 1; i < 19; i++)
		for (size_t j = 1; j < 19; j++)
		{
			if (boardInfo[i][j].bombed)
			{
				std::cout << "found a bomb!\n";
				for (int m = -1; m < 2; m++)
					for (int n = -1; n < 2; n++)
						boardInfo[i + m][j + n].around++;
			}
		}


	for (int i = 1; i < 19; i++)
	{
		if (boardInfo[i][0].bombed) {
			for (int j = -1; j < 2; j++)
			{
				boardInfo[i + j][0].around++;
				boardInfo[i + j][1].around++;
			}
		}
		if (boardInfo[i][19].bombed) {
			for (int j = -1; j < 2; j++) {
				boardInfo[i + j][19].around++;
				boardInfo[i + j][18].around++;
			}
		}
		if (boardInfo[0][i].bombed) {
			for (int j = -1; j < 2; j++) {
				boardInfo[0][i + j].around++;
				boardInfo[1][i + j].around++;
			}
		}
		if (boardInfo[19][i].bombed) 
		{
			for (int j = -1; j < 2; j++) 
			{
				boardInfo[19][i + j].around++;
				boardInfo[18][i + j].around++;
			}
		}
	}
	for (int i = 0; i < mines.size(); i++)
	{
		boardInfo[mines[i].x][mines[i].y].around = 0;
	}
}

void revealB(int x, int y)
{
	if (boardInfo[x][y].around > 0)
	{
		if (!boardInfo[x][y].revealed)
			revealed++;
		boardInfo[x][y].revealed = true;
		return;
	}
	if (boardInfo[x][y].bombed || boardInfo[x][y].flagged || boardInfo[x][y].revealed)
		return;
	if (!boardInfo[x][y].revealed)
		revealed++;
	boardInfo[x][y].revealed = true;
	for (int i = x - 1; i < x + 2; i++)
	{
		if (i < 0 || i > 19)
			continue;
		for (int j = y - 1; j < y + 2; j++)
		{
			if (j < 0 || j > 19)
				continue;
			if (boardInfo[i][j].around == 0 && !boardInfo[i][j].revealed)
				reveal(i, j);
			if (!boardInfo[i][j].flagged && !boardInfo[i][j].bombed)
			{
				if (!boardInfo[i][j].revealed)
					revealed++;
				boardInfo[i][j].revealed = true;
			}
		}
	}
}



void reveal(int x, int y)
{
	if (boardInfo[x][y].bombed || boardInfo[x][y].flagged || boardInfo[x][y].revealed)
		return;
	if (boardInfo[x][y].around > 0)
	{
		revealed += (!boardInfo[x][y].revealed);
		boardInfo[x][y].revealed = true;
		return;
	}
	revealed += (!boardInfo[x][y].revealed);
	boardInfo[x][y].revealed = true;

	if (x > 0)
		reveal(x - 1, y);
	if (y > 0)
		reveal(x, y - 1);
	if (x < 19)
		reveal(x + 1, y);
	if (y < 19)
		reveal(x, y + 1);
}
