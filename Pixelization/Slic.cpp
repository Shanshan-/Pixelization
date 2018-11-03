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
		std::vector<double> spCoor = pixelImage->getPixel(num).getImgCoor();
		int lxBound = std::max(0, int(spCoor[0] - 1.5*pixelImage->getSpSize()));
		int uxBound = std::min(pixelImage->cols(), int(std::ceil(spCoor[0] + 1.5*pixelImage->getSpSize())));
		int lyBound = std::max(0, int(std::floor(spCoor[1] - 1.5*pixelImage->getSpSize())));
		int uyBound = std::min(pixelImage->rows(), int(std::ceil(spCoor[1] + 1.5*pixelImage->getSpSize())));
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

	////generate average positions and colors of each centroid
	//tmp.assign(centers.size(), std::vector<double>(6, 0));
	//for (int pos = 0; pos < numRows*numCols; pos++) {
	//	int spGroup = spNum[std::floor(pos / numRows)][pos % numRows];
	//	tmp[pos][0] = color[spGroup][0];
	//	tmp[pos][1] = color[spGroup][1];
	//	tmp[pos][2] = color[spGroup][2];
	//	tmp[pos][3] = std::floor(pos / numRows);
	//	tmp[pos][4] = pos % numRows;
	//	tmp[pos][5] += 1;
	//}

	////adjust centroid locations and color valules
	//for (int x = 0; x < centers.size(); x++) {
	//	//get 4-neighbor positions
	//	double nadj[2], sadj[2], wadj[2], eadj[2];
	//	if (x >= numSCols)
	//		double nadj[] = { centers[x - numSCols][0], centers[x - numSCols][1] };
	//	else
	//		double nadj[] = { 2 * tmp[x][0] - centers[x + numSCols][0], 2 * tmp[x][1] - centers[x + numSCols][1] };
	//	if (x < centers.size() - numSCols)
	//		double sadj[] = { centers[x + numSCols][0], centers[x + numSCols][1] };
	//	else
	//		double sadj[] = { 2 * tmp[x][0] - centers[x - numSCols][0], 2 * tmp[x][0] - centers[x - numSCols][1] };
	//	if (x % numSCols != 0)
	//		double wadj[] = { centers[x - numSRows][0], centers[x - numSRows][1] };
	//	else
	//		double wadj[] = { 2 * tmp[x][0] - centers[x + numSRows][0], 2 * tmp[x][0] - centers[x + numSRows][1] };
	//	if (x % numSCols == numSCols - 1)
	//		double eadj[] = { centers[x + numSRows][0], centers[x + numSRows][1] };
	//	else
	//		double eadj[] = { 2 * tmp[x][0] - centers[x - numSRows][0], 2 * tmp[x][0] - centers[x - numSRows][1] };


	//	//get average position and laplacian smooth towards grid structure
	//	double newPos[] = { tmp[x][3] / tmp[x][5], tmp[x][4] / tmp[x][5] };
	//	double avgPos[] = { (nadj[0] + sadj[0] + wadj[0] + eadj[0]) / 4, (nadj[1] + sadj[1] + wadj[1] + eadj[1]) / 4 };

		//get average color and smooth
	//}

	//smooth color representatives of super pixels
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
	double dx = spPixel.getImgCoor()[0] - pixelx;
	double dy = spPixel.getImgCoor()[1] - pixely;
	double dp = std::hypot(dx, dy);

	//find total distance
	return dc + 45 * sqrt(pixelImage->numPixels() / origImage->numPixels()) * dp;
}
