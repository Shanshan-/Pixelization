#include "PicImage.h"

/* CONSTRUCTORS */
PicImage::PicImage() {
}

PicImage::PicImage(cv::Mat image) {
	dimensions[0] = image.rows;
	dimensions[1] = image.cols;
	pixels.resize(image.cols * image.rows);
	for (int x = 0; x < image.rows; x++) {
		for (int y = 0; y < image.cols; y++) {
			cv::Scalar color = cv::Scalar(image.at<cv::Vec3b>(x, y)[0],
				image.at<cv::Vec3b>(x, y)[1], image.at<cv::Vec3b>(x, y)[2]);
			pixels[x*dimensions[1] + y] = new Pixel(x, y, color); //TODO: call delete later
		}
	}
}

/* METHODS */
std::vector<Pixel*> PicImage::getSpPixels(int num) {
	std::vector<Pixel*> ans = std::vector<Pixel*>(10, NULL);
	int spSize = 0;
	for (int x = 0; x < pixels.size(); x++) {
		Pixel cur = *(pixels[x]);
		if (cur.getSpNum() == num) {
			ans[spSize] = pixels[x];
			spSize++;
		}
	}
	return ans;
}

void PicImage::assignSP(int spSize) {
	for (int x = 0; x < pixels.size(); x++) {
		int tmp1 = int((x / dimensions[1]) / spSize) * (dimensions[1] / spSize);
		int tmp2 = int((x % dimensions[1]) / spSize);
		(*pixels[x]).setSpNum(tmp1 + tmp2);
	}
}

cv::Scalar PicImage::getAvgColor(int spNum) {
	//TODO: implement this
	std::cout << "PicImage.getAvgColor() has not been implemented yet.";
	return NULL;
}

/* GETTERS AND SETTERS */
Pixel* PicImage::getPixel(int x, int y) {
	return pixels[x*dimensions[1] + y];
}

Pixel* PicImage::getPixel(int num) {
	return pixels[num];
}

int PicImage::numPixels() {
	return dimensions[0] * dimensions[1];
}

int PicImage::rows() {
	return dimensions[0];
}

int PicImage::cols() {
	return dimensions[1];
}

/* PRINT FUNCTIONS */
void PicImage::printAssignments() {
	for (int x = 0; x < dimensions[0]; x++) {
		for (int y = 0; y < dimensions[1]; y++) {
			std::cout << (*pixels[x*dimensions[1] + y]).getSpNum() << "\t";
		}
		std::cout << std::endl;
	}
}