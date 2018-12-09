#include "Palette.h"

Palette::Palette() {}

Palette::Palette(PicImage* img1, SPImage* img2, int size, double cT, cv::Scalar startColor1, cv::Scalar startColor2) {
	temp = cT;
	origImage = img1;
	pixelImage = img2;
	curSize = 2;
	maxSize = 2*size;
	colors.reserve(2*size);
	margProbs.reserve(2*size);
	paletteChange = 0.0;

	//add the first color to palette
	colors.push_back({ startColor1 });
	colors.push_back({ startColor2 });
	margProbs.push_back({ 0.5 });
	margProbs.push_back({ 0.5 });
}

Palette::Palette(PicImage* img1, SPImage* img2, int size, double cT, cv::Vec3b startColor1, cv::Vec3b startColor2) {
	temp = cT;
	origImage = img1;
	pixelImage = img2;
	curSize = 2;
	maxSize = 2 * size;
	colors.reserve(2 * size);
	margProbs.reserve(2 * size);
	paletteChange = 0.0;

	//add the first color to palette
	colors.push_back({ (cv::Scalar)startColor1 });
	colors.push_back({ (cv::Scalar)startColor2 });
	margProbs.push_back({ 0.5 });
	margProbs.push_back({ 0.5 });
}

void Palette::associatePalette() { //associate superpixels to colors in the palette
	// calculate the conditional probabillities P(c_k | p_s)
	for (int x = 0; x < pixelImage->numPixels(); x++) {
		auto sPixel = pixelImage->getPixel(x);

		//determine the probability of association with each color given current superpixel
		std::vector<double> condProb = std::vector<double>(curSize, 0.1);
		for (int y = 0; y < curSize; y++) {
			double dist = -abs(colorDist((*sPixel).getAvgColor(), y));
			condProb[y] = (*sPixel).getPaletteProbs()[y] * exp(dist / temp);
		}

		// set superpixel to condProb values, and start aggregating for marginal probability P(c_k)
		for (int y = 0; y < curSize; y++) {
			(*sPixel).setPaletteProb(y, condProb[y]);
		}

		//normalize the probabilities to 1
		sPixel->normPaletteProbs();
	}

	//^v seperated to help with debugging

	// update the marginal probabilities in the palette P(c_k)
	std::vector<double> probs = std::vector<double>(curSize, 0.0);
	for (int y = 0; y < curSize; y++) {
		for (int x = 0; x < pixelImage->numPixels(); x++) {
			probs[y] += pixelImage->getPixel(x)->getPaletteProbs()[y] * weight(x);
		}
	}
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
			auto pcolor = (*pixelImage->getPixel(num)).getAvgColor();
			double factor = (*pixelImage->getPixel(num)).getPaletteProbs()[x] * weight(x);
			agg[0] += pcolor[0] * factor;
			agg[1] += pcolor[1] * factor;
			agg[2] += pcolor[2] * factor;
		}
		for (int num = 0; num < 3; num++) {
			agg[num] = agg[num] / margProbs[x];
			paletteChange += abs((colors[x][num] * 100/255) - (agg[num] * 100/255));
		}
		editColor(x, agg);
	}
}

bool Palette::expandPalette() {
	paletteChange = 0.0;

	//check if any cluster pairs have seperated
	std::vector<std::vector<double>> pairs;
	for (int x = 0; x < curSize; x += 2) {
		double diff = pairDiff(x);
		if (diff > SPLIT_THRESH) {
			pairs.push_back({ diff, x + 0.0 });
		}
	}
	std::sort(pairs.begin(), pairs.end(), [](const std::vector<double> & a, 
		const std::vector<double> & b) { return a[0] < b[0]; });

	//start splitting the pairs
	int ptr = 0;
	while (curSize < maxSize && ptr < int(pairs.size())) {
		//extract the new colors to be used
		int colorNum = int(pairs[ptr][1]);
		cv::Scalar newColor1 = colors[colorNum];
		cv::Scalar newColor2 = colors[colorNum + 1];

		//change associations of superpixel assignments, superPixel condProb
		for (int num = 0; num < pixelImage->numPixels(); num++) {
			auto pixel = pixelImage->getPixel(num);
			pixel->addNewProb(); //will also update superpixel condProb
			pixel->addNewProb();
			if (pixel->getPaletteColor() == colorNum + 1) {
				pixel->setPaletteColor(curSize);
			}
		}

		//update margProb
		margProbs[colorNum] /= 2;
		margProbs.push_back(margProbs[colorNum + 1] / 2);
		margProbs.push_back(margProbs[colorNum + 1] / 2);
		margProbs[colorNum + 1] = margProbs[colorNum];

		//add the second component color to the palette
		colors.push_back({ colors[colorNum + 1][0],colors[colorNum + 1][1],colors[colorNum + 1][2] });
		colors.push_back({ colors[colorNum + 1][0],colors[colorNum + 1][1],colors[colorNum + 1][2] });
		colors[colorNum + 1][0] = colors[colorNum][0];
		colors[colorNum + 1][1] = colors[colorNum][1];
		colors[colorNum + 1][2] = colors[colorNum][2];
		curSize += 2;
		ptr += 2;
	}

	//iterate through all cluster centroids (every even index)
	for (int x = 0; x < curSize; x += 2) {
		//get the average color of the pair
		auto avg = getClusterAvg(x);
		colors[x][0] = avg[0];
		colors[x][1] = avg[1];
		colors[x][2] = avg[2];

		//perform PCA to permute all pairs
		permutePCA(x);
	}
	
	//lower the annealing temperature
	temp *= EXPAND_THRESH_FACTOR;
	if (curSize == maxSize)
		return false;
	return true;
}

void Palette::permutePCA(int pcolor) {
	//get all of the superpixels assigned to this color or its partner
	std::vector<int> assigned;
	for (int num = 0; num < pixelImage->numPixels(); num++) {
		int pIndx = (*pixelImage->getPixel(num)).getPaletteColor();
		if (pIndx == pcolor || pIndx == pcolor + 1) {
			assigned.push_back(num);
		}
	}

	//get pixel data of all pixels in the assigned super pixels
	//cv::Mat data(pixelImage->rows(), pixelImage->cols(), CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat data;
	for (int num = 0; num < origImage->numPixels(); num++) {
		auto pixel = origImage->getPixel(num);
		auto tmp = std::find(assigned.begin(), assigned.end(), pixel->getSpNum());
		if (assigned.end() != tmp) {
			cv::Vec3b color = cv::Vec3b((uchar)(pixel->getColor()[0]),
				(uchar)(pixel->getColor()[1]), (uchar)(pixel->getColor()[2]));
			data.push_back(color);
			//TODO: fix this conversion
			/*int xcoor = int(std::floor(num / pixelImage->cols()));
			int ycoor = num % pixelImage->cols();
			Pixel* pixel = origImage->getPixel(num);
			cv::Vec3b color = cv::Vec3b((uchar)(pixel->getColor()[0]),
				(uchar)(pixel->getColor()[1]), (uchar)(pixel->getColor()[2]));
			data.at<cv::Vec3b>(xcoor, ycoor) = color;*/
		}
	}
	std::cout << data.size() << std::endl;

	if (data.empty()) {
		colors[pcolor + 1][0] = colors[pcolor][0];
		colors[pcolor + 1][1] = colors[pcolor][1];
		colors[pcolor + 1][2] = colors[pcolor][2];
		return;
	}

	//perform pca
	data = data.reshape(1, 3);
	data.convertTo(data, CV_8U);
	cv::PCA pca = cv::PCA(data, cv::Mat(), CV_PCA_DATA_AS_COL);

	//permute palette[pcolor] along axis
	colors[pcolor + 1][0] = colors[pcolor][0] - PCA_FACTOR * pca.eigenvectors.at<float>(0, 0);
	colors[pcolor + 1][1] = colors[pcolor][1] - PCA_FACTOR * pca.eigenvectors.at<float>(0, 1);
	colors[pcolor + 1][2] = colors[pcolor][2] - PCA_FACTOR * pca.eigenvectors.at<float>(0, 2);
	colors[pcolor][0] = colors[pcolor][0] + PCA_FACTOR * pca.eigenvectors.at<float>(0, 0);
	colors[pcolor][1] = colors[pcolor][1] + PCA_FACTOR * pca.eigenvectors.at<float>(0, 1);
	colors[pcolor][2] = colors[pcolor][2] + PCA_FACTOR * pca.eigenvectors.at<float>(0, 2);
}

double Palette::colorDist(cv::Scalar icolor, int pcolor) {
	double dl = (icolor[0] * 100/255) - (colors[pcolor][0] * 100/255);
	double da = icolor[1] - colors[pcolor][1];
	double db = icolor[2] - colors[pcolor][2];
	return sqrt(pow(dl, 2) + pow(da, 2) + pow(db, 2));
}

double Palette::weight(int num) {
	//std::cout << "Palette.weight() has not yet been implemented; defaulting to uniform.\n";
	return 1.0 / pixelImage->numPixels();
}

void Palette::editColor(int num, cv::Scalar newColor) {
	colors[num] = cv::Scalar(newColor[0], newColor[1], newColor[2]);
	/*if (expand) { //setting this value to FALSE saves computation time
		permutePCA(num);
	}*/
}

double Palette::getChange() {
	return paletteChange;
}

std::vector<cv::Scalar>* Palette::getColors() {
	return &colors;
}

int Palette::getCurSize() {
	return curSize;
}

double Palette::getCurTemp() {
	return temp;
}

void Palette::displayPixelImage(int scale, std::string path, bool wait) {
	cv::Mat image(pixelImage->rows(), pixelImage->cols(), CV_8UC3, cv::Scalar(0,0,0));

	for (int x = 0; x < pixelImage->numPixels(); x++) {
		cv::Scalar displayColor = getClusterAvg((*pixelImage->getPixel(x)).getPaletteColor());
		cv::Vec3b tmp = cv::Vec3b((uchar)(displayColor[0]), 
			(uchar)(displayColor[1]), (uchar)(displayColor[2]));
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
	if (wait)
		cv::waitKey(0); // Wait for any keystroke in the window
	else
		cv::waitKey(DISPLAY_TIME); // Wait for time
	cv::destroyWindow(windowName); //destroy the created window

	if (!path.empty()) {
		cv::imwrite(path, image);
	}
}

cv::Scalar Palette::getColor(int pcolor) {
	return colors[pcolor];
}

cv::Scalar Palette::getClusterAvg(int pcolor) {
	cv::Scalar ans = cv::Scalar({ colors[pcolor][0], colors[pcolor][1], colors[pcolor][2] });
	ans[0] = (ans[0] + colors[pcolor][0]) / 2.0;
	ans[1] = (ans[1] + colors[pcolor][1]) / 2.0;
	ans[2] = (ans[2] + colors[pcolor][2]) / 2.0;
	return ans;
}

double Palette::pairDiff(int num) {
	double dl = (colors[num][0] * 100 / 255) - (colors[num+1][0] * 100 / 255);
	double da = colors[num][1] - colors[num + 1][1];
	double db = colors[num][2] - colors[num + 1][2];
	return sqrt(pow(dl, 2) + pow(da, 2) + pow(db, 2));
}