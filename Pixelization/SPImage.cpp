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
		pixels[x] = new SuperPixel(x / xdim, x % xdim, spSize, x, meanColor); //TODO: call delete later
	}
}

/* METHODS */
bool SPImage::expandPalette() {
	//TODO: use MCDA algo here
	std::cout << "SPImage.expandPalette() has not been implemented yet";
	return FALSE;
}

void SPImage::assignCentroids() {
	//TODO: do this
	std::cout << "SPImage.assignCentroids has not been implemented yet.";
}

/* GETTERS AND SETTERS */
cv::vector<cv::Scalar> SPImage::getPalette() {
	return palette;
}

SuperPixel SPImage::getPixel(int x, int y) {
	return *(pixels[x*dimensions[0] + y]);
}

SuperPixel SPImage::getPixel(int num) {
	return *(pixels[num]);
}

int SPImage::numPixels() {
	return dimensions[0] * dimensions[1];
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
		std::cout << "(" << (*pixels[x]).getImgCoor()[0]
			<< "," << (*pixels[x]).getImgCoor()[1] << ")" << std::endl;
	}
}
