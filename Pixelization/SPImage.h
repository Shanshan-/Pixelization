#pragma once
#include "base.h"

class SPImage {
private:
	int dimensions[2];
	int spSize;
	cv::Mat *image;
	std::map<int, int> paletteAssigns;
	std::map<int, std::tuple<double, double>> centroids;
	std::map<int, std::vector<double>> condProbs;
	cv::Mat avgColors; //TODO: Initialize this

public:
	SPImage();
	SPImage(int xdim, int ydim, int size, cv::Scalar meanColor);
	bool expandPalette();
	void assignCentroids();
	//SuperPixel getPixel(int x, int y);
	//SuperPixel getPixel(int num);
	std::map<int, std::tuple<double, double>>* getCentroids();
	cv::Mat* getAvgColors();
	int getSpSize();
	int numPixels();
	int rows();
	int cols();
	void printCentroids();
};

