#pragma once
#include "base.h"
#include "Image.h"
#include "SPImage.h"

class Slic {
private:
	Image* origImage;
	SPImage* pixelImage;

	Slic();
	Slic(Image* img1, SPImage* img2);
	double distance(int pixelx, int pixely, int centroid);
	void refine();
};
