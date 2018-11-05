#pragma once
#include "base.h"
#include "Image.h"
#include "SPImage.h"

#define LVAL 0
#define AVAL 1
#define BVAL 2
#define XCOOR 3
#define YCOOR 4
#define COUNT 5
#define RATIO { 4, 6 } //used for moving centroid

class Slic {
private:
	Image* origImage;
	SPImage* pixelImage;
	cv::Mat workingImage;

public:
	Slic();
	Slic(Image* img1, SPImage* img2);
	double distance(int pixelx, int pixely, int centroid);
	void refineSP();
};
