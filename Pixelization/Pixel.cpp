#include "Pixel.h"

/* CONSTRUCTORS */
Pixel::Pixel() {
}

Pixel::Pixel(int x, int y, cv::Scalar curColor) {
	xcoor = x;
	ycoor = y;
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

std::vector<int> Pixel::getCoor() {
	return std::vector<int>(xcoor, ycoor);
}

void Pixel::getSpNum(int num) {
	spNum = num;
}