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
	cv::Scalar color1;
	cv::Scalar color2;

public:
	SuperPixel();
	SuperPixel(int spx, int spy, int pixSize, int num, cv::Scalar color);
	SuperPixel(double x, double y, int spx, int spy, int pixSize, int num);
	SuperPixel(double x, double y, int spx, int spy, int pixSize, int num, cv::Scalar color);
	cv::Scalar getColor();
	std::vector<double> getImgCoor();
	std::vector<int> getSpCoor();
	int getSpNum();
	void setColor(cv::Scalar color);
	void setCentroid(double x, double y);
};

