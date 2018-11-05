#include "Slic.h"

Slic::Slic() {}

Slic::Slic(Image* img1, SPImage* img2) {
	origImage = img1;
	pixelImage = img2;
}

void Slic::refineSP() { //runs one step of SLIC superpixel refinement
	//TODO: fill in the refinement process from section 4.3, or contents of SLIC paper

	//generate storage vector to be used (to store distances and average for calculation purposes)
	std::vector<std::vector<double>> tmp;
	tmp.assign(origImage->rows(), std::vector<double>(origImage->cols(), INT_MAX));

	//assign new superpixel values based on distance
	//TODO: fix the offset problem because of pixels and centers not lining up
	for (int num = 0; num < pixelImage->numPixels(); num++) {
		auto sp = pixelImage->getPixel(num);
		int lxBound = std::max(0, int(sp.getImgXCoor() - 1.5*pixelImage->getSpSize()));
		int uxBound = std::min(pixelImage->cols(), int(std::ceil(sp.getImgXCoor() + 1.5*pixelImage->getSpSize())));
		int lyBound = std::max(0, int(std::floor(sp.getImgYCoor() - 1.5*pixelImage->getSpSize())));
		int uyBound = std::min(pixelImage->rows(), int(std::ceil(sp.getImgYCoor() + 1.5*pixelImage->getSpSize())));
		for (int x = lxBound; x < uxBound; x++) {
			for (int y = lyBound; y < uyBound; y++) {
				double pixel_dist = distance(x, y, num);
				if (pixel_dist <= tmp[x][y]) {
					tmp[x][y] = pixel_dist;
					origImage->getPixel(num / origImage->cols(), num % origImage->cols()).setSpNum(num);
				}
			}
		}
	}
	origImage->printAssignments();

	//generate new average positions and colors of each centroid
	tmp.assign(pixelImage->numPixels(), std::vector<double>(6, 0));
	for (int pos = 0; pos < origImage->numPixels(); pos++) {
		auto pix = origImage->getPixel(pos / origImage->cols(), pos % origImage->cols());
		tmp[pix.getSpNum()][LVAL] += pix.getColor()[0];		//l value agg
		tmp[pix.getSpNum()][AVAL] += pix.getColor()[1];		//a value agg
		tmp[pix.getSpNum()][BVAL] += pix.getColor()[2];		//b value agg
		tmp[pix.getSpNum()][XCOOR] += pix.getXCoor();		//x coor agg
		tmp[pix.getSpNum()][YCOOR] += pix.getYCoor();		//y coor agg
		tmp[pix.getSpNum()][COUNT] += 1;					//counter
	}

	//setup extra image to be used for color smoothing
	cv::Mat fimage = cv::Mat(pixelImage->rows(), pixelImage->cols(), CV_8UC3);

	//adjust centroid locations and color values
	double newPos[2];
	for (int x = 0; x < pixelImage->numPixels(); x++) {
		//get 4-neighbor positions
		double nadj[2] = { 0.0, 0.0 };
		if (x >= pixelImage->cols()) {
			nadj[0] = pixelImage->getPixel(x - pixelImage->cols()).getImgXCoor();
			nadj[1] = pixelImage->getPixel(x - pixelImage->cols()).getImgYCoor();
		}
		else {
			nadj[0] = 2 * pixelImage->getPixel(x).getImgXCoor() - pixelImage->getPixel(x + pixelImage->cols()).getImgXCoor();
			nadj[1] = 2 * pixelImage->getPixel(x).getImgYCoor() - pixelImage->getPixel(x + pixelImage->cols()).getImgYCoor();
		}

		double sadj[2] = { 0.0, 0.0 };
		if (x < pixelImage->numPixels() - pixelImage->cols()) {
			sadj[0] = pixelImage->getPixel(x + pixelImage->cols()).getImgXCoor();
			sadj[1] = pixelImage->getPixel(x + pixelImage->cols()).getImgYCoor();
		}
		else {
			sadj[0] = 2 * pixelImage->getPixel(x).getImgXCoor() - pixelImage->getPixel(x - pixelImage->cols()).getImgXCoor();
			sadj[1] = 2 * pixelImage->getPixel(x).getImgYCoor() - pixelImage->getPixel(x - pixelImage->cols()).getImgYCoor();
		}

		double wadj[2] = { 0.0, 0.0 };
		if (x % pixelImage->cols() != 0) {
			wadj[0] = pixelImage->getPixel(x - 1).getImgXCoor();
			wadj[1] = pixelImage->getPixel(x - 1).getImgYCoor();
		}
		else {
			wadj[0] = 2 * pixelImage->getPixel(x).getImgXCoor() - pixelImage->getPixel(x + 1).getImgXCoor();
			wadj[1] = 2 * pixelImage->getPixel(x).getImgYCoor() - pixelImage->getPixel(x + 1).getImgYCoor();
		}

		double eadj[2] = { 0.0, 0.0 };
		if (x % pixelImage->cols() != pixelImage->cols() - 1) {
			eadj[0] = pixelImage->getPixel(x + 1).getImgXCoor();
			eadj[1] = pixelImage->getPixel(x + 1).getImgYCoor();
		}
		else {
			eadj[0] = 2 * pixelImage->getPixel(x).getImgXCoor() - pixelImage->getPixel(x - 1).getImgXCoor();
			eadj[1] = 2 * pixelImage->getPixel(x).getImgYCoor() - pixelImage->getPixel(x - 1).getImgYCoor();
		}


		//get average position and laplacian smooth towards grid structure
		int ratio[] = RATIO; //used for section formula
		double tmpPos[] = { tmp[x][XCOOR] / tmp[x][COUNT], tmp[x][YCOOR] / tmp[x][COUNT] };
		double avgPos[] = { (nadj[0] + sadj[0] + wadj[0] + eadj[0]) / 4, (nadj[1] + sadj[1] + wadj[1] + eadj[1]) / 4 };
		newPos[0] = (ratio[0] * avgPos[0] + ratio[1] * tmpPos[0]) / (ratio[0] + ratio[1]);
		newPos[1] = (ratio[0] * avgPos[1] + ratio[1] * tmpPos[1]) / (ratio[0] + ratio[1]);

		//get average color and add to filter image
		cv::Scalar avgColor = cv::Scalar(tmp[x][LVAL] / tmp[x][COUNT], tmp[x][AVAL] / tmp[x][COUNT], tmp[x][BVAL] / tmp[x][COUNT]);
		cv::Vec3b color = cv::Vec3b((uchar)avgColor[0], (uchar)avgColor[1], (uchar)avgColor[2]);
		int tmpx = int(x / pixelImage->cols());
		int tmpy = x % pixelImage->cols();
		fimage.at<cv::Vec3b>(tmpx, tmpy) = color;
	}

	//bilaterally filter fimage //Note: difference better noticible at larger scales/images
	int fd = pixelImage->getSpSize();
	cv::Mat fimage2;
	cv::bilateralFilter(fimage, fimage2, fd, FD_MULT*fd, FD_MULT*fd);
	//TODO: program will break here if fd and 10*fd not factors of fimage.shape

	//assign superpixel values (color and center)
	for (int x = 0; x < pixelImage->numPixels(); x++) {
		auto spixel = pixelImage->getPixel(x);
		spixel.setCentroid(newPos[0], newPos[1]);
		spixel.setColor(fimage2.at<cv::Vec3b>(x / pixelImage->cols(), x % pixelImage->cols()));
	}

	//preview image
	cv::Mat outImg;
	cv::String windowName = "Image Results"; //Name of the window
	cv::namedWindow(windowName); // , cv::WINDOW_NORMAL); // Create a window
	cv::moveWindow(windowName, 30, 40);
	cv::resize(fimage, outImg, cv::Size(), SCALE, SCALE, CV_INTER_NN);
	cv::cvtColor(outImg, outImg, cv::COLOR_Lab2BGR);
	cv::imshow(windowName, outImg); // Show our image inside the created window.
	cv::waitKey(0); // Wait for any keystroke in the window
	cv::destroyWindow(windowName); //destroy the created window

	cv::namedWindow(windowName); // , cv::WINDOW_NORMAL); // Create a window
	cv::moveWindow(windowName, 30, 40);
	cv::resize(fimage2, outImg, cv::Size(), SCALE, SCALE, CV_INTER_NN);
	cv::cvtColor(outImg, outImg, cv::COLOR_Lab2BGR);
	cv::imshow(windowName, outImg); // Show our image inside the created window.
	cv::waitKey(0); // Wait for any keystroke in the window
	cv::destroyWindow(windowName); //destroy the created window

	workingImage = fimage2;
}

double Slic::distance(int pixelx, int pixely, int spVal) {
	auto imgPixel = origImage->getPixel(pixelx, pixely);
	auto spPixel = pixelImage->getPixel(spVal);

	//find the color distance
	auto color1 = imgPixel.getColor();
	auto color2 = spPixel.getColor();
	double dl = color1[0] - color2[0];
	double da = color1[1] - color2[1];
	double db = color1[2] - color2[2];
	double dc = sqrt(pow(dl, 2) + pow(da, 2) + pow(db, 2));

	//find the positional difference
	double dx = spPixel.getImgXCoor() - pixelx;
	double dy = spPixel.getImgYCoor() - pixely;
	double dp = std::hypot(dx, dy);

	//find total distance
	return dc + 45 * sqrt(pixelImage->numPixels() / origImage->numPixels()) * dp;
}
