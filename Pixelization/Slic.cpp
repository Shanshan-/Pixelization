#include "Slic.h"

Slic::Slic(int rows, int cols, int pixSize, cv::Scalar defVal, cv::Mat *img) {
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
			int tmp1 = std::floor(x / pixSize) * sCols;
			int tmp2 = std::floor(y / pixSize);
			spNum[x][y] = tmp1 + tmp2;
		}
	}

	//generate the superpixel centers
	double offset = pixSize / 2;
	for (int x = 0; x < sRows*sCols; x++) {
		double centerx = offset + std::floor(x / sCols) * pixSize;
		double centery = offset + (x % sCols) * pixSize;
		centers[x] = { centerx, centery };
	}
	printCenters();

	//initialize all other variables
	image = img;
	numRows = rows;
	numCols = cols;
	numSRows = sRows;
	numSCols = sCols;
	pixelSize = pixSize;
}

double Slic::distance(int pixelx, int pixely, int centroid) {
	//find the color distance
	//std::cout << image << std::endl; //gives address of image
	//std::cout << &image << std::endl; //gives address of image variable
	//std::cout << *image << std::endl; //gives content of image
	cv::Mat img = *image;
	auto pixelColor = img.at<cv::Vec3b>(pixelx, pixely);
	double dl = pixelColor[0] - color[centroid][0];
	double da = pixelColor[1] - color[centroid][1];
	double db = pixelColor[2] - color[centroid][2];
	double dc = sqrt(pow(dl, 2) + pow(da, 2) + pow(db, 2));

	//find the positional difference
	double dx = centers[centroid][0] - pixelx;
	double dy = centers[centroid][1] - pixely;
	double dp = std::hypot(dx, dy);

	//find total distance
	return dc + 45 * pow(pixelSize, 2) * dp;
}

void Slic::refine() { //runs one step of SLIC superpixel refinement
	//TODO: fill in the refinement process from section 4.3, or contents of SLIC paper
	// could also potentially move this to be outside this class

	//generate storage vector to be used (to store distances and sverage for calculation purposes)
	std::vector<std::vector<double>> tmp;
	tmp.assign(numRows, std::vector<double>(numCols, INT_MAX));

	//assign new superpixel values based on distance
	//TODO: fix the offset problem because of pixels and centers not lining up
	for (int center = 0; center < centers.size(); center++) {
		int lxBound = std::max(0, int(std::floor(centers[center][0] - 1.5*pixelSize)));
		int uxBound = std::min(numCols, int(std::ceil(centers[center][0] + 1.5*pixelSize)));
		int lyBound = std::max(0, int(std::floor(centers[center][1] - 1.5*pixelSize)));
		int uyBound = std::min(numRows, int(std::ceil(centers[center][1] + 1.5*pixelSize)));
		for (int x = lxBound; x < uxBound; x++) {
			for (int y = lyBound; y < uyBound; y++) {
				double pixel_dist = distance(x, y, center);
				if (pixel_dist <= tmp[x][y]) {
					tmp[x][y] = pixel_dist;
					spNum[x][y] = center;
				}
			}
		}
	}
	printAssignments();

	//generate average positions and colors of each centroid
	tmp.assign(centers.size(), std::vector<double>(6, 0));
	for (int pos = 0; pos < numRows*numCols; pos++) {
		int spGroup = spNum[std::floor(pos / numRows)][pos % numRows];
		tmp[pos][0] = color[spGroup][0];
		tmp[pos][1] = color[spGroup][1];
		tmp[pos][2] = color[spGroup][2];
		tmp[pos][3] = std::floor(pos / numRows);
		tmp[pos][4] = pos % numRows;
		tmp[pos][5] += 1;
	}

	//adjust centroid locations and color valules
	for (int x = 0; x < centers.size(); x++) {
		//get 4-neighbor positions
		double nadj[2], sadj[2], wadj[2], eadj[2];
		if (x >= numSCols)
			double nadj[] = { centers[x - numSCols][0], centers[x - numSCols][1] };
		else
			double nadj[] = { 2 * tmp[x][0] - centers[x + numSCols][0], 2 * tmp[x][1] - centers[x + numSCols][1] };
		if (x < centers.size() - numSCols)
			double sadj[] = { centers[x + numSCols][0], centers[x + numSCols][1] };
		else
			double sadj[] = { 2 * tmp[x][0] - centers[x - numSCols][0], 2 * tmp[x][0] - centers[x - numSCols][1] };
		if (x % numSCols != 0)
			double wadj[] = { centers[x - numSRows][0], centers[x - numSRows][1] };
		else
			double wadj[] = { 2 * tmp[x][0] - centers[x + numSRows][0], 2 * tmp[x][0] - centers[x + numSRows][1] };
		if (x % numSCols == numSCols - 1)
			double eadj[] = { centers[x + numSRows][0], centers[x + numSRows][1] };
		else
			double eadj[] = { 2 * tmp[x][0] - centers[x - numSRows][0], 2 * tmp[x][0] - centers[x - numSRows][1] };


		//get average position and laplacian smooth towards grid structure
		double newPos[] = { tmp[x][3] / tmp[x][5], tmp[x][4] / tmp[x][5] };
		double avgPos[] = { (nadj[0] + sadj[0] + wadj[0] + eadj[0]) / 4, (nadj[1] + sadj[1] + wadj[1] + eadj[1]) / 4 };

		//get average color and smooth
	}

	//smooth color representatives of super pixels
}

void Slic::printAssignments() {
	for (int x = 0; x < spNum.size(); x++) {
		for (int y = 0; y < spNum[0].size(); y++) {
			std::cout << spNum[x][y] << "\t";
		}
		std::cout << std::endl;
	}
}

void Slic::printCenters() {
	for (int x = 0; x < centers.size(); x++) {
		std::cout << "(" << centers[x][0] << "," << centers[x][1] << ")" << std::endl;
	}
}
