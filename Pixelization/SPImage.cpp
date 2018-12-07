#include "SPImage.h"

/* CONSTRUCTORS */
SPImage::SPImage() {
	std::cout << "The SuperPixel class must be constructed with dimensions given";
}

SPImage::SPImage(int xdim, int ydim, int size, cv::Scalar meanColor) {
	dimensions[0] = xdim;
	dimensions[1] = ydim;
	spSize = size;
	double offset = size / 2;
	for (int x = 0; x < xdim * ydim; x++) {
		//determine the centroid coordinates
		double centerx = offset + std::floor(x / ydim) * size;
		double centery = offset + (x % ydim) * size;

		//initialize map values
		paletteAssigns[x] = 0;
		centroids[x] = std::make_tuple(centerx, centery);
		condProbs[x] = std::vector<double>(1, 1.0);
	}
}

/* METHODS */
bool SPImage::expandPalette() {
	//TODO: use MCDA algo here <-- implemented in palette.cpp
	std::cout << "SPImage.expandPalette() has not been implemented yet";
	return FALSE;
}

void SPImage::assignCentroids() {
	//TODO: do this <-- implemented in slic.cpp
	std::cout << "SPImage.assignCentroids has not been implemented yet.";
}

/* GETTERS AND SETTERS */
int SPImage::numPixels() {
	return dimensions[0] * dimensions[1];
}

std::map<int, std::tuple<double, double>>* SPImage::getCentroids() {
	return &centroids;
}

cv::Mat* SPImage::getAvgColors() {
	return &avgColors;
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
	int count = 0;
	for (auto it = centroids.begin(); it != centroids.end(); it++) {
		std::cout << "(" << std::get<0>(it->second)
			<< "," << std::get<1>(it->second) << ")" << std::endl;
	}
}
