#include "Slic.h"

Slic::Slic() {}

Slic::Slic(PicImage* img1, SPImage* img2) {
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
	auto pImgCent = pixelImage->getCentroids();
	int spSize = pixelImage->getSpSize();
	for (int num = 0; num < pixelImage->numPixels(); num++) {
		std::tuple<double, double> pCent = (*pImgCent)[num];
		int lxBound = std::max(0, int(std::floor(std::get<0>(pCent) - 1.5*spSize)));
		int uxBound = std::min(pixelImage->cols(), int(std::ceil(std::get<0>(pCent) + 1.5*spSize)));
		int lyBound = std::max(0, int(std::floor(std::get<1>(pCent) - 1.5*spSize)));
		int uyBound = std::min(pixelImage->rows(), int(std::ceil(std::get<1>(pCent) + 1.5*spSize)));
		for (int x = lxBound; x < uxBound; x++) {
			for (int y = lyBound; y < uyBound; y++) {
				double pixel_dist = distance(x, y, num);
				if (pixel_dist <= tmp[x][y]) {
					tmp[x][y] = pixel_dist;
					origImage->setSpNum(x, y, num);
				}
			}
		}
	}
	origImage->printAssignments();

	//generate new average positions and colors of each centroid
	tmp.assign(pixelImage->numPixels(), std::vector<double>(6, 0));
	auto spAssignments = origImage->getSpAssigns();
	cv::Mat* img = origImage->getImage();
	for (int pos = 0; pos < origImage->numPixels(); pos++) {
		//auto pix = origImage->getPixel(pos / origImage->cols(), pos % origImage->cols());
		int spNum = (*spAssignments)[pos];
		tmp[spNum][LVAL] += img->at<cv::Vec3b>(pos)[0];		//l value agg
		tmp[spNum][AVAL] += img->at<cv::Vec3b>(pos)[0];		//a value agg
		tmp[spNum][BVAL] += img->at<cv::Vec3b>(pos)[0];		//b value agg
		tmp[spNum][XCOOR] += int(pos/origImage->cols());	//x coor agg
		tmp[spNum][YCOOR] += int(pos%origImage->cols());	//y coor agg
		tmp[spNum][COUNT] += 1;								//counter
	}

	//setup extra image to be used for color smoothing
	cv::Mat fimage = cv::Mat(pixelImage->rows(), pixelImage->cols(), CV_8UC3);

	//adjust centroid locations and color values
	double newPos[2];
	for (int x = 0; x < pixelImage->numPixels(); x++) {
		//get 4-neighbor positions
		double nadj[2] = { 0.0, 0.0 };
		if (x >= pixelImage->cols()) {
			nadj[0] = std::get<0>((*pImgCent)[x - pixelImage->cols()]);
			nadj[1] = std::get<1>((*pImgCent)[x - pixelImage->cols()]);
		}
		else {
			nadj[0] = std::get<0>((*pImgCent)[x]);
			nadj[1] = 2 * std::get<1>((*pImgCent)[x]) - std::get<1>((*pImgCent)[x + pixelImage->cols()]);
		}

		double sadj[2] = { 0.0, 0.0 };
		if (x < pixelImage->numPixels() - pixelImage->cols()) {
			sadj[0] = std::get<0>((*pImgCent)[x + pixelImage->cols()]);
			sadj[1] = std::get<1>((*pImgCent)[x + pixelImage->cols()]);
		}
		else {
			sadj[0] = std::get<0>((*pImgCent)[x]);
			sadj[1] = 2 * std::get<1>((*pImgCent)[x]) - std::get<1>((*pImgCent)[x - pixelImage->cols()]);
		}

		double wadj[2] = { 0.0, 0.0 };
		if (x % pixelImage->cols() != 0) {
			wadj[0] = std::get<0>((*pImgCent)[x - 1]);
			wadj[1] = std::get<1>((*pImgCent)[x - 1]);
		}
		else {
			wadj[0] = 2 * std::get<0>((*pImgCent)[x]) - std::get<0>((*pImgCent)[x + 1]);
			wadj[1] = std::get<1>((*pImgCent)[x]);
		}

		double eadj[2] = { 0.0, 0.0 };
		if (x % pixelImage->cols() != pixelImage->cols() - 1) {
			eadj[0] = std::get<0>((*pImgCent)[x + 1]);
			eadj[1] = std::get<1>((*pImgCent)[x + 1]);
		}
		else {
			eadj[0] = 2 * std::get<0>((*pImgCent)[x]) - std::get<0>((*pImgCent)[x - 1]);
			eadj[1] = std::get<1>((*pImgCent)[x]);
		}


		//get average position and laplacian smooth towards grid structure
		int ratio[] = RATIO; //used for section formula
		double tmpPos[] = { tmp[x][XCOOR] / tmp[x][COUNT], tmp[x][YCOOR] / tmp[x][COUNT] };
		double avgPos[] = { (nadj[0] + sadj[0] + wadj[0] + eadj[0]) / 4, (nadj[1] + sadj[1] + wadj[1] + eadj[1]) / 4 };
		newPos[0] = (ratio[0] * avgPos[0] + ratio[1] * tmpPos[0]) / (ratio[0] + ratio[1]);
		newPos[1] = (ratio[0] * avgPos[1] + ratio[1] * tmpPos[1]) / (ratio[0] + ratio[1]);
		//TODO: the image centroids never seem to be set anywhere

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

	//workingImage = fimage2;
}

double Slic::distance(int pixelx, int pixely, int spVal) {
	//auto imgPixel = origImage->getPixel(pixelx, pixely);
	//auto spPixel = pixelImage->getPixel(spVal);

	//find the color distance
	auto color1 = (*origImage->getImage()).at<cv::Vec3b>(pixelx, pixely);
	auto color2 = (*pixelImage->getAvgColors()).at<cv::Vec3b>(spVal);
	double dl = color1[0] - color2[0];
	double da = color1[1] - color2[1];
	double db = color1[2] - color2[2];
	double dc = sqrt(pow(dl, 2) + pow(da, 2) + pow(db, 2));

	//find the positional difference
	auto centroids = pixelImage->getCentroids();
	double dx = std::get<0>((*centroids)[spVal]) - pixelx;
	double dy = std::get<1>((*centroids)[spVal]) - pixely;
	double dp = std::hypot(dx, dy);

	//find total distance
	return dc + 45 * sqrt(pixelImage->numPixels() / origImage->numPixels()) * dp;
}
