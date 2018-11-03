#include "Pixel.h"

/* CONSTRUCTORS */
Pixel::Pixel() {
}

Pixel::Pixel(int x, int y, cv::Scalar curColor) {
	xcoor = x;
	ycoor = y;
	spNum = -1;
	color = curColor;
}

Pixel::Pixel(int x, int y, int num, cv::Scalar curColor) {
	xcoor = x;
	ycoor = y;
	spNum = num;
	color = curColor;
}

/* GETTERS AND SETTERS */
int Pixel::getSpNum() {
	return spNum;
}

cv::Scalar Pixel::getColor() {
	return color;
}

int Pixel::getXCoor() {
	return xcoor;
}

int Pixel::getYCoor() {
	return ycoor;
}

void Pixel::setSpNum(int num) {
	spNum = num;
}