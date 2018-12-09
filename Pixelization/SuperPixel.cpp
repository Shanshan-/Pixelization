#include "SuperPixel.h"

/* CONSTRUCTORS */
SuperPixel::SuperPixel() {
	std::cout << "The SuperPixel class must be constructed with arguments";
	exit(-1);
}

SuperPixel::SuperPixel(int spx, int spy, int pixSize, int num) {
	xcoor = spx;
	ycoor = spy;
	spNum = num;
	size = pixSize;
	paletteColor = 0;
	condProbs.push_back({ 1.0 });
}

SuperPixel::SuperPixel(int spx, int spy, int pixSize, int num, cv::Scalar curColor) {
	xcoor = spx;
	ycoor = spy;
	spNum = num;
	size = pixSize;
	color = curColor;
	paletteColor = 0;
	condProbs.push_back({ 1.0 });
}

SuperPixel::SuperPixel(double x, double y, int spx, int spy, int pixSize, int num) {
	ximgcoor = x;
	yimgcoor = y;
	xcoor = spx;
	ycoor = spy;
	spNum = num;
	size = pixSize;
	color = NULL;
	paletteColor = 0;
	condProbs.push_back({ 1.0 });
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
	condProbs.push_back({ 1.0 });
}


/* GETTERS AND SETTERS */
cv::Scalar SuperPixel::getAvgColor() {
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

std::vector<double> SuperPixel::getPaletteProbs() {
	return condProbs;
}

void SuperPixel::setAvgColor(cv::Scalar newColor) {
	color = newColor;
}

void SuperPixel::setAvgColor(cv::Vec3b newColor) {
	color = cv::Scalar(newColor[0], newColor[1], newColor[2]);
}

void SuperPixel::setPaletteColor(int newColor) {
	paletteColor = newColor;
}

void SuperPixel::setCentroid(double x, double y) {
	ximgcoor = x;
	yimgcoor = y;
}

void SuperPixel::setPaletteProb(int num, double prob) {
	//update the probability vector
	if (int(condProbs.size()) < num)
		condProbs.resize(num);
	condProbs[num] = prob;

	//update the color if needed
	if (prob > condProbs[paletteColor]) {
		paletteColor = num;
	}
}

void SuperPixel::addNewProb() {
	double factor = 1.0 - (1.0 / (int(condProbs.size()) + 1.0));
	for (int x = 0; x < int(condProbs.size()); x++) {
		condProbs[x] = condProbs[x] * factor;
	}
	condProbs.push_back(1.0 / (1.0 + condProbs.size()));
	//condProbs.push_back(0.0);
}
