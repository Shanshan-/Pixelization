#pragma once
#include "base.h"
#include "SuperPixel.h"

class SPImage {
private:
	int dimensions[2];
	std::vector<SuperPixel*> pixels;
	std::vector<cv::Scalar> palette;

public:
	SPImage();
	SPImage(int xdim, int ydim, int spSize, cv::Scalar meanColor);
	bool expandPalette();
	cv::vector<cv::Scalar> getPalette();
	void assignCentroids();
	SuperPixel getPixel(int x, int y);
	SuperPixel getPixel(int num);
	int numPixels();
	int rows();
	int cols();
	void printCentroids();
};

