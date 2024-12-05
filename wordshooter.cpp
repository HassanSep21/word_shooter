//============================================================================
// Name        : cookie-crush.cpp
// Author      : Sibt ul Hussain
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game of Cookie  Crush...
//============================================================================
#ifndef WORD_SHOOTER_CPP
#define WORD_SHOOTER_CPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

//#include <GL/gl.h>
//#include <GL/glut.h>
#include <iostream>
#include<string>
#include<cmath>
#include<fstream>
#include "util.h"
using namespace std;
#define MAX(A,B) ((A) > (B) ? (A):(B)) // defining single line functions....
#define MIN(A,B) ((A) < (B) ? (A):(B))
#define ABS(A) ((A) < (0) ? -(A):(A))
#define FPS 60

// int FPS = 60;

Mix_Music* bgMusic = nullptr;
Mix_Chunk* popSound = nullptr;

string * dictionary;
int dictionarysize = 369646;
#define KEY_ESC 27 // A

// 20,30,30
const int bradius = 30; // ball radius in pixels...

int width = 930, height = 660;
int byoffset = bradius;

int nxcells = (width - bradius) / (2 * bradius);
int nycells = (height - byoffset /*- bradius*/) / (2 * bradius);
int nfrows = 2; // initially number of full rows //
float score = 0;

int **board = new int *[10]; // 2D-arrays for holding the data...

int bwidth = 130;
int bheight = 10;
int bsx, bsy;
const int nalphabets = 26;
enum alphabets {
	AL_A, AL_B, AL_C, AL_D, AL_E, AL_F, AL_G, AL_H, AL_I, AL_J, AL_K, AL_L, AL_M, AL_N, AL_O, AL_P, AL_Q, AL_R, AL_S, AL_T, AL_U, AL_W, AL_X, AL_y, AL_Z
};
GLuint texture[nalphabets];
GLuint tid[nalphabets];
string tnames[] = { "a.bmp", "b.bmp", "c.bmp", "d.bmp", "e.bmp", "f.bmp", "g.bmp", "h.bmp", "i.bmp", "j.bmp",
"k.bmp", "l.bmp", "m.bmp", "n.bmp", "o.bmp", "p.bmp", "q.bmp", "r.bmp", "s.bmp", "t.bmp", "u.bmp", "v.bmp", "w.bmp",
"x.bmp", "y.bmp", "z.bmp" };
GLuint mtid[nalphabets];
int awidth = 60, aheight = 60; // 60x60 pixels cookies...


// MY VARIABLES
float sec = 150;

int shooterBall;
int newShooterBall;

int ballPosX = 460;
int ballPosY = 10;

float speedX;
float speedY;

bool initialCheck = true;
int checks = 0;
int wordCount = 0;
bool match = false;

float finalX[20];
float finalY[20];

bool start = false;

bool flash = true;

/* FOR BACKGROUNG MUSIC */
void initAudio() 
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) 
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) 
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }

    bgMusic = Mix_LoadMUS("music.mp3");
    if (!bgMusic) 
    {
        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }
    
    popSound = Mix_LoadWAV("pop.wav");
    if (!popSound) 
    {
        printf("Failed to load pop sound! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }
    
    Mix_PlayMusic(bgMusic, -1);
}

void cleanupAudio() 
{
    Mix_FreeMusic(bgMusic);
    
    Mix_FreeChunk(popSound);
    
    Mix_CloseAudio();
    
    SDL_Quit();
}

void playPopSound() 
{
    if (popSound) {
        Mix_PlayChannel(-1, popSound, 0);
    }
}
/* FOR BACKGROUNG MUSIC AND AUDIO */

//USED THIS CODE FOR WRITING THE IMAGES TO .bin FILE
void RegisterTextures_Write()
//Function is used to load the textures from the
// files and display
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);
	vector<unsigned char> data;
	ofstream ofile("image-data.bin", ios::binary | ios::out);
	// now load each cookies data...

	for (int i = 0; i < nalphabets; ++i) {

		// Read current cookie

		ReadImage(tnames[i], data);
		if (i == 0) {
			int length = data.size();
			ofile.write((char*)&length, sizeof(int));
		}
		ofile.write((char*)&data[0], sizeof(char) * data.size());

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ofile.close();

}
void RegisterTextures()
/*Function is used to load the textures from the
* files and display*/
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);

	vector<unsigned char> data;
	ifstream ifile("image-data.bin", ios::binary | ios::in);

	if (!ifile) {
		cout << " Couldn't Read the Image Data file ";
		//exit(-1);
	}
	// now load each cookies data...
	int length;
	ifile.read((char*)&length, sizeof(int));
	data.resize(length, 0);
	for (int i = 0; i < nalphabets; ++i) {
		// Read current cookie
		//ReadImage(tnames[i], data);
		/*if (i == 0) {
		int length = data.size();
		ofile.write((char*) &length, sizeof(int));
		}*/
		ifile.read((char*)&data[0], sizeof(char)* length);

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ifile.close();
}
void DrawAlphabet(const alphabets &cname, int sx, int sy, int cwidth = 60, int cheight = 60)
{
	/*Draws a specfic cookie at given position coordinate
	* sx = position of x-axis from left-bottom
	* sy = position of y-axis from left-bottom
	* cwidth= width of displayed cookie in pixels
	* cheight= height of displayed cookiei pixels.
	* */
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mtid[cname]);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// glutSwapBuffers();
}
int GetAlphabet() 
{
	return GetRandInRange(0, 26);
}

void Pixels2Cell(int px, int py, int & cx, int &cy) 
{
	cx = round((px + 10) / 60);
	cy = round((py + 10) / 60);
}
void Cell2Pixels(int cx, int cy, int & px, int &py)
{
	// converts the cell coordinates to pixel coordinates...
	px = cx * 60 + 10;
	py = cy * 60 + 10;
}
void DrawShooter(int sx, int sy, int cwidth = 60, int cheight = 60)
{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, -1);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}

// MY FUNCTIONS
void writeToTxt(string word)
{
	ofstream outFile("words_made.txt", ios::app);
	if (outFile.is_open())
	{
		outFile << ++wordCount << ". " << word << endl;
	}
	outFile.close();
}

void horzWordCheck(int start, int i)
{
	for (int k = 14; k < start - 1; k--)
	{
		string str = "";

		for (int j = start; j <= k; j++)
		{
			if(board[i][j] == -1)
			{
				break;
			}

			str += char ('a' + board[i][j]);
		}

		if (str.length() > 2)
		{
			for (int word = 0; word < dictionarysize; word++)
			{
				if (str.length() == dictionary[word].length() && str == dictionary[word])
				{
					cout << "found: " << str << endl;
					writeToTxt(str);
					playPopSound();

					score += str.length();

					for (int j = start; j <= k; j++)
					{
						board[i][j] = -1;
					}
				}
			}
		}
	}

	int end = start;

	for (int k = 0; k < end - 1; k++)
	{
		string str = "";

		for (int j = k; j <= end; j++)
		{
			if(board[i][j] == -1)
			{
				break;
			}

			str += char ('a' + board[i][j]);
		}

		if (str.length() > 2)
		{
			for (int word = 0; word < dictionarysize; word++)
			{
				if (str.length() == dictionary[word].length() && str == dictionary[word])
				{
					cout << "found: " << str << endl;
					writeToTxt(str);
					playPopSound();

					score += str.length();

					for (int j = k; j <= end; j++)
					{
						board[i][j] = -1;
					}
				}
			}
		}
	}
}

void vertWordCheck(int end, int j)
{
	for (int k = 0; k < end - 1; k++)
	{
		string str = "";

		for (int i = k; i <= end; i++)
		{
			if (board[i][j] == -1)
			{
				break;
			}

			str += char('a' + board[i][j]);
		}

		if (str.length() > 2)
		{
			for (int word = 0; word < dictionarysize; word++)
			{
				if (str.length() == dictionary[word].length() && str == dictionary[word])
				{
					cout << "found: " << str << endl;
					writeToTxt(str);
					playPopSound();

					score += str.length();

					for (int i = k; i <= end; i++)
					{
						board[i][j] = -1;
					}
				}
			}
		}
	}
}

void rightDiagWordCheck(int startX, int startY)
{
	int endX = startX;
	int endY = startY;

	while (endY - 1 >= 0 && endX + 1 <= 14)
	{
		endY--;
		endX++;
	}

	while(endY < startY && endX > startX)
	{
		int i = startY;
		int j = startX;

		string str = "";	

		while (i >= endY && j <= endX)
		{
			if (board[i][j] == -1)
			{
				break;
			}

			str += char('a' + board[i--][j++]);
		}

		if (str.length() > 2)
		{
			for (int word = 0; word < dictionarysize; word++)
			{
				if (str.length() == dictionary[word].length() && str == dictionary[word])
				{
					cout << "found: " << str << endl;
					writeToTxt(str);
					playPopSound();

					score += str.length();

					i = startY;
					j = startX;

					for (int p = 0; p < str.length(); p++)
					{
						board[i--][j++] = -1;
					}
				}
			}
		}

		endY++;
		endX--;
	}
}

void leftDiagWordCheck(int startX, int startY)
{
	int endX = startX;
	int endY = startY;

	while (endY - 1 >= 0 && endX - 1 >= 0)
	{
		endY--;
		endX--;
	}

	while(endY < startY && endX < startX)
	{
		int i = startY;
		int j = startX;

		string str = "";	

		while (i >= endY && j >= endX)
		{
			if (board[i][j] == -1)
			{
				break;
			}

			str = char('a' + board[i--][j--]) + str;
		}

		if (str.length() > 2)
		{
			for (int word = 0; word < dictionarysize; word++)
			{
				if (str.length() == dictionary[word].length() && str == dictionary[word])
				{
					cout << "found: " << str << endl;
					writeToTxt(str);
					playPopSound();

					score += str.length();

					i = startY;
					j = startX;

					for (int p = 0; p < str.length(); p++)
					{
						board[i--][j--] = -1;
					}
				}
			}
		}

		endY++;
		endX++;
	}
}

bool checkCollision()
{
	int tmpX;
	int tmpY;

	Pixels2Cell(ballPosX, ballPosY, tmpX, tmpY);

	bool top = (9 - tmpY - 1 >= 0) && (board[9 - tmpY - 1][tmpX] != -1);
	bool topLeft = (9 - tmpY - 1 >= 0) && (tmpX - 1 >= 0) && (board[9 - tmpY - 1][tmpX - 1] != -1);
	bool topRight = (9 - tmpY - 1 >= 0) && (tmpX + 1 < 15) && (board[9 - tmpY - 1][tmpX + 1] != -1);

	bool bottom = (9 - tmpY + 1 < 10) && (board[9 - tmpY + 1][tmpX] != -1);
	bool bottomLeft = (9 - tmpY + 1 > 10) && (tmpX - 1 >= 0) && (board[9 - tmpY + 1][tmpX - 1] != -1);
	bool bottomRight = (9 - tmpY + 1 > 10) && (tmpX + 1 < 15) && (board[9 - tmpY + 1][tmpX + 1] != -1);

	bool left = (tmpX - 1 >= 0) && (board[9 - tmpY][tmpX - 1] != -1);
	bool right = (tmpX + 1 < 15) && (board[9 - tmpY][tmpX + 1] != -1);

	// return top || topLeft || topRight || bottom || bottomLeft || bottomRight || left || right;
	return top || bottom || left || right;
}

void displayRows()
{
	int posX;
	int posY = 550;

	for (int i = 0; i < 10; i++)
	{
		posX = 10;
		for (int j = 0; j < 15; j++)
		{
			DrawAlphabet((alphabets)board[i][j], posX, posY, awidth, aheight);
			posX += 60;
		}
		posY -= 60;
	}
}

void updateBall()
{
	ballPosX += speedX;
	ballPosY += speedY;

	if (ballPosX <= 10 || ballPosX >= width - 61)
	{
		speedX = -speedX;
	}

	if (checkCollision())
	{
		Pixels2Cell(ballPosX, ballPosY, ballPosX, ballPosY);

		if (board[9 - ballPosY][ballPosX] == -1)
		{
			board[9 - ballPosY][ballPosX] = shooterBall;
		}
		else
		{
			if (speedX > 0)
			{
				board[9 - ballPosY + 1][ballPosX - 1] = shooterBall;
			}
			else
			{
				board[9 - ballPosY + 1][ballPosX + 1] = shooterBall;
			}
		}

		if (ballPosX >= 6 && ballPosX <= 9 && 9 - ballPosY == 8)
		{
			sec = 0;
		}

		Cell2Pixels(ballPosX, ballPosY, ballPosX, ballPosY);
	}
}

void resetball()
{
	speedX = 0;
	speedY = 0;

	ballPosX = 460;
	ballPosY = 10;

	shooterBall = newShooterBall;
	newShooterBall = GetAlphabet();
}

void checkRows(int i, int startX)
{
    string longestWord = "";
    int endIndex = -1;

    for (int endx = 14; endx > startX + 1; endx--)
    {
        string str = "";
        for (int j = startX; j <= endx; j++)
        {
            str += char('a' + board[i][j]);
        }

        for (int word = 0; word < dictionarysize; word++)
        {
            if (str == dictionary[word])
            {
                if (str.length() > longestWord.length())
                {
                    longestWord = str;
                    endIndex = endx;
                }
            }
        }
    }

    if (longestWord != "")
    {
        if (checks == 4)
        {
            return;
        }
        
        score += longestWord.length();

        cout << "found: " << longestWord << endl;
        writeToTxt(longestWord);

        for (int start = startX; start <= endIndex; start++)
        {
            board[i][start] = -1;
        }
        
        checks++;
    }
}

void aimer()
{
	for (int i = 19; i >= 0; i--)
	{
		DrawString(finalX[i], finalY[i], width, height, "x", colors[DARK_RED]);
	}
}

/*
* Main Canvas drawing function.
* */
void DisplayFunction() 
{
	// set the background color using function glClearColor.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim red and 1 means pure red and so on.
	//#if 0
	glClearColor(1/*Red Component*/, 1/*Green Component*/,
		1/*Blue Component*/, 0 /*Alpha component*/); // Red==Green==Blue==1 --> White Colour
	glClear(GL_COLOR_BUFFER_BIT); //Update the colors

	//write your drawing commands here or call your drawing functions...

	if (start)
	{

		if (initialCheck)
		{	
			bool popped = false;
			
			displayRows();

			if (checks == 4)
			{
				// Fill if empty space is left
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 15; j++)
					{
						if (board[i][j] == -1)
						{
							board[i][j] = GetAlphabet();
						}
					}
				}

				initialCheck = false;

				return;
			}

			if (sec <= 149.95 || sec <= 149.85)
			{
				bool fill = false;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 15; j++)
					{
						if (board[i][j] == -1)
						{
							board[i][j] = GetAlphabet();
							popped = false;
							fill = true;
						}
					}
				}
				if (!fill)
				{
					checks = 4;
				}
			}

			if (sec > 149.95 || sec > 149.85)
			{
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 15; j++)
					{
						checkRows(i, j);
					}
				}
				popped = true;
			}
		}
		else if (sec > 0)
		{
			/*Display Rows*/
			displayRows();
				
			/*Move Ball*/
			if (!checkCollision())
			{
				updateBall();
			}

			/*When the ball reaches the rows*/
			else
			{
				Pixels2Cell(ballPosX, ballPosY, ballPosX, ballPosY);

				leftDiagWordCheck(ballPosX, 9 - ballPosY);
				rightDiagWordCheck(ballPosX, 9 - ballPosY);
				vertWordCheck(9 - ballPosY, ballPosX);
				horzWordCheck(ballPosX, 9 - ballPosY);

				Cell2Pixels(ballPosX, ballPosY, ballPosX, ballPosY);

				/*Generate new ball and reset its position*/
				resetball();
			}


			/* SHOOTER'S BALL */
			DrawAlphabet((alphabets)shooterBall, ballPosX, ballPosY, awidth, aheight);
			DrawAlphabet((alphabets)newShooterBall, 800, 10, awidth, aheight);

			/* Aimer */
			aimer();

			/* TEXTS TO BE DISPLAYED */
			DrawString(40, height - 20, width, height + 5, "Score: " + Num2Str(score), colors[BLUE_VIOLET]);
			DrawString(width / 2 - 130, height - 20, width, height + 5, "Hassan Ahmed [24i-2521]", colors[BLUE_VIOLET]);
			DrawString(width - 200, height - 25, width, height, "Time Left:" + Num2Str(sec) + " sec", colors[RED]);
			DrawString(790, 90, width, height, "Next ball:", colors[BLACK]);
		}
		else
		{
			/* GAME OVER SCREEN */

			// GAME
			DrawAlphabet((alphabets)6, width / 2 - 180, height / 2 + 70, awidth, aheight);
			DrawAlphabet((alphabets)0, width / 2 - 120, height / 2 + 70, awidth, aheight);
			DrawAlphabet((alphabets)12, width / 2 - 60, height / 2 + 70, awidth, aheight);
			DrawAlphabet((alphabets)4, width / 2, height / 2 + 70, awidth, aheight);

			// OVER
			DrawAlphabet((alphabets)14, width / 2 - 60, height / 2, awidth, aheight);
			DrawAlphabet((alphabets)21, width / 2, height / 2, awidth, aheight);
			DrawAlphabet((alphabets)4, width / 2 + 60, height / 2, awidth, aheight);
			DrawAlphabet((alphabets)17, width / 2 + 120, height / 2, awidth, aheight);

			DrawString(360, height / 2 - 100, width, height + 5, "FINAL SCORE: " + Num2Str(score), colors[GREEN]);
			DrawString(340, height / 2 - 200, width, height + 5, "PRESS [ESC] TO QUIT", colors[GRAY]);
		}
	}
	else
	{
		// WORD
		DrawAlphabet((alphabets)22, width / 2 - 125, height / 2 + 140, awidth, aheight);
		DrawAlphabet((alphabets)14, width / 2 - 65, height / 2 + 140, awidth, aheight);
		DrawAlphabet((alphabets)17, width / 2 - 5, height / 2 + 140, awidth, aheight);
		DrawAlphabet((alphabets)3, width / 2 + 55, height / 2 + 140, awidth, aheight);

		// SHOOTER
		DrawAlphabet((alphabets)18, width / 2 - 220, height / 2 + 70, awidth, aheight);
		DrawAlphabet((alphabets)7, width / 2 - 160, height / 2 + 70, awidth, aheight);
		DrawAlphabet((alphabets)14, width / 2 - 100, height / 2 + 70, awidth, aheight);
		DrawAlphabet((alphabets)14, width / 2 - 40, height / 2 + 70, awidth, aheight);
		DrawAlphabet((alphabets)19, width / 2 + 20, height / 2 + 70, awidth, aheight);
		DrawAlphabet((alphabets)4, width / 2 + 80, height / 2 + 70, awidth, aheight);
		DrawAlphabet((alphabets)17, width / 2 + 140, height / 2 + 70, awidth, aheight);

		// START
		if (flash)
		{
			DrawAlphabet((alphabets)14, width / 2 - 240, height / 2 - 200, 20, 20);
			DrawAlphabet((alphabets)14, width / 2 - 220, height / 2 - 180, 20, 20);
			DrawAlphabet((alphabets)14, width / 2 - 200, height / 2 - 160, 20, 20);
			DrawAlphabet((alphabets)14, width / 2 - 220, height / 2 - 140, 20, 20);
			DrawAlphabet((alphabets)14, width / 2 - 240, height / 2 - 120, 20, 20);

			DrawAlphabet((alphabets)14, width / 2 + 200, height / 2 - 200, 20, 20);
			DrawAlphabet((alphabets)14, width / 2 + 180, height / 2 - 180, 20, 20);
			DrawAlphabet((alphabets)14, width / 2 + 160, height / 2 - 160, 20, 20);
			DrawAlphabet((alphabets)14, width / 2 + 180, height / 2 - 140, 20, 20);
			DrawAlphabet((alphabets)14, width / 2 + 200, height / 2 - 120, 20, 20);

			flash = false;
		}
		else
		{
			flash = true;
		}

		DrawAlphabet((alphabets)18, width / 2 - 160, height / 2 - 180, awidth, aheight);
		DrawAlphabet((alphabets)19, width / 2 - 100, height / 2 - 180, awidth, aheight);
		DrawAlphabet((alphabets)0, width / 2 - 40, height / 2 - 180, awidth, aheight);
		DrawAlphabet((alphabets)17, width / 2 + 20, height / 2 - 180, awidth, aheight);
		DrawAlphabet((alphabets)19, width / 2 + 80, height / 2 - 180, awidth, aheight);
	}

	// #----------------- Write your code till here ----------------------------#
	//DO NOT MODIFY THESE LINES
	DrawShooter((width / 2) - 35, 0, bwidth, bheight);
	glutSwapBuffers();
	//DO NOT MODIFY THESE LINES..
}

/* Function sets canvas size (drawing area) in pixels...
*  that is what dimensions (x and y) your game will have
*  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
* */
void SetCanvasSize(int width, int height) 
{
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.*/
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
* is pressed from the keyboard
*
* You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
*
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
*
* */

void NonPrintableKeys(int key, int x, int y) 
{
	if (key == GLUT_KEY_LEFT /*GLUT_KEY_LEFT is constant and contains ASCII for left arrow key*/) {
		// what to do when left key is pressed...

	}
	else if (key == GLUT_KEY_RIGHT /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/) {

	}
	else if (key == GLUT_KEY_UP/*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/) {
	}
	else if (key == GLUT_KEY_DOWN/*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/) {
	}

	/* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
	* this function*/
	/*
	glutPostRedisplay();
	*/
}
/*This function is called (automatically) whenever your mouse moves witin inside the game window
*
* You will have to add the necessary code here for finding the direction of shooting
*
* This function has two arguments: x & y that tells the coordinate of current position of move mouse
*
* */

void MouseMoved(int x, int y) 
{
	int aimX = 490;
	int aimY = 620;

	float d = sqrt((pow((x - aimX), 2) + pow((aimY - y), 2)));

	float ratios[20];
	float tmp = 100.0;

	for (int i = 0; i < 20; i++)
	{
		ratios[i] = d / tmp;
		tmp += 60;
	}

	for (int i = 0; i < 20; i++)
    {
        float dx = ((x - aimX) / ratios[i] + aimX);
        float dy = ((aimY - y) / ratios[i] + 10);
        
        if (dx <= 10)
        {
            // Reflect from left wall
            dx = 10 + (10 - dx);
        }
        else if (dx >= 920)
        {
            // Reflect from right wall
            dx = 920 - (dx - 920);
        }
        
        finalX[i] = dx;
        finalY[i] = dy;
    }
}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
*
* You will have to add the necessary code here for shooting, etc.
*
* This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
* x & y that tells the coordinate of current position of move mouse
*
* */

void MouseClicked(int button, int state, int x, int y) 
{

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) // dealing only with left button
	{
		if (start)
		{
			// Minimum y coordinate check
			if (y >= 600)
			{
				y = 600;
			}

			if (speedX == 0 && speedY == 0)
			{
				// Change in x and y positions
				float dx = x - ballPosX;
				float dy = height - (y - ballPosY);

				// Hypotenuse
				float d = sqrt(dx * dx + dy * dy);

				// Step speed
				float spdx = 810 / FPS;
				float spdy = 540 / FPS;

				// Speed at which the ball's coordinates will change with
				speedX = (dx / d) * spdx;
				speedY = (dy / d) * spdy;
			}
		}
		else
		{
			if (x >= (width / 2 - 60) && x <= (width / 2 + 80) && y >= 450 && y <= 510)
			{
				start = true;
			}
		}

	}
	else if (button == GLUT_RIGHT_BUTTON) // dealing with right button
	{
		if (state == GLUT_UP)
		{

		}
	}
	glutPostRedisplay();
}
/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
* is pressed from the keyboard
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
* */
void PrintableKeys(unsigned char key, int x, int y) 
{
	if (key == KEY_ESC/* Escape key ASCII*/) {
		exit(1); // exit the program when escape key is pressed.
	}
}

/*
* This function is called after every 1000.0/FPS milliseconds
* (FPS is defined on in the beginning).
* You can use this function to animate objects and control the
* speed of different moving objects by varying the constant FPS.
*
* */
void Timer(int m) 
{
	if (start)
	{
		if (sec > 0)
		{
			sec -= 1 / float(FPS);
		}
	}

	glutPostRedisplay();
	glutTimerFunc(1000.0/FPS, Timer, 0);
}

/*
* our gateway main function
* */
int main(int argc, char*argv[]) 
{
	InitRandomizer(); // seed the random number generator...

	//Dictionary for matching the words. It contains the 369646 words.
	dictionary = new string[dictionarysize]; 
	ReadWords("words_alpha.txt", dictionary); // dictionary is an array of strings

	// Open file to store popped words
	ofstream outFile("words_made.txt");
	if (outFile.is_open())
	{
		outFile << "Words Made: " << endl
				<< "-----------" << endl;
	}
	outFile.close();

	//Write your code here for filling the canvas with different Alphabets. You can use the Getalphabet function for getting the random alphabets

	// Allocate Memory for board
	for (int i = 0; i < 10; i++)
	{
		board[i] = new int[15];
	}

	// Populate board with letters
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (i < nfrows)
			{
				board[i][j] = GetAlphabet();
			}
			else
			{
				board[i][j] = -1;
			}
		}
	}

	// Initial Shooter ball
	shooterBall = GetAlphabet();
	newShooterBall = GetAlphabet();

	initAudio(); // Initialize audio for bg music
	// Mix_VolumeMusic(50);

	glutInit(&argc, argv); // initialize the graphics library...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(50, 50); // set the initial position of our window
	glutInitWindowSize(width, height); // set the size of our window
	glutCreateWindow("HASSAN AHMED | 24i-2521"); // set the title of our game window
	//SetCanvasSize(width, height); // set the number of pixels...

	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.
	RegisterTextures();
	glutDisplayFunc(DisplayFunction); // tell library which function to call for drawing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters
	glutMouseFunc(MouseClicked);
	glutPassiveMotionFunc(MouseMoved); // Mouse

	//// This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
	glutTimerFunc(1000.0/FPS, Timer, 0);

	//// now handle the control to library and it will call our registered functions when
	//// it deems necessary...

	atexit(cleanupAudio);

	glutMainLoop();

	// Free Board
	for (int i = 0; i < 10; i++) 
	{
		delete[] board[i];
	}

	delete[] board;

	return 1;
}
#endif /* */
