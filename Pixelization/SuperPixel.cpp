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
	paletteColor = 0;
}

SuperPixel::SuperPixel(double x, double y, int spx, int spy, int pixSize, int num) {
	ximgcoor = x;
	yimgcoor = y;
	xcoor = spx;
	ycoor = spy;
	spNum = num;
	size = pixSize;
	color1 = NULL;
	color2 = NULL;
	paletteColor = -1;
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
	paletteColor = 0;
}


/* GETTERS AND SETTERS */
cv::Scalar SuperPixel::getColor() {
	double ansx = (color1[0] + color2[0]) / 2;
	double ansy = (color1[1] + color2[1]) / 2;
	double ansz = (color1[2] + color2[2]) / 2;
	return cv::Scalar(ansx, ansy, ansz);
}

int SuperPixel::getPaletteColor() {
	return paletteColor;
}

double SuperPixel::getImgXCoor() {
	return ximgcoor;
}

double SuperPixel::getImgYCoor() {
	return yimgcoor;
}

int SuperPixel::getSpXCoor() {
	return xcoor;
}

int SuperPixel::getSpYCoor() {
	return ycoor;
}

int SuperPixel::getSpNum() {
	return spNum;
}

void SuperPixel::setColor(cv::Scalar color) {
	color1 = cv::Scalar(color[0], color[1], color[2] + 1);
	color2 = cv::Scalar(color[0], color[1], color[2] - 1);
}

void SuperPixel::setColor(cv::Vec3b color) {
	color1 = cv::Scalar(color[0], color[1], color[2] + 1);
	color2 = cv::Scalar(color[0], color[1], color[2] - 1);
}

void SuperPixel::setPaletteColor(int color) {
	paletteColor = color;
}

void SuperPixel::setCentroid(double x, double y) {
	ximgcoor = x;
	yimgcoor = y;
}
