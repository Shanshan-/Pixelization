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
	std::vector<double> margProbs;
	Image* origImage;
	SPImage* pixelImage;
	double paletteChange;
	double convergeThresh;
	double temp;
	int curSize;
	int maxSize;

public:
	Palette();
	Palette(Image* img1, SPImage* img2, int size, double cT, double thresh, cv::Scalar startColor);
	Palette(Image* img1, SPImage* img2, int size, double cT, double thresh, cv::Vec3b startColor);
	void associatePalette();
	void refinePalette();
	void expandPalette();
	double colorDist(cv::Scalar icolor, int pcolor);
	double weight(int num);
	void addColor(cv::Scalar newColor);
	void editColor(int num, cv::Scalar newColor);
	double getChange();
	std::vector<std::vector<cv::Scalar>> getColors();
	int getCurSize();
};

