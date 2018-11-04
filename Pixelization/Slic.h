#pragma once
#include "base.h"
#include "Image.h"
#include "SPImage.h"

#define RATIO { 4, 6 } //used for moving centroid

class Slic {
private:
	Image* origImage;
	SPImage* pixelImage;

public:
	Slic();
	Slic(Image* img1, SPImage* img2);
	double distance(int pixelx, int pixely, int centroid);
	void refineSP();
};
