#include "Palette.h"

Palette::Palette() {}

Palette::Palette(PicImage* img1, SPImage* img2, int size, double cT, cv::Scalar startColor) {
	temp = cT;
	origImage = img1;
	pixelImage = img2;
	curSize = 1;
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
	curSize = 1;
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
			double dist = -abs(colorDist(sPixel.getColor(), y));
			condProb[y] = sPixel.getPaletteProbs()[y] * exp(dist / temp);
			agg += sPixel.getPaletteProbs()[y] * exp(dist / temp);
		}

		// set superpixel to condProb values, and start aggregating for marginal probability P(c_k)
		for (int y = 0; y < curSize; y++) {
			condProb[y] = condProb[y] / agg;
			sPixel.setPaletteProb(y, condProb[y]);
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
			auto pcolor = pixelImage->getPixel(num).getColor();
			double factor = pixelImage->getPixel(num).getPaletteProbs()[x] / pixelImage->numPixels();
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
	std::cout << "Palette.expandPalette() has not yet been implemented.";
	temp *= EXPAND_THRESH_FACTOR;
	if (curSize == maxSize)
		return false;
	return true;
}

void Palette::permutePCA(int pcolor) {
	//get all of the pixels assigned to this color, and store in cv::Mat structure

	//perform pca, and get principal component axis

	//permute palette[pcolor] along axis

	//store results in the palette
}


double Palette::colorDist(cv::Scalar icolor, int pcolor) {
	double dl = icolor[0] - colors[pcolor][REP_COLOR][0];
	double da = icolor[1] - colors[pcolor][REP_COLOR][1];
	double db = icolor[2] - colors[pcolor][REP_COLOR][2];
	return sqrt(pow(dl, 2) + pow(da, 2) + pow(db, 2));
}

double Palette::weight(int num) {
	std::cout << "This capability has not yet been implemented; defaulting to uniform.";
	return 1.0;
}

void Palette::addColor(cv::Scalar newColor) {
	cv::Scalar color1 = cv::Scalar(newColor[0], newColor[1], newColor[2] + 1);
	cv::Scalar startColor1 = cv::Scalar(newColor[0], newColor[1], newColor[2]);
	cv::Scalar color2 = cv::Scalar(newColor[0], newColor[1], newColor[2] - 1);
	colors.push_back({ startColor1, color1, color2 });
}

void Palette::editColor(int num, cv::Scalar newColor) {
	colors[num][0] = cv::Scalar(newColor[0], newColor[1], newColor[2]);
	colors[num][1] = cv::Scalar(newColor[0], newColor[1], newColor[2] + 1);
	colors[num][2] = cv::Scalar(newColor[0], newColor[1], newColor[2] - 1);
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