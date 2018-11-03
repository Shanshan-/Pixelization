#pragma once
#include "base.h"
#include "Pixel.h"

class Image {
private:
	int dimensions[2];
	std::vector<Pixel*> pixels;

public:
	Image();
	Image(cv::Mat image);
	std::vector<Pixel*> getSpPixels(int num);
	cv::Scalar getAvgColor(int spNum);
	void assignSP(int spSize);
	Pixel getPixel(int x, int y);
	int numPixels();
	int rows();
	int cols();
	void printAssignments();
};

