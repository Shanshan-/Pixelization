#include <GL\glew.h>
#include <GL\freeglut.h>
#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>
#include <math.h>

#define IMG_PATH "../Images/"

class SPAssignments {
private:
	std::vector<std::vector<int>> spNum; //stores the superpixel assignments of each pixel
	std::vector<cv::Scalar> color; //stores the color value of each superpixel
	std::vector<std::vector<double>> centers; //stores all of the superpixel centroid coordinates
	cv::Mat *image;
	int numRows;
	int numCols;
	int pixelSize;
	
public:
	SPAssignments(int rows, int cols, int pixSize, cv::Scalar defVal, cv::Mat *img) {
		//std::cout << img << std::endl; //outputs the address of the image
		//std::cout << &img << std::endl; //outputs storage location of the variable
		//initialize variables
		int sRows = std::floor(rows / pixSize);
		int sCols = std::floor(cols / pixSize);
		spNum.assign(rows, std::vector<int>(cols, -1));
		color.assign(sRows*sCols, cv::Scalar(defVal));
		centers.assign(sRows*sCols, std::vector<double>(2, 0.0));

		//set initial values for the spNum matrix
		for (int x = 0; x < rows; x++) {
			for (int y = 0; y < cols; y++) {
				int tmp1 = std::floor(x/pixSize) * sCols;
				int tmp2 = std::floor(y/pixSize);
				spNum[x][y] = tmp1 + tmp2;
			}
		}

		//generate the superpixel centers
		double offset = pixSize / 2;
		for (int x = 0; x < sRows*sCols; x++) {
			centers[x] = { offset + std::floor(x / pixSize) * pixSize, offset + (x % pixSize) * pixSize };
		}

		//initialize all other variables
		image = img;
		numRows = rows;
		numCols = cols;
		pixelSize = pixSize;
	}

	double distance(int pixelx, int pixely, int centroid) {
		//find the color distance
		//std::cout << image << std::endl; //gives address of image
		//std::cout << &image << std::endl; //gives address of image variable
		//std::cout << *image << std::endl; //gives content of image
		//cv::Mat img = *image;
		auto pixelColor = (*image).at<cv::Vec3b>(pixelx, pixely);
		double dl = pixelColor[0] - color[centroid][0];
		double da = pixelColor[1] - color[centroid][1];
		double db = pixelColor[2] - color[centroid][2];
		double dc = sqrt(pow(dl, 2) + pow(da, 2) + pow(db, 2));

		//find the positional difference
		double dx = centers[centroid][0] - pixelx;
		double dy = centers[centroid][1] - pixely;
		double dp = sqrt(pow(dx, 2) + pow(dy, 2));

		//find total distance
		return dc + 45 * pow(pixelSize, 2) * dp;
	}

	void refine() { //runs one step of SLIC superpixel refinement
		//TODO: fill in the refinement process from section 4.3, or contents of SLIC paper
		// could also potentially move this to be outside this class

		//generate distance vector to be used
		std::vector<std::vector<double>> dist;
		dist.assign(numRows, std::vector<double>(numCols, INT_MAX));

		//assign new superpixel values based on distance
		for (int center = 0; center < centers.size(); center++) {
			int lxBound = std::floor(centers[center][0] - 1.5*pixelSize);
			int uxBound = std::ceil(centers[center][0] + 1.5*pixelSize);
			int lyBound = std::floor(centers[center][1] - 1.5*pixelSize);
			int uyBound = std::ceil(centers[center][1] + 1.5*pixelSize);
			for (int x = lxBound; x < uxBound; x++) {
				for (int y = lyBound; y < uyBound; y++) {
					double pixel_dist = distance(x, y, center);
					if (pixel_dist < dist[x][y]) {
						dist[x][y] = pixel_dist;
						spNum[x][y] = center;
					}
				}
			}
		}

		//adjust centroid positions based on SLIC algorithm

		//laplacian smooth centroid positions towards grid structure

		//smooth color representatives of super pixels
	}

	void printAssignments() {
		for (int x = 0; x < spNum.size(); x++) {
			for (int y = 0; y < spNum[0].size(); y++) {
				std::cout << spNum[x][y];
			}
			std::cout << std::endl;
		}
	}

	/* Other potential functions:
		- displaying current palette assignments (using glut and glew)
		- getters for each of three storage vectors
	*/
};

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
	cv::Mat image = cv::imread(IMG_PATH "table.jpg");
	if (image.empty()) {
		std::cout << "Could not open or find the image" << std::endl;
		exit(-1);
	}
	//std::cout << &image << std::endl;

	//determine the mean color of image, and generate the palette
	cv::Scalar meanColor = getMeanColor(image);
	std::vector<cv::Scalar> palette(paletteSize);
	SPAssignments sp = SPAssignments(image.rows, image.cols, 10, meanColor, &image);
	SPAssignments sp1 = SPAssignments(8, 4, 2, meanColor, &image);
	sp1.printAssignments();
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