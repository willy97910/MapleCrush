#define _CRT_SECURE_NO_WARNINGS

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
using namespace sf;
using namespace std;

int ts = 54; //tile size
Vector2i offset(48, 24);
//遊戲結束音效(登出音效)、combo音效

struct piece
{
	int x, y, col, row, kind, match, alpha;
	piece()
	{
		match = 0;
		alpha = 255; //透明度
	}
} grid[10][10];//宣告個多出兩個

void swap(piece p1, piece p2)
{
	std::swap(p1.col, p2.col);
	std::swap(p1.row, p2.row);

	grid[p1.row][p1.col] = p1;
	grid[p2.row][p2.col] = p2;
}

int main()
{
	srand(time(0));

	RenderWindow app(VideoMode(740, 480), "Maple Crush"); //視窗顯示 + 大小控制
	app.setFramerateLimit(60); //落下速度

							   /////////////////////////////////////////////////////////////////////////

							   // Count-down timer  : Text box 
	sf::Font font;
	if (!font.loadFromFile("digital-7.ttf"))
	{
		return -1;
	}
	sf::Text text;
	text.setFont(font);
	text.setFillColor(sf::Color::Yellow);
	text.setCharacterSize(70);
	text.move(580, 6);


	ifstream file("score.txt");
	int historyscore;

	if (file)
		file >> historyscore;

	file.close();
	///////////////////////////////////////////////////////////////////////////////


	sf::Music music;
	sf::Music matched;
	sf::Music login;

	if (!music.openFromFile("toytown.ogg"))
		return -1; // error
	music.setVolume(50);
	music.setLoop(true);
	music.play();

	if (!matched.openFromFile("triumph.ogg"))
		return -1; // error
	matched.setVolume(50);
	matched.setLoop(false);
	float origin_pitch = matched.getPitch();

	if (!login.openFromFile("login.ogg"))
		return -1; // error
	login.setVolume(50);
	login.setLoop(true);

	/////////////////////////////////////////////////////////////////

	Texture t1, t2, c1;
	t1.loadFromFile("images/background.png");
	t2.loadFromFile("images/gems.png");
	c1.loadFromFile("images/clock1.png");

	Texture c2, c3, c4;

	c2.loadFromFile("images/clock2.png");
	c3.loadFromFile("images/clock3.png");
	c4.loadFromFile("images/clock4.png");

	/////////////////////////////////////////////////////////////////
	Sprite background(t1), gems(t2);

	//宣告珠珠群
	for (int i = 1; i <= 8; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			grid[i][j].kind = rand() % 6; //六種東西
			grid[i][j].col = j;
			grid[i][j].row = i;
			grid[i][j].x = j*ts;
			grid[i][j].y = i*ts;
		}
	}
	///////////////////////////////////////////////////
	//修正
	for (int i = 0; i < 8; i++)
	{
		grid[9][i].kind = 10 + i;
		grid[i][9].kind = 10 + i;
		grid[i][0].kind = 10 + i;
		grid[0][i].kind = 10 + i;
	}
	///////////////////////////////////////////////////
	int x0, y0, x, y;
	int click = 0;
	Vector2i pos;
	bool isSwap = false, isMoving = false;

	///////////////////////////////////////////////////////////////

	// Count-down Timer : Calculating time-1
	unsigned int x_seconds = 0;
	unsigned int x_milliseconds = 0;
	unsigned int total_time = 0, count_down = 0, time_left = 0;
	clock_t start_time, count_time;
	count_down = 60;
	start_time = clock(); // record beginning time
	time_left = count_down;

	//////////////////////////////////////////////////////////////
	//計分宣告
	//score & combos count
	int get_score = 0;
	char output_score[10];
	int move_time = 0;
	int new_combos = 0;

	//////////////////////////////////////////////////////////////
	while (app.isOpen() && time_left > 0)
	{

		// Count-down timer - 2
		count_time = clock();
		x_milliseconds = count_time - start_time;
		x_seconds = x_milliseconds / (CLOCKS_PER_SEC);
		time_left = count_down - x_seconds;

		std::string s;
		std::stringstream out;
		out << time_left;

		text.setString(out.str());

		/////////////////////////////////////////////////////////

		//取得滑鼠指令
		Event e;
		while (app.pollEvent(e))
		{
			//關掉程式 (按 X)
			if (e.type == Event::Closed)
				app.close();

			if (e.type == Event::MouseButtonPressed)
			{
				if (e.key.code == Mouse::Left)
				{
					if (!isSwap && !isMoving)
					{
						click++;
						pos = Mouse::getPosition(app) - offset;
						if (pos.x > 420 || pos.y > 420)
						{
							click = 0;
						}
					}
				}

			}
		}

		// mouse click
		if (click == 1)
		{
			x0 = pos.x / ts + 1;
			y0 = pos.y / ts + 1;
		}

		if (click == 2)
		{
			x = pos.x / ts + 1;
			y = pos.y / ts + 1;

			if (abs(x - x0) + abs(y - y0) == 1) //如果兩個是相鄰的
			{
				matched.setPitch(origin_pitch);
				matched.play();
				swap(grid[y0][x0], grid[y][x]);
				isSwap = 1; //將列入判斷的指標變為 true
				click = 0;
				move_time++;
			}
			else
				click = 1;
		}

		//Match finding
		for (int i = 1; i <= 8; i++)
			for (int j = 1; j <= 8; j++)
			{
				if (grid[i][j].kind == grid[i + 1][j].kind)
					if (grid[i][j].kind == grid[i - 1][j].kind)
						for (int n = -1; n <= 1; n++)
						{
							grid[i + n][j].match++;
							//green.play();
						}

				if (grid[i][j].kind == grid[i][j + 1].kind)
					if (grid[i][j].kind == grid[i][j - 1].kind)
						for (int n = -1; n <= 1; n++)
						{
							grid[i][j + n].match++;
							//green.play();
						}
			}

		//Moving animation
		isMoving = false;
		for (int i = 1; i <= 8; i++)
			for (int j = 1; j <= 8; j++)
			{
				piece &p = grid[i][j];
				int dx, dy;
				//可以控制速度
				for (int n = 0; n < 5; n++)   // 4 - speed
				{
					dx = p.x - p.col*ts;
					dy = p.y - p.row*ts;
					if (dx)
						p.x -= dx / abs(dx);
					if (dy)
						p.y -= dy / abs(dy);
				}
				if (dx || dy)
					isMoving = 1;
			}

		//Deleting amimation
		if (!isMoving)
		{
			for (int i = 1; i <= 8; i++)
			{
				for (int j = 1; j <= 8; j++)
				{
					if (grid[i][j].match)
					{
						if (grid[i][j].alpha > 10)
						{
							grid[i][j].alpha -= 10;
							isMoving = true;
						}

					}
				}
			}
		}


		//Get score
		int score = 0;
		for (int i = 1; i <= 8; i++)
		{
			for (int j = 1; j <= 8; j++)
			{
				score += grid[i][j].match;
			}
		}

		//Second swap if no match
		if (isSwap == true && isMoving == false)
		{
			if (!score)
			{
				swap(grid[y0][x0], grid[y][x]);
			}
			isSwap = false;
		}

		//Update grid
		if (isMoving == false)
		{
			//讓原來存在的往下掉
			for (int i = 8; i > 0; i--)
			{
				for (int j = 1; j <= 8; j++)
				{
					if (grid[i][j].match)
					{
						for (int n = i; n > 0; n--)
						{
							if (!grid[n][j].match)
							{
								swap(grid[n][j], grid[i][j]);
								break;
							}
						}
					}
				}
			}
			//
			int tem_cnt = 0;
			//產生新的補空格
			for (int j = 1; j <= 8; j++)
			{
				for (int i = 8, n = 0; i > 0; i--)
				{
					if (grid[i][j].match)
					{
						grid[i][j].kind = rand() % 6;
						grid[i][j].y = -ts*n++;
						grid[i][j].match = 0;
						grid[i][j].alpha = 255;
						tem_cnt++;
					}
				}
			}

			if (tem_cnt != 0 && move_time != 0)
			{
				new_combos++;
				matched.setPitch(origin_pitch);
				//matched.play();
				if (new_combos >= 2)
				{
					matched.setPitch(origin_pitch + 1);
					matched.play();
				}
				get_score = get_score + tem_cnt*new_combos;
			}

			else if (tem_cnt == 0 && move_time != 0)
				new_combos = 0;

		}
		////////////////////////////////////////////////////////////////////

		//計分板

		Text record_board;
		sprintf(output_score, "%d", get_score);
		record_board.setString(output_score);
		sf::Font font;
		if (!font.loadFromFile("digital-7.ttf"))
		{
			return -1;
		}
		record_board.setString(output_score);
		record_board.setFont(font);
		record_board.setCharacterSize(60);
		record_board.setPosition(640, 410);


		////////////////////////////////////////////////////////////////////

		//////draw///////
		app.draw(background);//圖片輸出
		app.draw(text);
		//GEM是方塊群
		for (int i = 1; i <= 8; i++) //8是高
			for (int j = 1; j <= 8; j++) //8是寬
			{

				piece p = grid[i][j];
				gems.setTextureRect(IntRect(p.kind * 49, 0, 49, 49)); //切割圖片 製造磚塊 數字表示切割方式
				gems.setColor(Color(255, 255, 255, p.alpha)); //圖案顏色設定 透明度設定
				gems.setPosition(p.x, p.y); //設定位置
				gems.move(offset.x - ts, offset.y - ts); //位置校正
				app.draw(gems);
			}

		//////////////////////////////////////////////////////////
		// time box

		sf::RectangleShape time_rec;
		time_rec.setSize(sf::Vector2f(140, 60));
		time_rec.setFillColor(Color(0, 0, 0, 180));
		time_rec.setPosition(570, 23);
		app.draw(time_rec);

		sf::Sprite timer;

		if (time_left >= 45)
			timer.setTexture(c1);
		else if (time_left >= 30)
			timer.setTexture(c2);
		else if (time_left >= 15)
			timer.setTexture(c3);
		else
			timer.setTexture(c4);

		timer.setTextureRect(IntRect(0, 0, 80, 80));
		timer.setPosition(485, 10);

		sf::Text sec;
		sec.setFont(font);
		sec.setFillColor(sf::Color::White);
		sec.setString("SEC");
		sec.setCharacterSize(30);
		sec.move(660, 45);
		app.draw(sec);

		app.draw(timer);
		app.draw(text);

		// highest score
		sf::RectangleShape History_rec;
		History_rec.setSize(sf::Vector2f(250, 60));
		History_rec.setFillColor(Color(0, 0, 0, 180));
		History_rec.setPosition(480, 423);
		app.draw(History_rec);

		sf::Text HighScore_title;
		HighScore_title.setFont(font);
		HighScore_title.setFillColor(sf::Color::White);
		HighScore_title.setString("Highest Score");
		HighScore_title.setCharacterSize(20);
		HighScore_title.move(485, 422);
		app.draw(HighScore_title);

		std::string h_s;
		std::stringstream h_s_out;
		h_s_out << historyscore;

		sf::Text HighScore;
		HighScore.setFont(font);
		HighScore.setFillColor(sf::Color::White);
		HighScore.setString(h_s_out.str());
		HighScore.setCharacterSize(30);
		HighScore.move(560, 440);
		///////////////////////////////////////////////////////


		app.draw(HighScore);
		app.draw(record_board); //記分板顯示
		app.display(); //顯示
	}
	app.clear();


	music.pause();
	login.play();

	//ifstream file("score.txt");
	int new_score = get_score; //to be corrected
	int score;

	if (new_score > historyscore)
	{
		ofstream file2;
		file2.open("score.txt");
		file2 << new_score;
		file2.close();
	}

	while (app.isOpen())
	{
		// game-over page

		sf::RectangleShape color_rec;
		color_rec.setSize(sf::Vector2f(740, 480));
		color_rec.setFillColor(Color(0, 0, 0, 180));
		color_rec.setPosition(0, 0);

		sf::Text gameover;
		gameover.setFont(font);
		//gameover.setFillColor(sf::Color::White);
		gameover.setString("Game Over");
		gameover.setCharacterSize(100);
		gameover.move(190, 140);

		sf::Text YourScore;
		YourScore.setFont(font);
		//YourScore.setFillColor(sf::Color::White);
		YourScore.setString("Your score:");
		YourScore.setCharacterSize(30);
		YourScore.move(260, 280);

		std::string s;
		std::stringstream out;
		out << get_score;

		sf::Text Score;
		Score.setFont(font);
		Score.setString(out.str());
		Score.setCharacterSize(30);
		Score.move(440, 280);

		Event e;
		while (app.pollEvent(e))
		{
			if (e.type == Event::Closed)
				app.close();

		}

		bool exit = false;
		app.draw(background);
		app.draw(color_rec);
		app.draw(gameover);
		app.draw(YourScore);
		app.draw(Score);
		app.display();
	}

	return 0;
}