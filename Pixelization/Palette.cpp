#include "Palette.h"

Palette::Palette() {}

Palette::Palette(Image* img1, SPImage* img2, int size, double cT, cv::Scalar startColor) {
	temp = cT;
	origImage = img1;
	pixelImage = img2;
	colors.reserve(size);
	cv::Scalar color1 = cv::Scalar(startColor[0], startColor[1], startColor[2] + 1);
	cv::Scalar color2 = cv::Scalar(startColor[0], startColor[1], startColor[2] - 1);
	colors.push_back({ color1, color2 });
}

Palette::Palette(Image* img1, SPImage* img2, int size, double cT, cv::Vec3b startColor) {
	temp = cT;
	origImage = img1;
	pixelImage = img2;
	colors.reserve(size);
	cv::Scalar color1 = cv::Scalar(startColor[0], startColor[1], startColor[2] + 1);
	cv::Scalar color2 = cv::Scalar(startColor[0], startColor[1], startColor[2] - 1);
	colors.push_back({ color1, color2 });
}

void Palette::refinePalette() {
	//associate superpixels to colors in the palette

	// refine the palette


}

void Palette::expandPalette() {

}

