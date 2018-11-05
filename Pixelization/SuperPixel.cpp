#include "SuperPixel.h"

/* CONSTRUCTORS */
SuperPixel::SuperPixel() {
	std::cout << "The SuperPixel class must be constructed with arguments";
	exit(-1);
}

SuperPixel::SuperPixel(int spx, int spy, int pixSize, int num, cv::Scalar curColor) {
	xcoor = spx;
	ycoor = spy;
	spNum = num;
	size = pixSize;
	color = curColor;
	paletteColor = 0;
}

SuperPixel::SuperPixel(double x, double y, int spx, int spy, int pixSize, int num) {
	ximgcoor = x;
	yimgcoor = y;
	xcoor = spx;
	ycoor = spy;
	spNum = num;
	size = pixSize;
	color = NULL;
	paletteColor = -1;
}

SuperPixel::SuperPixel(double x, double y, int spx, int spy, int pixSize, int num, cv::Scalar curColor) {
	ximgcoor = x;
	yimgcoor = y;
	xcoor = spx;
	ycoor = spy; 
	spNum = num;
	size = pixSize;
	color = curColor;
	paletteColor = 0;
}


/* GETTERS AND SETTERS */
cv::Scalar SuperPixel::getColor() {
	return color;
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

void SuperPixel::setColor(cv::Scalar newColor) {
	color = newColor;
}

void SuperPixel::setColor(cv::Vec3b newColor) {
	color = cv::Scalar(newColor[0], newColor[1], newColor[2]);
}

void SuperPixel::setPaletteColor(int newColor) {
	paletteColor = newColor;
}

void SuperPixel::setCentroid(double x, double y) {
	ximgcoor = x;
	yimgcoor = y;
}
