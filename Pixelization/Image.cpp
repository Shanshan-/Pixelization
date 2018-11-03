#include "Image.h"

/* CONSTRUCTORS */
Image::Image() {
}

Image::Image(cv::Mat image) {
	dimensions[0] = image.rows;
	dimensions[1] = image.cols;
	pixels.resize(image.cols * image.rows);
	for (int x = 0; x < image.rows; x++) {
		for (int y = 0; y < image.cols; y++) {
			cv::Scalar color = cv::Scalar(image.at<cv::Vec3b>(x, y)[0],
				image.at<cv::Vec3b>(x, y)[1], image.at<cv::Vec3b>(x, y)[2]);
			pixels[x*dimensions[0] + y] = new Pixel(x, y, color); //TODO: call delete later
		}
	}
}

/* METHODS */
std::vector<Pixel*> Image::getSpPixels(int num) {
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

void Image::assignSP(int spSize) {
	for (int x = 0; x < pixels.size(); x++) {
		int tmp1 = int((x / dimensions[0]) / spSize) * (dimensions[1] / spSize);
		int tmp2 = int((x % dimensions[0]) / spSize);
		(*pixels[x]).setSpNum(tmp1 + tmp2);
	}
}

/* GETTERS AND SETTERS */
Pixel Image::getPixel(int x, int y) {
	return *(pixels[x*dimensions[0] + y]);
}

int Image::rows() {
	return dimensions[0];
}

int Image::cols() {
	return dimensions[1];
}

/* PRINT FUNCTIONS */
void Image::printAssignments() {
	for (int x = 0; x < dimensions[0]; x++) {
		for (int y = 0; y < dimensions[1]; y++) {
			std::cout << (*pixels[x*dimensions[0] + y]).getSpNum() << "\t";
		}
		std::cout << std::endl;
	}
}