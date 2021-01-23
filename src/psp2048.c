/*
 * 2048.c : 2048-PSP Clone
 *
 * Licensed under the GPLv2 license 
 *
 * clone & port by Anderain Ryu <anderain.develop@gmail.com>
 *
 */
 
#include <pspkernel.h>
#include <pspctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "graphics.h"
#include "callbacks.h"
#include "input.h"
#include <string.h>

#define VERS 1
#define REVS 0

PSP_MODULE_INFO("2048", PSP_MODULE_USER, VERS, REVS);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER); 
PSP_HEAP_SIZE_MAX();
/*
 */
#define ABS(x) ((x) < 0 ? -(x) : (x))
 
 #define blit blitAlphaImageToScreen
 
#define TABLE_WIDTH 	4
#define TABLE_HEIGHT 	4

#define SCORE_DRAW_X_OFFSET 48
#define SCORE_DRAW_Y_OFFSET 44
#define HISCORE_DRAW_X_OFFSET SCORE_DRAW_X_OFFSET
#define HISCORE_DRAW_Y_OFFSET 144

#define NUM_CHAR_WIDTH  16
#define NUM_CHAR_HEIGHT 22

#define NUM_SQR_WIDTH	54
#define NUM_SQR_HEIGHT	54

#define STR_BUFFER_SIZE	64

#define NUM_DRAW_X_OFFSET 226
#define NUM_DRAW_Y_OFFSET 22
#define NUM_DRAW_SPACING 5

#define NUMBER_IMAGE_MAX 11

#define NUM_DRAW_X_POS(x)	(NUM_DRAW_X_OFFSET + (NUM_DRAW_SPACING + NUM_SQR_WIDTH) * (x))
#define NUM_DRAW_Y_POS(y)	(NUM_DRAW_Y_OFFSET + (NUM_DRAW_SPACING + NUM_SQR_HEIGHT) * (y))

#define NEW_POINT_ANI_SIZE 4

Image * imgBack;
Image * imgNum[NUMBER_IMAGE_MAX];
Image * imgNumTable;
Image * imgNewPointAni[NEW_POINT_ANI_SIZE];

int table[TABLE_HEIGHT][TABLE_WIDTH] = {
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0}
};

int iScore,iHighScore;

const char * NUMBER_IMAGE_TABLE[] = {
	"0002.png","0004.png","0008.png","0016.png",
	"0032.png","0064.png","0128.png","0256.png",
	"0512.png","1024.png","2048.png",NULL
};
/*!
 * @brief draw a number to screen
 * @param iNum number to draw
 * @param x x pos
 * @param y y pos
 */
void drawNumber(int iNum,int x,int y) {
	char szBuf[STR_BUFFER_SIZE],*p;
	if (iNum < 0)
		return;
	sprintf(p = szBuf,"%d",iNum);
	while (*p) {
		blit((*p - '0') * NUM_CHAR_WIDTH,0,NUM_CHAR_WIDTH,NUM_CHAR_HEIGHT,imgNumTable,x,y);
		x += NUM_CHAR_WIDTH;
		p++;
	}
}
/*!
 * @brief load all images
 */
void loadImages () {
	int y;
	for (y=0;NUMBER_IMAGE_TABLE[y] != NULL;++y) {
		char szFilePath[STR_BUFFER_SIZE];
		sprintf(szFilePath,"graphics/%s",NUMBER_IMAGE_TABLE[y]);
		imgNum[y] = loadImage(szFilePath);
	}
	for (y=0;y < NEW_POINT_ANI_SIZE;++y) {
		char szFilePath[STR_BUFFER_SIZE];
		sprintf(szFilePath,"graphics/b%d.png",y);
		imgNewPointAni[y] = loadImage(szFilePath);
	}
	imgBack = loadImage("graphics/bg.png");
	imgNumTable = loadImage("graphics/numtable.png");
}
/*!
 * @brief redraw screen content
 * @param ix ingore square x pos 
 * @param iy ingore square y pos
 */
void redrawScreen (int ix,int iy) {
	int y,x;
	
	blit(0,0,imgBack->imageWidth,imgBack->imageHeight,imgBack,0,0);
	drawNumber(iScore,SCORE_DRAW_X_OFFSET,SCORE_DRAW_Y_OFFSET);
	drawNumber(iHighScore,HISCORE_DRAW_X_OFFSET,HISCORE_DRAW_Y_OFFSET);
	
	for (y=0;y<TABLE_HEIGHT;++y) {
		for (x=0;x<TABLE_WIDTH;++x) {
			int index = 0,t,rx,ry;
			if (x==ix && y==iy)
				continue;
			t = table[y][x];
			if (t == 0)
				continue;
			while (t > 0) {
				t >>= 1;
				index ++;
			}
			index--,index--;
			rx = NUM_DRAW_X_POS(x); 
			ry = NUM_DRAW_Y_POS(y);
			blit(0,0,NUM_SQR_WIDTH,NUM_SQR_HEIGHT,imgNum[index],rx,ry);
		}
	}
	
}
/*!
 * @brief refresh screen 
 */
void refreshScreen () {
	clearScreen(WHITE);
	redrawScreen(-1,-1);
	flipScreen();
}
/*!
 * @brief check table is full
 * @return 1 if table is full
 */
int tableFull () {
	int y,x,f = 0;
	for (y=0;y<TABLE_HEIGHT;++y)
		for (x=0;x<TABLE_WIDTH;++x)
			if (table[y][x] == 0)
				f++;
	return f <= 0;
}
/*!
 * @biref play a animation new point
 * @param ox  x pos 
 * @param oy  y pos
 */
void playNewPoint(int ox,int oy) {
	int y;
	
	int rx = NUM_DRAW_X_POS(ox);
	int ry = NUM_DRAW_Y_POS(oy);
	
	for (y=0;y<NEW_POINT_ANI_SIZE;++y) {
		clearScreen(WHITE);
		redrawScreen(-1,-1);
		blit(0,0,NUM_SQR_WIDTH,NUM_SQR_HEIGHT,imgNewPointAni[y],rx,ry);
		flipScreen();
		//sleep(1);
	}
}
/*!
 * @brief set a new numer '2' at random
 * @return 1 if table is full
 */
int newPoint () {
	int r;
	if (tableFull())
		return 1;
	r = (rand() % 100 < 10) ? 4 : 2;
	while (1) {
		int x,y;
		x = rand() % TABLE_WIDTH;
		y = rand() % TABLE_HEIGHT;
		if (table[y][x] == 0) {
			table[y][x] = r;
			playNewPoint(x,y);
			break;
		}
	}
	return 0;
}
/*!
 * @biref add score
 */
void addScore (int a) {
	iScore += a;
	if (iHighScore < iScore)
		iHighScore = iScore;
}
/*!
 * @biref play a animation of move square
 * @param ox original x
 * @param oy original y
 * @param dx dest x
 * @param dy dest y
 */
void playMove(int ox,int oy,int dx,int dy) {
	int frames;
	int y,rx,ry,stepX,stepY,t,index;
	
	if (ABS(ox-dy)>=3 || (oy-dy)>=3) 		frames = 15;
	else if (ABS(ox-dy)>=2 || (oy-dy)>=2) 	frames = 10;
	else frames = 5;
	
	rx = NUM_DRAW_X_POS(ox);
	ry = NUM_DRAW_Y_POS(oy);
	stepX = (NUM_DRAW_X_POS(dx) - rx) / frames;
	stepY = (NUM_DRAW_Y_POS(dy) - ry) / frames;
	t = table[oy][ox];
	index = 0;
	
	if (t == 0)
		return;
	while (t > 0) {
		t >>= 1;
		index ++;
	}
	index--,index--;
	for (y=0;y<frames;++y,rx+=stepX,ry+=stepY) {
		clearScreen(WHITE);
		redrawScreen(ox,oy);
		blit(0,0,NUM_SQR_WIDTH,NUM_SQR_HEIGHT,imgNum[index],rx,ry);
		flipScreen();
	}
}
/*!
 * @brief move left
 */
void moveLeft () {
	int y,x,k;
	for (y=0;y<TABLE_HEIGHT;++y) {
		for (x=1;x<TABLE_WIDTH;++x) {
			int done = 0;
			if (table[y][x] == 0)
				continue;
			for (k=x-1;k>=0;--k) {
				if (table[y][k] == 0)
					continue;
				if (table[y][k] == table[y][x]) {
					playMove(x,y,k,y);
					addScore(table[y][k] += table[y][x]);
					table[y][x] = 0;
					done = 1;
					break;
				}
				else {
					break;
				}
			}
			if (!done) {
				k++;
				if (k != x) {
					playMove(x,y,k,y);
					table[y][k] = table[y][x];
					table[y][x] = 0;
				}
			}
			
		}
	}
}
/*!
 * @brief move up
 */
void moveUp () {
	int y,x,k;
	for (x=0;x<TABLE_WIDTH;++x) {
		for (y=1;y<TABLE_HEIGHT;++y) {
			int done = 0;
			if (table[y][x] == 0)
				continue;
			for (k=y-1;k>=0;--k) {
				if (table[k][x] == 0)
					continue;
				if (table[k][x] == table[y][x]) {
					playMove(x,y,x,k);
					addScore(table[k][x] += table[y][x]);
					table[y][x] = 0;
					done = 1;
					break;
				}
				else {
					break;
				}
			}
			if (!done) {
				k++;
				if (k != y) {
					playMove(x,y,x,k);
					table[k][x] = table[y][x];
					table[y][x] = 0;
					done = 1;
				}
			}
		}
	}
}
/*!
 * @brief move down
 */
void moveDown () {
	int y,x,k;
	for (x=0;x<TABLE_WIDTH;++x) {
		for (y=TABLE_HEIGHT-2;y>=0;--y) {
			int done = 0;
			if (table[y][x] == 0)
				continue;
			for (k=y+1;k<TABLE_HEIGHT;++k) {
				if (table[k][x] == 0)
					continue;
				if (table[k][x] == table[y][x]) {
					playMove(x,y,x,k);
					addScore(table[k][x] += table[y][x]);
					table[y][x] = 0;
					done = 1;
					break;
				}
				else {
					break;
				}
			}
			if (!done) {
				k--;
				if (k != y) {
					playMove(x,y,x,k);
					table[k][x] = table[y][x];
					table[y][x] = 0;
				}
			}
			
		}
	}
}
/*!
 * @brief move right
 */
void moveRight () {
	int y,x,k;
	for (y=0;y<TABLE_HEIGHT;++y) {
		for (x=TABLE_WIDTH-2;x>=0;--x) {
			int done = 0;
			if (table[y][x] == 0)
				continue;
			for (k=x+1;k<TABLE_WIDTH;++k) {
				if (table[y][k] == 0)
					continue;
				if (table[y][k] == table[y][x]) {
					playMove(x,y,k,y);
					addScore(table[y][k] += table[y][x]);
					table[y][x] = 0;
					done = 1;
					break;
				}
				else {
					break;
				}
			}
			if (!done) {
				k--;
				if (k != x) {
					playMove(x,y,k,y);
					table[y][k] = table[y][x];
					table[y][x] = 0;
				}
			}
			
		}
	}
}
/*!
 * @biref entry point of game
 */
int main() {
	int refresh  = 1;
	setupCallbacks();

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	initGraphics();
	loadImages();
	
	srand(time(NULL));
	
	iScore = 0;
	iHighScore = 0;
	
	newPoint();
	
	while(running()) {
		if (refresh) {
			refresh = 0;
			refreshScreen();
		}
		updateInput();
		if(keyTrigger(PSP_CTRL_UP)) {
			moveUp();
			newPoint();
			refresh = 1;
		}
		if(keyTrigger(PSP_CTRL_DOWN)) {
			moveDown();
			newPoint();
			refresh = 1;
		}
		if(keyTrigger(PSP_CTRL_LEFT)) {
			moveLeft();
			newPoint();
			refresh = 1;
		}
		if(keyTrigger(PSP_CTRL_RIGHT)) {
			moveRight();
			newPoint();
			refresh = 1;
		}
		if (keyTrigger(PSP_CTRL_START)) {
			break;
		}
		if (keyTrigger(PSP_CTRL_SELECT)) {
			iScore = 0;
			memset(table,0,sizeof(table));
			newPoint();
			refresh = 1;
		}
	}
	
	sceKernelExitGame();
	return 0;
}
