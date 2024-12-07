#include "util.h"

void InitRandomizer() 
{
	srand((unsigned int)time(0)); // time(0) returns number of seconds elapsed since January 1, 1970.
}

long GetRandInRange(const long &rmin, const long &rmax) 
{
	long range = rmax - rmin; // find the range
	long value = (rand() % (long)range) + rmin; // translate the generated number ...

	return value;
}

// Function draws a string at given x,y coordinates
void DrawString(int x, int y, int width, int height, const string &score, float*color) 
{
	float fx = (float)x / width * 2 - 1, fy = (float)y / height * 2 - 1;
	DrawString(fx, fy, score, color);
}

// Function draws a string at given x,y coordinates
void DrawString(float x, float y, const string& score, float * color) 
{
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_24;
	if (color)
		glColor3fv(color);
	
	glRasterPos3f(x, y, 1);
	
	//  Draw the characters one by one
	for (int i = 0; i < score.size(); i++)
		glutBitmapCharacter(font_style, score[i]);

	glPopMatrix();
}

string Num2Str(int t) 
{
	stringstream s;
	s << t;
	return s.str();
}

void ReadImage(string imgname, vector<unsigned char> &imgArray) 
{
	using namespace cimg_library;
	CImg<unsigned char> img(imgname.c_str());
	imgArray.resize(img.height() * img.width() * 3, 0);
	int k = 0;
	unsigned char *rp = img.data();
	unsigned char *gp = img.data() + img.height() * img.width();
	unsigned char *bp = gp + img.height() * img.width();

	for (int j = 0; j < img.width(); ++j) 
	{
		int t = j;
		for (int i = 0; i < img.height(); ++i, t += img.width()) 
		{
			imgArray[k++] = rp[t];
			imgArray[k++] = gp[t];
			imgArray[k++] = bp[t];
		}
	}
}

void ReadWords(string filename, string * dictionary) 
{
	string word;
	int i = 0;

	ifstream ifile;
	ifile.open("words_alpha.txt");
	if (ifile.is_open()) 
	{
		while (!ifile.eof()) 
		{

			ifile >> word;
			dictionary[i] = word;
			i++;

		}
	}
	ifile.close();

}