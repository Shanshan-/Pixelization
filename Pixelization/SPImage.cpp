#include "SPImage.h"

/* CONSTRUCTORS */
SPImage::SPImage() {
	std::cout << "The SuperPixel class must be constructed with dimensions given";
	exit(-1);
}

SPImage::SPImage(int xdim, int ydim, int spSize, cv::Scalar meanColor) {
	dimensions[0] = xdim;
	dimensions[1] = ydim;
	for (int x = 0; x < xdim * ydim; x++) {
		pixels[x] = SuperPixel(x / xdim, x % xdim, spSize, x, meanColor);
	}
}

/* METHODS */
bool SPImage::expandPalette() {
	//TODO: use MCDA algo here
	return FALSE;
}

/* GETTERS AND SETTERS */
cv::vector<cv::Scalar> SPImage::getPalette() {
	return palette;
}

SuperPixel SPImage::getPixel(int x, int y) {
	return pixels[x*dimensions[0] + y];
}

int SPImage::rows() {
	return dimensions[0];
}

int SPImage::cols() {
	return dimensions[1];
}

/* PRINT FUNCTIONS */
void SPImage::printCentroids() {
	for (int x = 0; x < pixels.size(); x++) {
		std::cout << "(" << pixels[x].getImgCoor()[0] << "," << pixels[x].getImgCoor()[1] << ")" << std::endl;
	}
}

void SPImage::printAssignments() {
	for (int x = 0; x < dimensions[0]; x++) {
		for (int y = 0; y < dimensions[1]; y++) {
			std::cout << pixels[x*dimensions[0] + y].getSpNum() << "\t";
		}
		std::cout << std::endl;
	}
}
