#include "PicImage.h"

/* CONSTRUCTORS */
PicImage::PicImage() {
}

PicImage::PicImage(cv::Mat* origImage) {
	dimensions[0] = origImage->rows;
	dimensions[1] = origImage->cols;
	image = origImage;
	equalImport = TRUE;
}

PicImage::PicImage(cv::Mat* origImage, std::map<int, short>* importanceMap) {
	dimensions[0] = origImage->rows;
	dimensions[1] = origImage->cols;
	image = origImage;
	equalImport = FALSE;
	importance = importanceMap;
}

/* METHODS */
std::vector<int> PicImage::getSpPixels(int num) {
	std::vector<int> ans = std::vector<int>();
	int spSize = 0;
	for (auto it = spAssignments.begin(); it != spAssignments.end(); it++) {
		if (it->second == num) { //second = value
			ans.push_back(it->first); //first = key
		}
	}
	return ans;
}

void PicImage::initSP(int spSize) {
	//TODO: change so that robust to not being exact
	for (int x = 0; x < dimensions[0] * dimensions[1]; x++) {
		int tmp1 = int((x / dimensions[1]) / spSize) * (dimensions[1] / spSize);
		int tmp2 = int((x % dimensions[1]) / spSize);
		spAssignments[x] = tmp1 + tmp2;
		//std::cout << x << std::endl;
	}
}

void PicImage::setSpNum(int pixel, int spNum) {
	spAssignments[pixel] = spNum;
}

void PicImage::setSpNum(int x, int y, int spNum) {
	spAssignments[x*dimensions[1] + y] = spNum;
}

cv::Scalar PicImage::getAvgColor(int spNum) {
	//TODO: implement this
	std::cout << "PicImage.getAvgColor() has not been implemented yet.";
	return NULL;
}

/* GETTERS AND SETTERS */

int PicImage::numPixels() {
	return dimensions[0] * dimensions[1];
}

int PicImage::rows() {
	return dimensions[0];
}

int PicImage::cols() {
	return dimensions[1];
}

std::map<int, int>* PicImage::getSpAssigns() {
	return &spAssignments;
}

cv::Mat* PicImage::getImage() {
	return image;
}

/* PRINT FUNCTIONS */
void PicImage::printAssignments() {
	int count = 0;
	for (auto it = spAssignments.begin(); it != spAssignments.end(); it++) {
		std::cout << it->second << "\t";
		count++;
		if (count == dimensions[1]) {
			count = 0;
			std::cout << std::endl;
		}
	}
}