#pragma once
#include "base.h"
#include "PicImage.h"
#include "SPImage.h"

#define REP_COLOR 0
#define CL1_COLOR 1
#define CL2_COLOR 2
#define VARIANCE 3
#define PCA_FACTOR 5
#define EXPAND_THRESH_FACTOR 0.7
#define SPLIT_THRESH 5 //TODO: experiment with tweaking this number

class Palette {
private:
	std::vector<std::vector<cv::Scalar>> colors;
	std::vector<double> assignedSP;
	std::vector<double> margProbs;
	PicImage* origImage;
	SPImage* pixelImage;
	double paletteChange;
	double temp;
	int curSize;
	int maxSize;

public:
	Palette();
	Palette(PicImage* img1, SPImage* img2, int size, double cT, cv::Scalar startColor);
	Palette(PicImage* img1, SPImage* img2, int size, double cT, cv::Vec3b startColor);
	void associatePalette();
	void refinePalette();
	bool expandPalette();
	double colorDist(cv::Scalar icolor, int pcolor);
	double weight(int num);
	void addColor(cv::Scalar newColor);
	void editColor(int num, cv::Scalar newColor);
	void pcaAll();
	double getChange();
	std::vector<std::vector<cv::Scalar>> getColors();
	int getCurSize();
	double getCurTemp();
	void permutePCA(int pcolor);
	void displayPixelImage(int scale, bool wait=FALSE);
	cv::Scalar getColor(int pcolor);
};

