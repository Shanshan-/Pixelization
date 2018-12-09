#pragma once
#include "base.h"
#include "PicImage.h"
#include "SPImage.h"
#include "Palette.h"

#define LVAL 0
#define AVAL 1
#define BVAL 2
#define XCOOR 3
#define YCOOR 4
#define COUNT 5
#define RATIO { 4, 6 } //used for moving centroid
#define MVAL 45 //paper suggests 45 or 10

#define FD_MULT 2
#define SCALE 20

class Slic {
private:
	PicImage* origImage;
	SPImage* pixelImage;
	Palette* palette;
	cv::Mat workingImage;
	double distFactor;

public:
	Slic();
	Slic(PicImage* img1, SPImage* img2, Palette* pal);
	double distance(int pixelx, int pixely, int centroid);
	void refineSP();
};
