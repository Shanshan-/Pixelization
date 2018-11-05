#pragma once
#include "base.h"
#include "Image.h"
#include "SPImage.h"

#define REP_COLOR 0
#define CL1_COLOR 1
#define CL2_COLOR 2

class Palette {
private:
	std::vector<std::vector<cv::Scalar>> colors;
	Image* origImage;
	SPImage* pixelImage;
	double temp;
	int curSize;
	int maxSize;

public:
	Palette();
	Palette(Image* img1, SPImage* img2, int size, double cT, cv::Scalar startColor);
	Palette(Image* img1, SPImage* img2, int size, double cT, cv::Vec3b startColor);
	void refinePalette();
	void expandPalette();
	double colorDist(cv::Scalar icolor, int pcolor);
};

