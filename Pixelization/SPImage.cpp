#include "SPImage.h"

/* CONSTRUCTORS */
SPImage::SPImage() {
	std::cout << "The SuperPixel class must be constructed with dimensions given";
	exit(-1);
}

SPImage::SPImage(int xdim, int ydim, int size, PicImage* origImage) {
	dimensions[0] = std::floor((double)xdim / (double)size);
	dimensions[1] = std::floor((double)ydim / (double)size);
	spSize = size;
	pixels.resize(dimensions[0] * dimensions[1]);

	for (int x = 0; x < dimensions[0] * dimensions[1]; x++) {
		pixels[x] = new SuperPixel(x / dimensions[0], x % dimensions[0], spSize, x); //TODO: call delete later
	}

	/*double offset = (0.0 + size) / 2;
	for (int x = 0; x < dimensions[0] * dimensions[1]; x++) {
		//TODO: determine the centroid coordinates
		double centerx = offset + std::floor((0.0 + x) / dimensions[1]) * size;
		double centery = offset + (x % dimensions[1]) * size;
		pixels[x] = new SuperPixel(centerx, centery, x / xdim, x % xdim, spSize, x); //TODO: call delete later
	}

	std::vector<std::vector<double>> tmp;
	tmp.assign(origImage->numPixels(), std::vector<double>(4, 0));
	for (int pos = 0; pos < origImage->numPixels(); pos++) {
		auto pix = origImage->getPixel(pos);
		tmp[(*pix).getSpNum()][0] += (*pix).getColor()[0];		//l value agg
		tmp[(*pix).getSpNum()][1] += (*pix).getColor()[1];		//a value agg
		tmp[(*pix).getSpNum()][2] += (*pix).getColor()[2];		//b value agg
		tmp[(*pix).getSpNum()][3] += 1;							//counter, used for averaging
	}
	for (int num = 0; num < pixels.size(); num++) {
		cv::Scalar avg = cv::Scalar(
			{tmp[num][0] / tmp[num][3], tmp[num][1] / tmp[num][3], tmp[num][2] / tmp[num][3]});
		pixels[num]->setAvgColor(avg);
		//pixels[num]->setAvgColor(meanColor);
	}*/
}

/* METHODS */

/* GETTERS AND SETTERS */
cv::vector<cv::Scalar> SPImage::getPalette() {
	return palette;
}

SuperPixel* SPImage::getPixel(int x, int y) {
	return pixels[x*dimensions[1] + y];
}

SuperPixel* SPImage::getPixel(int num) {
	return pixels[num];
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
