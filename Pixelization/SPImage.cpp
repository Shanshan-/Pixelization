#include "SPImage.h"

/* CONSTRUCTORS */
SPImage::SPImage() {
	std::cout << "The SuperPixel class must be constructed with dimensions given";
	exit(-1);
}

SPImage::SPImage(int xdim, int ydim, int size, cv::Scalar meanColor) {
	dimensions[0] = xdim;
	dimensions[1] = ydim;
	spSize = size;
	pixels.resize(xdim * ydim);
	double offset = size / 2;
	for (int x = 0; x < xdim * ydim; x++) {
		//TODO: determine the centroid coordinates
		double centerx = offset + std::floor(x / ydim) * size;
		double centery = offset + (x % ydim) * size;
		pixels[x] = new SuperPixel(centerx, centery, x / xdim, x % xdim, spSize, x, meanColor); //TODO: call delete later
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
	return *(pixels[x*dimensions[1] + y]);
}

SuperPixel SPImage::getPixel(int num) {
	return *(pixels[num]);
}

int SPImage::numPixels() {
	return dimensions[0] * dimensions[1];
}

int SPImage::getSpSize() {
	return spSize;
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
		std::cout << "(" << (*pixels[x]).getImgXCoor()
			<< "," << (*pixels[x]).getImgYCoor() << ")" << std::endl;
	}
}
