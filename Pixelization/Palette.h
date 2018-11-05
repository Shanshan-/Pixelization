#pragma once
#include "base.h"
#include "Image.h"
#include "SPImage.h"

class Palette {
private:
	std::vector<std::vector<cv::Scalar>> colors;
	Image* origImage;
	SPImage* pixelImage;
	double temp;

public:
	Palette();
	Palette(Image* img1, SPImage* img2, int size, double cT, cv::Scalar startColor);
	Palette(Image* img1, SPImage* img2, int size, double cT, cv::Vec3b startColor);
	void refinePalette();
	void expandPalette();
};

