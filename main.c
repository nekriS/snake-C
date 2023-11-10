#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <malloc.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#define mapHeight 25
#define mapWidth 100
#define START_FLOW 1
#define START_LEN_SNAKE 5
#define START_SPEED 50000.0
#define DEATH_DELAY 50000.0
#define SPEED_SCALE_X 1.0
#define SPEED_SCALE_Y 1.6
#define MAX_COIN 30

static char map[mapHeight][mapWidth + 1];
static int flow = START_FLOW;
static int len = START_LEN_SNAKE;
static float delay = START_SPEED;

struct object {
	int x, y;
};

void* myThread(void* arg);
void clearWindow();
void PaintMap(int x, int y, int one_delay);
void setCur(int x, int y);
void init();
void gameOver();
bool isColisions(struct object obj1, struct object obj2);

void* myThread(void* arg) {
	int threadNumber = *((int*)arg);

	
	do {
		if ((GetKeyState(VK_LEFT) < 0) && (flow != 3)) flow = 1;
		if ((GetKeyState(VK_UP) < 0) && (flow != 4)) flow = 2;
		if ((GetKeyState(VK_RIGHT) < 0) && (flow != 1)) flow = 3;
		if ((GetKeyState(VK_DOWN) < 0) && (flow != 2)) flow = 4;
		
	} while (GetKeyState(VK_ESCAPE) >= 0);
	

	pthread_exit(NULL);

}









void clearWindow() {

	for (int i = 0; i < mapWidth; i++) {
		map[0][i] = '#';
	}
	for (int i = 0; i < mapWidth; i++) {
		map[1][i] = ' ';
	}
	map[1][0] = '#';
	map[1][mapWidth - 1] = '#';
	map[1][mapWidth] = '\0';
	for (int j = 1; j < mapHeight - 1; j++) {
		sprintf(map[j], map[1]);
	}
	sprintf(map[mapHeight - 1], map[0]);

}

void PaintMap(int x, int y, int one_delay) {

	setCur(0, 0);
	printf(" %s\n", map[0]);
	printf(" #  SCORE: %.5d  #  FLOW: %d  #  X: %3.d  Y: %3.d  # %50s\n", len - START_LEN_SNAKE, flow, y, x, "#");

	for (int i = 0; i < mapHeight; i++) {
		printf(" %s\n", map[i]);
	}

	float k = SPEED_SCALE_X;
	if ((flow == 2) || (flow == 4)) {
		k = SPEED_SCALE_Y;
	}

	printf(" Kirillov Lev E4160 : ESC - to exit\n");

	usleep(k * one_delay);

}


void setCur(int x, int y) {

	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

}

void init() {

	len = START_LEN_SNAKE;
	delay = START_SPEED;

}

void gameOver() {

	clearWindow();	
	do {
		strcpy(map[12], "#    					      GAMEOVER");
		strcpy(map[14], "#				      press SPACE to continue");
		PaintMap(0, 0, DEATH_DELAY);
	} while (GetKeyState(VK_SPACE) >= 0);
	init();
	
}


bool isColisions(struct object obj1, struct object obj2) {
	if ((obj1.x == obj2.x) && (obj1.y == obj2.y)) {
		return true;
	}
	return false;
}

int main() {


	
	//system("cls");

	pthread_t thread;
	int threadNumber = 1;
	int result;
	result = pthread_create(&thread, NULL, myThread, &threadNumber);

	

	struct object *elements = (struct object*)malloc((MAX_COIN + START_LEN_SNAKE + 1)  * sizeof(struct object));

	struct object coin;

	init(&coin);


	coin.x = 10;
	coin.y = 10;

	elements[0].x = 5;
	elements[0].y = 10;

	elements[1].x = 5;
	elements[1].y = 11;

	elements[2].x = 5;
	elements[2].y = 12;

	do {

		clearWindow();

		// Проверка на получения монет

		if (isColisions(elements[0], coin)) {
			if (len < (MAX_COIN + START_LEN_SNAKE)) len += 1;
			coin.x = rand()%20 + 2;
			coin.y = rand()%95 + 2;
			delay *= 0.99;				
		}

		// Проверка на смерть

		for (int i = 1; i < len; i++) {
			if (isColisions(elements[0], elements[i])) {
				gameOver();
			}
		}
		
		// Движение змейки

		for (int i = len - 1; i > 0; i--) {
			elements[i].y = elements[i - 1].y;
			elements[i].x = elements[i - 1].x;
		}

		if (flow == 1) {
			if (elements[0].y > 1) {
				elements[0].y -= 1;
			} else {
				elements[0].y = mapWidth - 2;
			}
		}

		if (flow == 4) {
			if (elements[0].x < mapHeight - 2) {
				elements[0].x += 1;
			} else {
				elements[0].x = 1;
			}
		}

		if (flow == 3) {
			if (elements[0].y < mapWidth - 2) {
				elements[0].y += 1;
			} else {
				elements[0].y = 1;
			}

		}

		if (flow == 2) {
			if (elements[0].x > 1) {
				elements[0].x -= 1;
			} else {
				elements[0].x = mapHeight - 2;
			}
		}

		// Читы на очки

		if ((GetKeyState(VK_LSHIFT) < 0) && (len < (MAX_COIN + START_LEN_SNAKE))) len += 1;

		// Отправка данных на карту

		for (int i = 0; i < len; i++) {
			//printf("%d %d\n", elements[i].x, elements[i].y);
			map[elements[i].x][elements[i].y] = 'o';
		}

		map[coin.x][coin.y] = 'c';

		// Отрисовка карты

		PaintMap(elements[0].x, elements[0].y, delay);


	} while (GetKeyState(VK_ESCAPE) >= 0);

	pthread_join(thread, NULL);

	return 0;
}