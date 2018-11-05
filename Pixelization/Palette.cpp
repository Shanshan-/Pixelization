#include "Palette.h"

Palette::Palette() {}

Palette::Palette(Image* img1, SPImage* img2, int size, double cT, cv::Scalar startColor) {
	temp = cT;
	origImage = img1;
	pixelImage = img2;
	curSize = 1;
	maxSize = size;
	colors.reserve(size);
	cv::Scalar color1 = cv::Scalar(startColor[0], startColor[1], startColor[2] + 1);
	cv::Scalar color2 = cv::Scalar(startColor[0], startColor[1], startColor[2] - 1);
	colors.push_back({ startColor, color1, color2 });
}

Palette::Palette(Image* img1, SPImage* img2, int size, double cT, cv::Vec3b startColor) {
	temp = cT;
	origImage = img1;
	pixelImage = img2;
	curSize = 1;
	maxSize = size;
	colors.reserve(size);
	cv::Scalar color1 = cv::Scalar(startColor[0], startColor[1], startColor[2] + 1);
	cv::Scalar startColor1 = cv::Scalar(startColor[0], startColor[1], startColor[2]);
	cv::Scalar color2 = cv::Scalar(startColor[0], startColor[1], startColor[2] - 1);
	colors.push_back({ startColor1, color1, color2 });
}

void Palette::refinePalette() {
	//associate superpixels to colors in the palette
	for (int x = 0; x < pixelImage->numPixels(); x++) {
		cv::Scalar spColor = pixelImage->getPixel(x).getColor();
		for (int y = 0; y < curSize; y++) {
			double dist = colorDist(spColor, y);
		}
	}

	// refine the palette


}

void Palette::expandPalette() {

}


double Palette::colorDist(cv::Scalar icolor, int pcolor) {
	double dl = icolor[0] - colors[pcolor][REP_COLOR][0];
	double da = icolor[1] - colors[pcolor][REP_COLOR][1];
	double db = icolor[2] - colors[pcolor][REP_COLOR][2];
	double dc = sqrt(pow(dl, 2) + pow(da, 2) + pow(db, 2));

}

