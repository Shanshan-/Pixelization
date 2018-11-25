#pragma once
#include "base.h"

class PicImage {
private:
	int dimensions[2];
	bool equalImport;
	cv::Mat *image;
	std::map<int, short>* importance;
	std::map<int, int> spAssignments;


public:
	PicImage();
	PicImage(cv::Mat* origImage);
	PicImage(cv::Mat* origImage, std::map<int, short>* importanceMap);
	std::vector<int> getSpPixels(int num);
	cv::Scalar getAvgColor(int spNum);
	void assignSP(int spSize);
	//Pixel getPixel(int x, int y);
	int numPixels();
	int rows();
	int cols();
	void printAssignments();
};

