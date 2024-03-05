#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

Pixel** createImage(int width, int height) {
	cout << "Start createImage... " << endl;

	// Create a one dimensional array on the heap of pointers to Pixels 
	//    that has width elements (i.e. the number of columns)
	Pixel** image = new Pixel * [width];

	bool fail = false;

	for (int i = 0; i < width; ++i) { // loop through each column
	  // assign that column to a one dimensional array on the heap of Pixels
	  //  that has height elements (i.e. the number of rows)
		image[i] = new Pixel[height];

		if (image[i] == nullptr) { // failed to allocate
			fail = true;
		}
	}

	if (fail) { // if any allocation fails, clean up and avoid memory leak
	  // deallocate any arrays created in for loop
		for (int i = 0; i < width; ++i) {
			delete[] image[i]; // deleting nullptr is not a problem
		}
		delete[] image; // dlete array of pointers
		return nullptr;
	}

	// initialize cells
	//cout << "Initializing cells..." << endl;
	for (int row = 0; row < height; ++row) {
		for (int col = 0; col < width; ++col) {
			//cout << "(" << col << ", " << row << ")" << endl;
			image[col][row] = { 0, 0, 0 };
		}
	}
	cout << "End createImage... " << endl;
	return image;
}

void deleteImage(Pixel** image, int width) {
	cout << "Start deleteImage..." << endl;
	// avoid memory leak by deleting the array
	for (int i = 0; i < width; ++i) {
		delete[] image[i]; // delete each individual array placed on the heap
	}
	delete[] image;
	image = nullptr;
}

int* createSeam(int length) {
	int* seam = new int[length];
	return seam;
}

void deleteSeam(int* seam) {
	delete[] seam;
}

bool loadImage(string filename, Pixel** image, int width, int height) {
	cout << "Loading image..." << endl;
	// declare/define and open input file stream
	ifstream ifs(filename);

	// check if input stream opened successfully
	if (!ifs.is_open()) {
		cout << "Error: failed to open input file - " << filename << endl;
		return false;
	}

	// get type from preamble
	char type[3];
	ifs >> type; // should be P3
	if ((toupper(type[0]) != 'P') || (type[1] != '3')) { // check that type is correct
		cout << "Error: type is " << type << " instead of P3" << endl;
		return false;
	}

	// get width (w) and height (h) from preamble
	int w, h;
	if (!(ifs >> w)) {
		cout << "Error: read non-integer value" << endl;
		return false;
	}
	if (w != width) { // check that width matches what was passed into the function
		cout << "Error: input width (" << width << ") does not match value in file (" << w << ")" << endl;
		return false;
	}
	if (!(ifs >> h)) {
		cout << "Error: read non-integer value" << endl;
		return false;
	}
	if (h != height) { // check that height matches what was passed into the function
		cout << "Error: input height (" << height << ") does not match value in file (" << h << ")" << endl;
		return false;
	}

	// get maximum value from preamble
	int colorMax = 0;
	ifs >> colorMax;
	if (colorMax != 255) {
		cout << "Error: invalid color value " << colorMax << endl;
		return false;
	}

	for (int col = 0; col < height; ++col) {
		for (int row = 0; row < width; ++row) {
			if (ifs >> image[row][col].r && ifs >> image[row][col].g && ifs >> image[row][col].b) {
				if (image[row][col].r > 255 || image[row][col].r < 0) {
					cout << "Error: invalid color value " << image[row][col].r << endl;
					return false;
				}
				else if (image[row][col].r % 1 != 0) {
					cout << "Error: read non-integer value" << endl;
					return false;
				}

				if (image[row][col].g > 255 || image[row][col].g < 0) {
					cout << "Error: invalid color value " << image[row][col].r << endl;
					return false;
				}
				else if (image[row][col].g % 1 != 0) {
					cout << "Error: read non-integer value" << endl;
					return false;
				}

				if (image[row][col].b > 255 || image[row][col].b < 0) {
					cout << "Error: invalid color value " << image[row][col].r << endl;
					return false;
				}
				else if (image[row][col].b % 1 != 0) {
					cout << "Error: read non-integer value" << endl;
					return false;
				}
			}
			else {
				cout << "Error: not enough color values" << endl;
				return false;
			}

		}
	}
	int isMore = 0;
	ifs >> isMore;
	if (ifs.good()) {
		cout << "Error: too many color values" << endl;
		return false;
	}
	return true;
}

bool outputImage(string filename, Pixel** image, int width, int height) {
	cout << "Outputting image..." << endl;

	//declare/define and open output file stream
	ofstream ofs(filename);

	//check if output stream opened successfully
	if (!ofs.is_open()) {
		cout << "Error: failed to open output file " << filename << endl;
	}
	//output preamble
	ofs << "P3" << endl;
	ofs << width << " " << height << endl;
	ofs << "255" << endl;

	//output pixels
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			ofs << image[j][i].r << " " << image[j][i].g << " " << image[j][i].b << " ";
		}
		ofs << endl;
	}

	return true;
}

int energy(Pixel** image, int x, int y, int width, int height) {
	int rX, gX, bX, rY, gY, bY;

	rX = abs(image[x == width - 1 ? 0 : x + 1][y].r - image[x == 0 ? width - 1 : x - 1][y].r);
	gX = abs(image[x == width - 1 ? 0 : x + 1][y].g - image[x == 0 ? width - 1 : x - 1][y].g);
	bX = abs(image[x == width - 1 ? 0 : x + 1][y].b - image[x == 0 ? width - 1 : x - 1][y].b);

	rY = abs(image[x][y == height - 1 ? 0 : y + 1].r - image[x][y == 0 ? height - 1 : y - 1].r);
	gY = abs(image[x][y == height - 1 ? 0 : y + 1].g - image[x][y == 0 ? height - 1 : y - 1].g);
	bY = abs(image[x][y == height - 1 ? 0 : y + 1].b - image[x][y == 0 ? height - 1 : y - 1].b);

	int energyX = rX * rX + gX * gX + bX * bX;
	int energyY = rY * rY + gY * gY + bY * bY;
	int energy = energyX + energyY;

	return energy;
}

int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int* seam){
	
	seam = createSeam(height);
	int col = start_col;
	int totalEnergy = energy(image, 0, col, width, height);
	seam[0] = col;
	for (int row = 0; row < height-1; ++row) {

		int energyDown = 2147483647;
		int energyLeft = 2147483647;
		int energyRight = 2147483647;

		if (row + 1 < height) energyDown = energy(image, row + 1, col, width, height);
		if (col - 1 >= 0 ) energyLeft = energy(image, row + 1, col - 1, width, height);
		if (col + 1 <= width ) energyRight = energy(image, row + 1, col + 1, width, height);
		
		//Down less than or equal to both
		if (energyDown <= energyRight && energyDown <= energyLeft) totalEnergy += energyDown;
		else if (energyLeft < energyDown && energyLeft <= energyRight) { totalEnergy += energyLeft; col--; }
		else { totalEnergy += energyRight; col++; }

		seam[row + 1] = col;
	}
	return totalEnergy;
}

int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam)
{
	return 0;
}

int* findMinVerticalSeam(Pixel** image, int width, int height)
{
	return nullptr;
}

int* findMinHorizontalSeam(Pixel** image, int width, int height)
{
	return nullptr;
}

void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam)
{
}

void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam)
{
}
