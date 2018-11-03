#include "base.h"
#include "SuperPixel.h"
#include "SPAssignments.h"
#include "SPImage.h"
#include "Image.h"

#define IMG_PATH "../Images/"

cv::Scalar getMeanColor(cv::Mat image) {
	cv::Scalar result = cv::Scalar(0, 0, 0, 0);
	for (int x = 0; x < image.rows; x++) {
		for (int y = 0; y < image.cols; y++) {
			result[0] += image.at<cv::Vec3b>(x, y)[0];
			result[1] += image.at<cv::Vec3b>(x, y)[1];
			result[2] += image.at<cv::Vec3b>(x, y)[2];
		}
	}
	result[0] = result[0] / (image.rows * image.cols);
	result[1] = result[1] / (image.rows * image.cols);
	result[2] = result[2] / (image.rows * image.cols);
	return result;
}

int main(int argc, char **argv) {
	//hardcode initial values for now
	//TODO: these should be taken in as inputs to program (start from console, then by gui)
	int spRows = 10;
	int spCols = 10;
	int paletteSize = 8;

	//load image
	cv::Mat image = cv::imread(IMG_PATH "flower.png");
	if (image.empty()) {
		std::cout << "Could not open or find the image" << std::endl;
		exit(-1);
	}
	//std::cout << &image << std::endl;

	//determine the mean color of image, and generate the palette
	cv::Scalar meanColor = getMeanColor(image);
	std::vector<cv::Scalar> palette(paletteSize);

	//generate image
	Image pimage = Image(image);

	//start here
	SPAssignments sp = SPAssignments(image.rows, image.cols, 2, meanColor, &image);
	SPAssignments sp1 = SPAssignments(8, 4, 2, meanColor, &image);
	//sp1.printAssignments();
	sp.refine();

	//displaying image
	cv::Mat outImg;
	cv::String windowName = "The Guitar"; //Name of the window
	cv::namedWindow(windowName); // , cv::WINDOW_NORMAL); // Create a window
	cv::moveWindow(windowName, 30, 40);
	cv::resize(image, outImg, cv::Size(), 0.4, 0.4);
	cv::imshow(windowName, outImg); // Show our image inside the created window.
	cv::waitKey(0); // Wait for any keystroke in the window
	cv::destroyWindow(windowName); //destroy the created window
	return 0;
}