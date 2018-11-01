#include <GL\glew.h>
#include <GL\freeglut.h>
#include <opencv2\opencv.hpp>
#include <iostream>
#define IMG_PATH "../Images/"

int main(int argc, char** argv) {
	cv::Mat image = cv::imread(IMG_PATH "table.jpg");
	cv::Mat img2 = cv::Mat(600, 800, CV_8UC3, cv::Scalar(100, 250, 30));

	// Check for failure
	if (image.empty())
	{
		std::cout << "Could not open or find the image" << std::endl;
		std::cin.get(); //wait for any key press
		return -1;
	}

	cv::String windowName = "The Guitar"; //Name of the window

	cv::namedWindow(windowName, cv::WINDOW_NORMAL); // Create a window

	cv::imshow(windowName, image); // Show our image inside the created window.

	cv::waitKey(0); // Wait for any keystroke in the window

	cv::destroyWindow(windowName); //destroy the created window
	return 0;

	return 0;
}