#pragma once
#include "base.h"
#include "Image.h"
#include "SPImage.h"

class Palette {
private:
	std::vector<cv::Scalar> colors;
	Image* origImage;
	SPImage* pixelImage;

public:
	Palette();
	Palette(cv::Scalar color);
	Palette(cv::Vec3b color);
	void refinePalette();
	void expandPalette();
};

