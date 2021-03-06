#pragma once
#include "base.h"
#include "PicImage.h"
#include "SPImage.h"

#define REP_COLOR 0
#define CL1_COLOR 1
#define CL2_COLOR 2
#define VARIANCE 3
#define SATURATE_FACTOR 1.1

class Palette {
private:
	std::vector<double> importanceMap;
	std::vector<double> curWeights;
	std::vector<cv::Scalar> colors;
	std::vector<double> assignedSP;
	std::vector<double> margProbs;
	PicImage* origImage;
	SPImage* pixelImage;
	double paletteChange;
	double temp;
	int curSize;
	int maxSize;
	bool reuse = false;

public:
	Palette();
	Palette(PicImage* img1, SPImage* img2, int size, double cT, cv::Scalar startColor1, cv::Scalar startColor2);
	Palette(PicImage* img1, SPImage* img2, int size, double cT, cv::Vec3b startColor1, cv::Vec3b startColor2);
	void associatePalette();
	void refinePalette();
	bool expandPalette();
	double colorDist(cv::Scalar icolor, int pcolor);
	double weight(int num);
	void editColor(int num, cv::Scalar newColor);
	double getChange();
	std::vector<cv::Scalar>* getColors();
	int getCurSize();
	double getCurTemp();
	void permutePCA(int pcolor);
	void displayPixelImage(int scale, std::string path="", bool wait=false);
	cv::Scalar getColor(int pcolor);
	double pairDiff(int num);
	void splitPair(int num);
	cv::Scalar getClusterAvg(int pcolor);
	void saturatePalette();
	bool importMat(std::string filename);
};

