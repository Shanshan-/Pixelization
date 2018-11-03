#include "SuperPixel.h"

/* CONSTRUCTORS */
SuperPixel::SuperPixel() {
	std::cout << "The SuperPixel class must be constructed with arguments";
	exit(-1);
}

SuperPixel::SuperPixel(int spx, int spy, int pixSize, int num, cv::Scalar color) {
	xcoor = spx;
	ycoor = spy;
	spNum = num;
	size = pixSize;
	color1 = cv::Scalar(color[0], color[1], color[2] + 1);
	color2 = cv::Scalar(color[0], color[1], color[2] - 1);
}

SuperPixel::SuperPixel(double x, double y, int spx, int spy, int pixSize, int num) {
	ximgcoor = x;
	yimgcoor = y;
	xcoor = spx;
	ycoor = spy;
	spNum = num;
	size = pixSize;
}

SuperPixel::SuperPixel(double x, double y, int spx, int spy, int pixSize, int num, cv::Scalar color) {
	ximgcoor = x;
	yimgcoor = y;
	xcoor = spx;
	ycoor = spy; 
	spNum = num;
	size = pixSize;
	color1 = cv::Scalar(color[0], color[1], color[2] + 1);
	color2 = cv::Scalar(color[0], color[1], color[2] - 1);
}


/* GETTERS AND SETTERS */
cv::Scalar SuperPixel::getColor() {
	double ansx = (color1[0] + color2[0]) / 2;
	double ansy = (color1[1] + color2[1]) / 2;
	double ansz = (color1[2] + color2[2]) / 2;
	return cv::Scalar(ansx, ansy, ansz);
}

std::vector<double> SuperPixel::getImgCoor() {
	return std::vector<double>(ximgcoor, yimgcoor);
}

std::vector<int> SuperPixel::getSpCoor() {
	return std::vector<int>(xcoor, ycoor);
}

int SuperPixel::getSpNum() {
	return spNum;
}

void SuperPixel::setColor(cv::Scalar color) {
	color1 = cv::Scalar(color[0], color[1], color[2] + 1);
	color2 = cv::Scalar(color[0], color[1], color[2] - 1);
}

void SuperPixel::setCentroid(double x, double y) {
	ximgcoor = x;
	yimgcoor = y;
}