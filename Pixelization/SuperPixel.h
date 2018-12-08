#pragma once
#include "base.h"

class SuperPixel {
private:
	double ximgcoor; //of centroid in full image
	double yimgcoor; //of centroid in full image
	int xcoor; //in pixel image
	int ycoor; //in pixel image
	int spNum;
	int size;
	cv::Scalar color;
	int paletteColor;
	std::vector<double> condProbs;

public:
	SuperPixel();
	SuperPixel(int spx, int spy, int pixSize, int num, cv::Scalar curColor);
	SuperPixel(double x, double y, int spx, int spy, int pixSize, int num);
	SuperPixel(double x, double y, int spx, int spy, int pixSize, int num, cv::Scalar color);
	cv::Scalar getColor();
	int getPaletteColor();
	double getImgXCoor();
	double getImgYCoor();
	int getSpXCoor();
	int getSpYCoor();
	int getSpNum();
	std::vector<double> getPaletteProbs();
	void setColor(cv::Scalar color);
	void setColor(cv::Vec3b color);
	void setPaletteColor(int color);
	void setCentroid(double x, double y);
	void setPaletteProb(int num, double prob);
	void addNewProb();
};

