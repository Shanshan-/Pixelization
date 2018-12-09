#pragma once
#include "base.h"
#include "SuperPixel.h"
#include "PicImage.h"

class SPImage {
private:
	int dimensions[2];
	int spSize;
	std::vector<SuperPixel*> pixels;
	std::vector<cv::Scalar> palette;

public:
	SPImage();
	SPImage(int xdim, int ydim, int size, PicImage* origImage);
	cv::vector<cv::Scalar> getPalette();
	SuperPixel* getPixel(int x, int y);
	SuperPixel* getPixel(int num);
	int getSpSize();
	int numPixels();
	int rows();
	int cols();
	void printCentroids();
};

