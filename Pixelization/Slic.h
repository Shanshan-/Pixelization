#pragma once
#include "base.h"

class Slic {
private:
	std::vector<std::vector<int>> spNum; //stores the superpixel assignments of each pixel
	std::vector<cv::Scalar> color; //stores the color value of each superpixel
	std::vector<std::vector<double>> centers; //stores all of the superpixel centroid coordinates
	cv::Mat *image;
	int numRows;
	int numCols;
	int pixelSize;
	int numSRows;
	int numSCols;

public:
	Slic(); 
	Slic(int rows, int cols, int pixSize, cv::Scalar defVal, cv::Mat *img);
	double distance(int pixelx, int pixely, int centroid);
	void refine();
	void printAssignments();
	void printCenters();

	/* Other potential functions:
		- displaying current palette assignments (using glut and glew)
		- getters for each of three storage vectors
	*/
};

