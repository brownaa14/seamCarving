#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"
#include <assert.h>

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
		delete[] image; // delete array of pointers
		return nullptr;
	}

	// initialize cells
	cout << "Initializing cells..." << endl;
	for (int row = 0; row < height; ++row) {
		for (int col = 0; col < width; ++col) {
			cout << "(" << col << ", " << row << ")" << endl;
			image[row][col] = { 0, 0, 0 };
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
	for (int row = 0; row < height; ++row) {
		for (int col = 0; col < width; ++col) {
			ofs << image[row][col].r << " " << image[row][col].g << " " << image[row][col].b << " ";
		}
		ofs << endl;
	}

	return true;
}

int energy(Pixel** image, int y, int x, int width, int height) {
	//I think the format of index should be [y][x] OR my vertical seam isnt checking boundaries correctly before going left or right
	//cout << "energy x: " << x << " y: " << y << endl;
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

//find pixels with least energy, while traveling down every step
int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int* seam){
	// Ensure image pointer is not null
    assert(image != nullptr);

    // Check if width and height are non-negative
    assert(width >= 0 && height >= 0);

    // Verify the dimensions of the image array
    //for (int col = 0; col < width; ++col) {
    //    assert(image[col] != nullptr); // Check if each column is not null
    //}

	//cout << "loadVerticalSeam" << endl;
	seam = createSeam(height);
	int col = start_col;
	int totalEnergy = energy(image, 0, col, width, height);
	seam[0] = col;
	for (int row = 0; row < height-1; ++row) {
		//cout << "loadVerticalSeam" << endl;

		int energyDown = -1;
		int energyLeft = -1;
		int energyRight = 99999;

		//cout << "col: " << col << " row: " << row << endl;
		if (col + 1 >= width) {
			col--;
			energyDown = energy(image, col, row, width, height);
			energyLeft = energy(image, col - 1, row, width, height);
		}
		else if (col - 1 < 0) {
			col++;
			energyDown = energy(image, col, row, width, height);
			energyRight = energy(image, col + 1, row, width, height);
		}
		else {
			energyDown = energy(image, col, row, width, height);
			energyLeft = energy(image, col - 1, row, width, height);
			energyRight = energy(image, col + 1, row, width, height);
		}

		//cout << "Down: " << energyDown << " Left: " << energyLeft << " Right: " << energyRight << endl;
		
		if (energyRight < energyDown && energyRight < energyLeft && energyRight >= 0) { totalEnergy += energyRight; col++; }
		else if (energyLeft < energyDown && energyLeft < energyRight && energyLeft >= 0) { totalEnergy += energyLeft; col--; }
		else { totalEnergy += energyDown;}

		seam[row + 1] = col;
		//cout << "row: " << row << endl;
	}
	return totalEnergy;
}

int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam)
{
	//cout << "loadHorizontalSeam" << endl;
	seam = createSeam(height);
	int row = start_row;
	int totalEnergy = energy(image, 0, row, width, height);
	seam[0] = row;
	for (int col = 0; col < width-1; ++col) {

		int energyDown;
		int energyUp;
		int energyStraight;

		if (col + 1 < width) energyStraight = energy(image, col, row + 1, width, height);
		if (row - 1 >= 0 ) energyDown = energy(image, col - 1, row + 1, width, height);
		if (row + 1 <= height ) energyUp = energy(image, col + 1, row + 1, width, height);
		
		//Down less than or equal to both
		if (energyDown <= energyUp && energyDown <= energyStraight) totalEnergy += energyDown;
		else if (energyStraight < energyDown && energyStraight <= energyUp) { totalEnergy += energyStraight; col--; }
		else { totalEnergy += energyUp; row++; }

		seam[col + 1] = row;

		return totalEnergy;
	}
	return totalEnergy;
}

int* findMinVerticalSeam(Pixel** image, int width, int height)
{

	int currSeamVal = 2147483647;
	int minSeamVal = 2147483647;

	int* minSeam = nullptr;

	//run through all seams in image width; return lowest energy seam
	for (int col = 0; col < width - 1; ++col) {

		int* currSeam = createSeam(height);
		//cout << endl << "width: " << width << endl;
		currSeamVal = loadVerticalSeam(image, col, width, height, currSeam);
		if (currSeamVal < minSeamVal) {
			minSeamVal = currSeamVal;
			minSeam = currSeam;
		}
	}
	//cout << minSeamVal << endl;
	return (minSeam != 0 ? minSeam : nullptr);
}

int* findMinHorizontalSeam(Pixel** image, int width, int height)
{
	int currSeamVal = 2147483647;
	int minSeamVal = currSeamVal;

	int* minSeam = nullptr;

	//run through all seams in image height; return lowest energy seam
	for (int row = 0; row < height - 1; ++row) {

		int* currSeam = createSeam(width);
		
		currSeamVal = loadVerticalSeam(image, row, width, height, currSeam);
		if (currSeamVal < minSeamVal) {
			minSeamVal = currSeamVal;
			minSeam = currSeam;
		}
	}
	
	return minSeam;
}

void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam)
{
    // Create a new image with reduced width
    Pixel** newImage = new Pixel * [width - 1];
    for (int i = 0; i < width - 1; ++i) {
        newImage[i] = new Pixel[height];
    }

    // Copy pixels from the original image to the new image; excluding pixels in the seam
    for (int row = 0; row < height; ++row) {
        int newCol = 0;
        for (int col = 0; col < width; ++col) {
			cout << verticalSeam[col] << endl;
            if (col != verticalSeam[row]) {
                newImage[row][newCol] = image[row][col];
			++newCol;
			}
			
        }
    }

    // Delete the original image
    //for (int i = 0; i < width - 1; ++i) {
    //    delete[] image[i];
    //}
    //delete[] image;

    // Update the image pointer to point to the new image
    image = newImage;
}

void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam)
{
    // Create a new image with reduced height
    Pixel** newImage = new Pixel*[width];
    for (int i = 0; i < width - 1; ++i) {
        newImage[i] = new Pixel[height - 1];
    }

    // Copy pixels from the original image to the new image, excluding pixels in the seam
    for (int col = 0; col < width - 1; ++col) {
        int newRow = 0;
        for (int row = 0; row < height - 1; ++row) {
            if (row != horizontalSeam[col]) {
                newImage[col][newRow] = image[col][row];
                ++newRow;
            }
        }
    }

    // Delete the original image
    //for (int i = 0; i < width; ++i) {
    //    delete[] image[i];
    //}
    //delete[] image;

    // Update the image pointer to point to the new image
    image = newImage;
}
