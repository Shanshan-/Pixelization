#include "Palette.h"

Palette::Palette() {}

Palette::Palette(PicImage* img1, SPImage* img2, int size, double cT, cv::Scalar startColor) {
	temp = cT;
	origImage = img1;
	pixelImage = img2;
	curSize = 0;
	maxSize = size;
	colors.reserve(size);
	margProbs.reserve(size);
	margProbs.push_back({ 0.0 });
	addColor(startColor);
	paletteChange = 0.0;
}

Palette::Palette(PicImage* img1, SPImage* img2, int size, double cT, cv::Vec3b startColor) {
	temp = cT;
	origImage = img1;
	pixelImage = img2;
	curSize = 0;
	maxSize = size;
	colors.reserve(size);
	margProbs.reserve(size);
	margProbs.push_back({ 0.0 });
	addColor((cv::Scalar)startColor);
	paletteChange = 0.0;
}

void Palette::associatePalette() {
	//associate superpixels to colors in the palette
	std::vector<double> probs = std::vector<double>(curSize, 0.0);
	for (int x = 0; x < pixelImage->numPixels(); x++) {
		auto sPixel = pixelImage->getPixel(x);

		// calculate the conditional probabillities P(c_k | p_s)
		std::vector<double> condProb = std::vector<double>(curSize, 0.1);
		double agg = 0.0;
		for (int y = 0; y < curSize; y++) {
			double dist = -abs(colorDist((*sPixel).getColor(), y));
			condProb[y] = (*sPixel).getPaletteProbs()[y] * exp(dist / temp);
			agg += condProb[y];
		}

		// set superpixel to condProb values, and start aggregating for marginal probability P(c_k)
		for (int y = 0; y < curSize; y++) {
			condProb[y] = condProb[y] / agg;
			(*sPixel).setPaletteProb(y, condProb[y]);
			probs[y] += condProb[y] * weight(y);
		}

	}

	// update the marginal probabilities in the palette
	for (int x = 0; x < curSize; x++) {
		margProbs[x] = probs[x];
	}
}

void Palette::refinePalette() {
	//clear the change counter for updating
	paletteChange = 0.0;

	// refine the palette
	for (int x = 0; x < curSize; x++) {
		cv::Scalar agg = cv::Scalar(0.0, 0.0, 0.0);
		for (int num = 0; num < pixelImage->numPixels(); num++) {
			auto pcolor = (*pixelImage->getPixel(num)).getColor();
			double factor = (*pixelImage->getPixel(num)).getPaletteProbs()[x] / pixelImage->numPixels();
			agg[0] += pcolor[0];
			agg[1] += pcolor[1];
			agg[2] += pcolor[2];
		}
		for (int num = 0; num < 3; num++) {
			agg[num] /= margProbs[x];
			paletteChange += abs(colors[x][REP_COLOR][num] - agg[num]);
		}
		editColor(x, agg);
	}
}

bool Palette::expandPalette() {
	//std::cout << "Palette.expandPalette() has not yet been implemented.";
	pcaAll();

	//find the palette color with the largest variance
	int maxColor = 0;
	for (int x = 1; x < curSize; x++) {
		if (colors[x][VARIANCE][0] > colors[maxColor][VARIANCE][0]) {
			maxColor = x;
		}
	}

	//add the two component colors to the palette, and remove the old
	addColor(colors[maxColor][CL2_COLOR]);
	editColor(maxColor, colors[maxColor][CL1_COLOR]);

	//lower the annealing temperature
	temp *= EXPAND_THRESH_FACTOR;
	if (curSize == maxSize)
		return false;
	return true;
}

void Palette::permutePCA(int pcolor) {
	//get all of the superpixels assigned to this color
	std::vector<int> assigned;
	for (int num = 0; num < pixelImage->numPixels(); num++) {
		if ((*pixelImage->getPixel(num)).getPaletteColor() == pcolor) {
			assigned.push_back(num);
		}
	}

	//get pixel data of all pixels in the assigned super pixels
	cv::Mat data;
	for (int num = 0; num < origImage->numPixels(); num++) {
		auto tmp = std::find(assigned.begin(), assigned.end(), (*origImage->getPixel(num)).getSpNum());
		if (assigned.end() != tmp) {
			data.push_back((*origImage->getPixel(num)).getColor());
		}
	}

	//perform pca
	data = data.reshape(1, 3);
	data.convertTo(data, CV_8U);
	cv::PCA pca = cv::PCA(data, cv::Mat(), CV_PCA_DATA_AS_COL);

	//permute palette[pcolor] along axis
	auto color1 = colors[pcolor][REP_COLOR];
	colors[pcolor][CL1_COLOR][0] = color1[0] + pca.eigenvectors.at<float>(0, 0);
	colors[pcolor][CL1_COLOR][1] = color1[1] + pca.eigenvectors.at<float>(0, 1);
	colors[pcolor][CL1_COLOR][2] = color1[2] + pca.eigenvectors.at<float>(0, 2);
	colors[pcolor][CL2_COLOR][0] = color1[0] - pca.eigenvectors.at<float>(0, 0);
	colors[pcolor][CL2_COLOR][1] = color1[1] - pca.eigenvectors.at<float>(0, 1);
	colors[pcolor][CL2_COLOR][2] = color1[2] - pca.eigenvectors.at<float>(0, 2);
	colors[pcolor][VARIANCE] = pca.eigenvalues.at<float>(0);
}

double Palette::colorDist(cv::Scalar icolor, int pcolor) {
	double dl = icolor[0] - colors[pcolor][REP_COLOR][0];
	double da = icolor[1] - colors[pcolor][REP_COLOR][1];
	double db = icolor[2] - colors[pcolor][REP_COLOR][2];
	return sqrt(pow(dl, 2) + pow(da, 2) + pow(db, 2));
}

double Palette::weight(int num) {
	//std::cout << "Palette.weight() has not yet been implemented; defaulting to uniform.\n";
	return 1.0;
}

void Palette::addColor(cv::Scalar newColor) {
	cv::Scalar startColor1 = cv::Scalar(newColor[0], newColor[1], newColor[2]);
	colors.push_back({ startColor1, startColor1, startColor1, 0 });
	curSize += 1;
	if (curSize == 1) {
		return;
	}

	//update superpixel probability vectors
	for (int num = 0; num < pixelImage->numPixels(); num++) {
		(*pixelImage->getPixel(num)).addNewProb();
	}

	//update marginal probabilities
	margProbs.push_back(0.0);
	for (int x = 0; x < curSize; x++) {
		margProbs[x] = 1 / curSize;
	}

	//permutePCA(colors.size() - 1);
}

void Palette::editColor(int num, cv::Scalar newColor) {
	colors[num][0] = cv::Scalar(newColor[0], newColor[1], newColor[2]);
	/*if (expand) { //setting this value to FALSE saves computation time
		permutePCA(num);
	}*/
}

void Palette::pcaAll() {
	for (int x = 0; x < curSize; x++) {
		permutePCA(x);
	}
}

double Palette::getChange() {
	return paletteChange;
}

std::vector<std::vector<cv::Scalar>> Palette::getColors() {
	return colors;
}

int Palette::getCurSize() {
	return curSize;
}

double Palette::getCurTemp() {
	return temp;
}

void Palette::displayPixelImage(int scale) {
	cv::Mat image(pixelImage->rows(), pixelImage->cols(), CV_8UC3, cv::Scalar(0,0,0));

	for (int x = 0; x < pixelImage->numPixels(); x++) {
		cv::Vec3b tmp = cv::Vec3b(
			(uchar)(colors[(*pixelImage->getPixel(x)).getPaletteColor()][REP_COLOR][0]),
			(uchar)(colors[(*pixelImage->getPixel(x)).getPaletteColor()][REP_COLOR][1]),
			(uchar)(colors[(*pixelImage->getPixel(x)).getPaletteColor()][REP_COLOR][2]));
		int col = x % pixelImage->cols();
		int row = int((x + 0.0) / (pixelImage->cols()));
 		image.at<cv::Vec3b>(row, col) = tmp;
	}

	cv::String windowName = "Image Results"; //Name of the window
	cv::namedWindow(windowName); // , cv::WINDOW_NORMAL); // Create a window
	cv::moveWindow(windowName, 30, 40);
	cv::resize(image, image, cv::Size(), scale, scale, CV_INTER_NN);
	cv::cvtColor(image, image, cv::COLOR_Lab2BGR);
	cv::imshow(windowName, image); // Show our image inside the created window.
	cv::waitKey(1500); // Wait for any keystroke in the window
	cv::destroyWindow(windowName); //destroy the created window
}