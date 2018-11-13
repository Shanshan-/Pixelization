#include "main.h"

int main(int argc, char **argv) {
	//hardcode initial values for now
	//TODO: these should be taken in as inputs to program (start from console, then by gui)
	int paletteSize = 8;
	int spSize = 3; //squirrel size = 9, scale = 10, mult = 2

	//load image
	cv::Mat image = cv::imread(IMG_PATH "squirrel.jpg");
	//TODO: program breaks if length and width are not exact multiples of spSize
	if (image.empty()) {
		char c;
		std::cout << "Could not open or find the image" << std::endl;
		std::cin >> c;
		exit(-1);
	}

	//determine the mean color of image, and generate the palette
	cv::cvtColor(image, image, cv::COLOR_BGR2Lab);
	cv::Scalar meanColor = getMeanColor(image);
	//std::vector<cv::Scalar> palette(paletteSize);

	//TODO: find the starting temperature
	double startTemp = getStartTemp(image);

	//generate image classes to work with
	Image pimage = Image(image);
	pimage.assignSP(spSize);
	pimage.printAssignments();
	SPImage spImage = SPImage(image.rows / spSize, image.cols / spSize, spSize, meanColor);
	spImage.printCentroids();
	Slic slic = Slic(&pimage, &spImage);
	double curTemp = startTemp;
	
	while (curTemp > FINAL_TEMP) {
		//refine the superpixels
		slic.refineSP();

		//refine the palette
		Palette palette = Palette(&pimage, &spImage, paletteSize, startTemp, meanColor);
		palette.associatePalette();
		palette.refinePalette();

		if (palette.getChange() < TEMP_CHANGE_THRESH) {
			palette.expandPalette();
			curTemp = palette.getCurTemp();
		}
	}

	//start here
	//Slic sp = Slic(image.rows, image.cols, 2, meanColor, &image);
	//Slic sp1 = Slic(8, 4, 2, meanColor, &image);
	////sp1.printAssignments();
	//sp.refineSP();

	//displaying image
	//TODO: move this process into its own function
	cv::Mat outImg;
	cv::String windowName = "Image Results"; //Name of the window
	cv::namedWindow(windowName); // , cv::WINDOW_NORMAL); // Create a window
	cv::moveWindow(windowName, 30, 40);
	cv::resize(image, outImg, cv::Size(), 10, 10, CV_INTER_NN);
	cv::cvtColor(outImg, outImg, cv::COLOR_Lab2BGR);
	cv::imshow(windowName, outImg); // Show our image inside the created window.
	cv::waitKey(0); // Wait for any keystroke in the window
	cv::destroyWindow(windowName); //destroy the created window
	return 0;
}

// Get the average color of all pixels
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

double getStartTemp(cv::Mat image) {
	/*////reshape the image into a single line
	//cv::Mat data = image.reshape(1, image.rows * image.cols * 3);
	//cv::PCA pca = cv::PCA(data, cv::Mat(), CV_PCA_DATA_AS_COL, 3);
	//std::cout << pca.eigenvectors << std::endl;
	//std::cout << pca.eigenvalues << std::endl;

	//split the image into three component channels
	//cv::Mat lab = cv::Mat(1, image.rows * image.cols * 3, CV_32F);
	cv::Mat lab[3];
	cv::split(image, lab);
	//int maxvariance = 

	//perform PCA on each
	for (int channel = 0; channel < 3; channel++) {
		cv::normalize(lab[channel], lab[channel], 0, 255, cv::NORM_MINMAX);
		cv::PCA pca = cv::PCA(lab[channel], cv::Mat(), CV_PCA_DATA_AS_ROW, 2);
		std::cout << pca.eigenvectors << std::endl;
		std::cout << pca.eigenvalues << std::endl;

		//reduce to 1 dimension
		//std::cout << pca.eigenvectors << std::endl;
		//std::cout << pca.eigenvalues << std::endl;
		cv::Mat final = pca.eigenvectors.row(0)*lab[channel].t();

		//calculate the mean and variance

	}*/

	//Assemble a data matrix
	cv::Mat data = image.reshape(1, 3); //switching to (3,1) changes image to 1d color image

	//Using PCA (mean calculated by PCA function)
	cv::PCA pca = cv::PCA(data, cv::Mat(), CV_PCA_DATA_AS_ROW, 3);

	//check the data
	std::cout << pca.eigenvectors << std::endl;
	std::cout << pca.eigenvalues << std::endl;
	std::cout << pca.eigenvalues.type() << std::endl;
	std::cout << pca.eigenvalues.at<float>(0) << std::endl;

	//return the initial temperature 
	return 2*pca.eigenvalues.at<float>(0);

	//to find cv::Mat.type()
	//https://stackoverflow.com/questions/10167534/how-to-find-out-what-type-of-a-mat-object-is-with-mattype-in-opencv

	//using namespace cv;
	////PCA pca = cv::PCA(image, cv::Mat(), CV_PCA_DATA_AC_ROW);
	//return 600.0; //TODO: replace this with actual valid value
}