#pragma once
#include "base.h"

class Pixel {
private:
	int xcoor;
	int ycoor;
	cv::Scalar color;
	int spNum;
public:
	Pixel();
	Pixel(int x, int y, cv::Scalar curColor);
	Pixel(int x, int y, int num, cv::Scalar curColor);
	int getSpNum();
	cv::Scalar getColor();
	std::vector<int> getCoor();
	void setSpNum(int num);
};

